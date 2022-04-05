#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <sys/timerfd.h>

#include "ipc_protocol.h"


#define SOCKET_NAME "/tmp/9Lq7BNBnBycd6nxy.socket"
#define BUFFER_SIZE (1024*64)

#include <glib.h>
#include <gio/gio.h>

typedef struct s_message_client {
	GIOChannel *source;
	guint id;
} message_client;

typedef struct s_message_clients {
	GList *clientlist;
	
} message_clients;

gboolean clientchannel_socket_event (	GIOChannel *source,
								GIOCondition condition,
								gpointer data)
{
	int datafd = -1;

	if ((condition & (G_IO_ERR | G_IO_HUP)) != 0) {	 //Client side socket was closed.
		GList* listptr = NULL;
		message_clients *serverinfo = (message_clients*)data;
		message_client *cliinfo = NULL;
		
		listptr = g_list_first(serverinfo->clientlist);
		
		for (int i=0; i < 1000;i++)	{	//loop limit
			if (listptr == NULL) break;
			
			cliinfo = (message_client*)listptr->data;
			
			if (cliinfo->source == source)
			{
				serverinfo->clientlist = g_list_remove(serverinfo->clientlist,cliinfo);
				g_source_remove(cliinfo->id);
				g_free(cliinfo);
				g_io_channel_unref(source);
				
				fprintf (stderr, "disconnexted\n");
			}
		}
		//return FALSE;	// When this event return FALSE, this event watch is disabled
	} else if ((condition & G_IO_IN) != 0) {	// receive data
		//dummy read
		uint64_t hoge[4];
		int fd = g_io_channel_unix_get_fd (source);
		int ret = read(fd, hoge, sizeof(hoge));
		fprintf (stderr, "cli in %d\n",ret);
		
	} else {	//	G_IO_OUT | G_IO_PRI | G_IO_NVAL
		return FALSE;	// When this event return FALSE, this event watch is disabled
	}
	
	
	return TRUE;
}

gboolean server_socket_event (	GIOChannel *source,
								GIOCondition condition,
								gpointer data)
{
	
	//fprintf (stderr, "server_socket_event in \n");
	int serverfd = -1;
	int datafd = -1;

	fprintf (stderr, "cli %x\n",(int)condition);

	serverfd = g_io_channel_unix_get_fd (source);
	
	if ((condition & (G_IO_ERR | G_IO_HUP)) != 0) {	
		//Other side socket was closed. The lisning socket will not active this event.  Fail safe.
		return FALSE;	// When this event return FALSE, this event watch is disabled
	} else if ((condition & G_IO_IN) != 0) {	// in comming connect
		datafd = accept4(serverfd, NULL, NULL,SOCK_NONBLOCK|SOCK_CLOEXEC);
		if (datafd == -1) {
			perror("accept");
			return FALSE;
        }
		
		GIOChannel *newcliio = NULL;
		guint evcliid = -1;
		message_clients *serverinfo = (message_clients*)data;
		message_client *cliinfo = NULL;
		
		newcliio = g_io_channel_unix_new (datafd);
		g_io_channel_set_close_on_unref(newcliio,TRUE);	// fd close on final unref
		
		evcliid = g_io_add_watch_full (	newcliio,
									G_PRIORITY_DEFAULT ,	// IO handling priority
									(G_IO_IN | G_IO_PRI | G_IO_ERR | G_IO_HUP | G_IO_NVAL),	// Wait input event
									clientchannel_socket_event,
									data,	// additional data
									NULL);	// Not set destroy notify function
		
		cliinfo = g_malloc(sizeof(message_client));
		cliinfo->source = newcliio;
		cliinfo->id = evcliid;
		
		serverinfo->clientlist = g_list_append( serverinfo->clientlist, cliinfo);
		
		fprintf (stderr, "connexted\n");
	} else {	//	G_IO_OUT | G_IO_PRI | G_IO_NVAL
		return FALSE;	// When this event return FALSE, this event watch is disabled
	}
	
	
	return TRUE;
}

gint64 g_time = 0;
int g_count = 0;

gboolean timerfd_event (	GIOChannel *source,
								GIOCondition condition,
								gpointer data)
{
	int timerfd = -1;
	uint64_t hoge[4];
	
	if (g_time != 0)
	{
		gint64 now = g_get_monotonic_time();
		gint64 estimate = g_time + (100 * 1000);
		g_time = g_get_monotonic_time();
		
		if ((now-estimate) > 500)
			fprintf (stderr, "now = %ld  estimate = %ld  diff=%ld\n", now, estimate, (now-estimate));
	} else {
		g_time = g_get_monotonic_time();
	}

	timerfd = g_io_channel_unix_get_fd (source);
	read(timerfd, hoge, sizeof(hoge));
	
	{
		AGLCLUSTER_SERVICE_PACKET packet;
		GList* listptr = NULL;
		message_clients *serverinfo = (message_clients*)data;
		message_client *cliinfo = NULL;
		
		packet.header.seqnum++;
		
		listptr = g_list_first(serverinfo->clientlist);
		
		for (int i=0; i < 1000;i++)	{	//loop limit
			if (listptr == NULL) break;
			
			cliinfo = (message_client*)listptr->data;
			
			fprintf(stderr,"send count = %ld,  total buff = %ld\n",packet.header.seqnum,sizeof(packet));
			ret = write(data_socket, &packet, sizeof(packet));
			if (ret == -1) {
				perror("write");
				break;
			}
			else
			{
				fprintf(stderr,"writed buffer = %d\n",ret);        		
			}
		}
	}
	
	return TRUE;
}

gboolean timer_event(gpointer data)
{
	if (g_time != 0)
	{
		gint64 now = g_get_monotonic_time();
		gint64 estimate = g_time + (100 * 1000);
		g_time = g_get_monotonic_time();
		
		if ((now-estimate) > 1000)
			fprintf (stderr, "now = %ld  estimate = %ld  diff=%ld\n", now, estimate, (now-estimate));
	} else {
		g_time = g_get_monotonic_time();
	}
	
	//if ((g_count % 10) == 0) sleep(1);
	//g_count++;
	
	return TRUE;
}

int main (int argc, char **argv) {
	GMainLoop *gloop = NULL;
	GIOChannel *gserverio = NULL;
	GIOChannel *timerio = NULL;
	struct sockaddr_un name;
	struct itimerspec timersetting;
	int serverfd = -1;
	int timerfd = -1;
	int ret = -1;
	guint evsrcid = -1;
	guint timerid = -1;
	
	message_clients clients;
	
	memset(&clients, 0, sizeof(clients));
	
	clients.clientlist = NULL; 
	
	//sigset_t ss;

#if 0 //TODO
	if (sigemptyset(&ss) < 0 || sigaddset(&ss, SIGTERM) < 0 ) {
		ret = -errno;
		goto finish;
	}

	// Block SIGTERM first, so that the event loop can handle it
	if (sigprocmask(SIG_BLOCK, &ss, NULL) < 0) {
		ret = -errno;
		goto finish;
	}

	// Let's make use of the default handler and "floating" reference features of sd_event_add_signal()
	ret = sd_event_add_signal(event, NULL, SIGTERM, NULL, NULL);
	if (ret < 0)
		goto finish;

	// Enable automatic service watchdog support
	ret = sd_event_set_watchdog(event, true);
	if (ret < 0)
		goto finish;	
#endif
	
	gloop = g_main_loop_new(NULL, FALSE);	//get default event loop context
	if (gloop == NULL)
		goto finish;

	unlink(SOCKET_NAME);
	
	serverfd = socket(AF_UNIX, SOCK_SEQPACKET|SOCK_CLOEXEC|SOCK_NONBLOCK, AF_UNIX);
	if (serverfd < 0) {
		ret = -errno;
		goto finish;
	}

	memset(&name, 0, sizeof(name));

	name.sun_family = AF_UNIX;
	strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);

	ret = bind(serverfd, (const struct sockaddr *) &name, sizeof(name));
	if (ret == -1) {
		goto finish;
	}

	ret = listen(serverfd, 20);
	if (ret == -1) {
		goto finish;
	}
	
	gserverio = g_io_channel_unix_new (serverfd);	// Create gio channel by fd.
	g_io_channel_set_close_on_unref(gserverio,TRUE);	// fd close on final unref
	
	evsrcid = g_io_add_watch_full (	gserverio,
									G_PRIORITY_DEFAULT ,	// IO handling priority
									(G_IO_IN | G_IO_OUT | G_IO_PRI | G_IO_ERR | G_IO_HUP | G_IO_NVAL),	// Wait all event
									server_socket_event,
									&clients,	// additional data
									NULL);	// Not set destroy notify function
	
	timerfd = timerfd_create (	CLOCK_MONOTONIC,
								(TFD_NONBLOCK | TFD_CLOEXEC));
	if (ret == -1) {
		goto finish;
	}
	
	memset(&timersetting, 0, sizeof(timersetting));
	
	timersetting.it_value.tv_sec  = 0;
	timersetting.it_value.tv_nsec = 100*1000*1000;
	
	timersetting.it_interval.tv_sec = 0;
	timersetting.it_interval.tv_nsec = 100*1000*1000;
	
	ret = timerfd_settime (	timerfd,
							0,
							&timersetting,
							NULL);
	if (ret == -1) {
		goto finish;
	}
	
	timerio = g_io_channel_unix_new (timerfd);
	g_io_channel_set_close_on_unref(timerio,TRUE);	// fd close on final unref
	
	timerid = g_io_add_watch_full (	timerio,
									G_PRIORITY_HIGH ,	// IO handling priority
									(G_IO_IN | G_IO_OUT | G_IO_PRI | G_IO_ERR | G_IO_HUP | G_IO_NVAL),	// Wait all event
									timerfd_event,
									//NULL,	// No additional data
									&clients,	// additional data
									NULL);	// Not set destroy notify function
	if (timerid == -1) {
		goto finish;
	}
	
	/*
	// poll timeout based timer
	// The poll timeout based timer cause big time error in millisecond order.  Shall be use over second order.
	timerid = g_timeout_add_full (	G_PRIORITY_DEFAULT,	// Cyclic timer event is highest priority
									100,				// 100ms interval
									timer_event,
									NULL,	// No additional data
									NULL);	// Not set destroy notify function
	*/
	
    g_main_loop_run(gloop);

finish:
	/*
	timerevent_source = sd_event_source_unref(timerevent_source);
	event_source = sd_event_source_unref(event_source);
	event = sd_event_unref(event);

	if (fd >= 0)
		(void) close(fd);

	if (ret < 0)
		fprintf(stderr, "Failure: %s\n", strerror(-ret));
	*/
	
	return ret < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

/*
	GSocket *serversoc = NULL;
	GError *gliberr = NULL;;
	GSocketAddress *serveraddr = NULL;
	gboolean glibresult = FALSE;
	
	// create new server socket
	serversoc = g_socket_new (	G_SOCKET_FAMILY_UNIX,		// UNIX Domain Socket
								G_SOCKET_TYPE_SEQPACKET,	// Seq Packet
								G_SOCKET_PROTOCOL_DEFAULT,	// Default protcol for UNIX Domain Seq Packet.
								&gliberr);
	if (serversoc == NULL) {
		fprintf (stderr, "Unable to create g_socket_new: %s\n", gliberr->message);
		g_error_free (gliberr);
		return -1;
	}
	
	// create server path
	serveraddr = g_unix_socket_address_new(SOCKET_NAME);
	if (serversoc == NULL) {
		fprintf (stderr, "Error return at g_unix_socket_address_new\n");
		g_object_unref(serversoc);
		return -1;
	}
	
	glibresult = g_socket_bind (serversoc,
								serveraddr,
								TRUE, // reuse to server adder
								&gliberr);
	if (glibresult == FALSE) {
		fprintf (stderr, "Unable to bind g_socket_bind: %s\n", gliberr->message);
		g_error_free (gliberr);
		g_object_unref(serversoc);
		return -1;
	}
	
	g_socket_set_listen_backlog (serversoc, 10);	// Set maximum connect queing for 10
	
	//Start server connection listening
	glibresult = g_socket_listen (	serversoc,
									&gliberr);
	if (glibresult == FALSE) {
		fprintf (stderr, "Unable to listen socket g_socket_listen: %s\n", gliberr->message);
		g_error_free (gliberr);
		g_object_unref(serversoc);
		return -1;
	}


#include <alloca.h>
#include <endian.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <systemd/sd-daemon.h>
#include <systemd/sd-event.h>

static int io_handler(sd_event_source *es, int fd, uint32_t revents, void *userdata) {
	int datafd = -1;
	
	if ((revents & EPOLLIN) != 0) {
		datafd = accept4(fd, NULL, NULL,SOCK_NONBLOCK|SOCK_CLOEXEC);
		if (datafd == -1) {
			perror("accept");
			return -1;
        }
	}
	if ((revents & (EPOLLHUP | EPOLLERR)) != 0) {
		return -1;
	}
	fprintf(stderr,"connect\n");

	return 0;
}

static uint64_t timerval=0;
int g_count = 0;
static int timer_handler(sd_event_source *es, uint64_t usec, void *userdata) {
	int ret = -1;
	
	if ((usec - timerval) > 10) {
		fprintf(stderr,"timer event sch=%ld  real=%ld\n", timerval,usec);
	}
	timerval = timerval + 10*1000;
	ret = sd_event_source_set_time(es, timerval);
	if (ret < 0) {
		return -1;
	}
	
	return 0;
}

int main(int argc, char *argv[]) {
	sd_event_source *event_source = NULL;
	sd_event_source *timerevent_source = NULL;
	sd_event *event = NULL;
	struct sockaddr_un name;
	int fd = -1;
	int ret = -1;
	sigset_t ss;

	ret = sd_event_default(&event);
	if (ret < 0)
		goto finish;

	if (sigemptyset(&ss) < 0 || sigaddset(&ss, SIGTERM) < 0 ) {
		ret = -errno;
		goto finish;
	}

	// Block SIGTERM first, so that the event loop can handle it
	if (sigprocmask(SIG_BLOCK, &ss, NULL) < 0) {
		ret = -errno;
		goto finish;
	}

	// Let's make use of the default handler and "floating" reference features of sd_event_add_signal()
	ret = sd_event_add_signal(event, NULL, SIGTERM, NULL, NULL);
	if (ret < 0)
		goto finish;
	
	// Enable automatic service watchdog support
	ret = sd_event_set_watchdog(event, true);
	if (ret < 0)
		goto finish;

	fd = socket(AF_UNIX, SOCK_SEQPACKET|SOCK_CLOEXEC|SOCK_NONBLOCK, AF_UNIX);
	if (fd < 0) {
		ret = -errno;
		goto finish;
	}

	memset(&name, 0, sizeof(name));

	name.sun_family = AF_UNIX;
	strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);


	ret = bind(fd, (const struct sockaddr *) &name, sizeof(name));
	if (ret == -1) {
		goto finish;
	}

	ret = listen(fd, 20);
	if (ret == -1) {
		goto finish;
	}
	
	ret = sd_event_add_io(event, &event_source, fd, EPOLLIN, io_handler, NULL);
	if (ret < 0)
		goto finish;

	ret = sd_event_now(event, CLOCK_MONOTONIC, &timerval);
	timerval = timerval + 1*1000*1000;
	ret = sd_event_add_time(event,
							&timerevent_source,
							CLOCK_MONOTONIC,
							timerval, //triger time (usec)
							1*1000,	//accuracy (1000usec)
 							timer_handler,
							NULL);
	if (ret < 0)
		goto finish;

	ret = sd_event_source_set_enabled(timerevent_source,SD_EVENT_ON);
	if (ret < 0)
		goto finish;
	
	(void)sd_notifyf(false,
					"READY=1\n"
					"STATUS=Daemon startup completed, processing events.");

	ret = sd_event_loop(event);

finish:
	timerevent_source = sd_event_source_unref(timerevent_source);
	event_source = sd_event_source_unref(event_source);
	event = sd_event_unref(event);

	if (fd >= 0)
		(void) close(fd);

	if (ret < 0)
		fprintf(stderr, "Failure: %s\n", strerror(-ret));

	return ret < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
*/
/*
int main(int argc, char *argv[])
{
	struct sockaddr_un name;
	int down_flag = 0;
	int ret;
	int connection_socket;
	int data_socket;
	int result;
	//char buffer[BUFFER_SIZE];
	AGLCLUSTER_SERVICE_PACKET packet;
	
	int count = 0;
	int sss;
	int bufsize;

	memset(&packet, 0, sizeof(packet));

	
	connection_socket = socket(AF_UNIX, SOCK_SEQPACKET, AF_UNIX);
	if (connection_socket == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	memset(&name, 0, sizeof(name));

	name.sun_family = AF_UNIX;
	strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);


	ret = bind(connection_socket, (const struct sockaddr *) &name, sizeof(name));
	if (ret == -1) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	ret = listen(connection_socket, 20);
	if (ret == -1) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	for (;;) {
        data_socket = accept4(connection_socket, NULL, NULL,SOCK_NONBLOCK|SOCK_CLOEXEC);
        if (data_socket == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
		
		bufsize = 0;
		sss = sizeof(bufsize);
		ret = getsockopt(data_socket, SOL_SOCKET, SO_SNDBUF, &bufsize, &sss);
		if (ret == 0)
		{
			fprintf(stderr,"getsockopt %d \n",bufsize);
		} else {
			fprintf(stderr,"setsockopt error \n");
		}
		
		bufsize = BUFFER_SIZE * 4;
		fprintf(stderr,"setsockopt %d \n",bufsize);
		ret = setsockopt(data_socket, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize));
		if (ret != 0)
		{
			fprintf(stderr,"setsockopt error \n");
		}
		
		bufsize = 0;
		sss = sizeof(bufsize);
		ret = getsockopt(data_socket, SOL_SOCKET, SO_SNDBUF, &bufsize, &sss);
		if (ret == 0)
		{
			fprintf(stderr,"getsockopt %d \n",bufsize);
		} else {
			fprintf(stderr,"setsockopt error \n");
		}
		
		
        result = 0;
        for (;;) {
			packet.header.seqnum++;
        	fprintf(stderr,"send count = %ld,  total buff = %ld\n",packet.header.seqnum,sizeof(packet));
        	ret = write(data_socket, &packet, sizeof(packet));
	        if (ret == -1) {
	            perror("write");
	            //exit(EXIT_FAILURE);
	        	break;
	        }
        	else
        	{
	        	fprintf(stderr,"writed buffer = %d\n",ret);        		
        	}
	    }
	}

	sleep(10);
	
	
    close(connection_socket);

    unlink(SOCKET_NAME);

	return 0;
}
*/
