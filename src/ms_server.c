#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "ipc_protocol.h"


#define SOCKET_NAME "/tmp/9Lq7BNBnBycd6nxy.socket"
#define BUFFER_SIZE (1024*64)


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
        void *buffer;
        ssize_t n;
        int sz;

        /* UDP enforces a somewhat reasonable maximum datagram size of 64K, we can just allocate the buffer on the stack */
        if (ioctl(fd, FIONREAD, &sz) < 0)
                return -errno;
        buffer = alloca(sz);

        n = recv(fd, buffer, sz, 0);
        if (n < 0) {
                if (errno == EAGAIN)
                        return 0;

                return -errno;
        }

        if (n == 5 && memcmp(buffer, "EXIT\n", 5) == 0) {
                /* Request a clean exit */
                sd_event_exit(sd_event_source_get_event(es), 0);
                return 0;
        }

        fwrite(buffer, 1, n, stdout);
        fflush(stdout);
        return 0;
}

int main(int argc, char *argv[]) {
        union {
                struct sockaddr_in in;
                struct sockaddr sa;
        } sa;
	sd_event_source *event_source = NULL;
	sd_event *event = NULL;
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

	/* Block SIGTERM first, so that the event loop can handle it */
	if (sigprocmask(SIG_BLOCK, &ss, NULL) < 0) {
		ret = -errno;
		goto finish;
	}

	/* Let's make use of the default handler and "floating" reference features of sd_event_add_signal() */
	ret = sd_event_add_signal(event, NULL, SIGTERM, NULL, NULL);
	if (ret < 0)
		goto finish;
	
	/* Enable automatic service watchdog support */
	ret = sd_event_set_watchdog(event, true);
	if (ret < 0)
		goto finish;

	fd = socket(AF_UNIX, SOCK_SEQPACKET|SOCK_CLOEXEC|SOCK_NONBLOCK, AF_UNIX);
	if (fd < 0) {
		ret = -errno;
		goto finish;
	}

        sa.in = (struct sockaddr_in) {
                .sin_family = AF_INET,
                .sin_port = htobe16(7777),
        };
        if (bind(fd, &sa.sa, sizeof(sa)) < 0) {
                r = -errno;
                goto finish;
        }

        r = sd_event_add_io(event, &event_source, fd, EPOLLIN, io_handler, NULL);
        if (r < 0)
                goto finish;

        (void) sd_notifyf(false,
                          "READY=1\n"
                          "STATUS=Daemon startup completed, processing events.");

        r = sd_event_loop(event);

finish:
        event_source = sd_event_source_unref(event_source);
        event = sd_event_unref(event);

        if (fd >= 0)
                (void) close(fd);

        if (r < 0)
                fprintf(stderr, "Failure: %s\n", strerror(-r));

        return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}


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
