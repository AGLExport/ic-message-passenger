#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_NAME "/tmp/9Lq7BNBnBycd6nxy.socket"
#define BUFFER_SIZE (1024*64)

int main(int argc, char *argv[])
{
	struct sockaddr_un name;
	int down_flag = 0;
	int ret;
	int connection_socket;
	int data_socket;
	int result;
	char buffer[BUFFER_SIZE];
	int count = 0;
	int sss;
	int bufsize;

	/* Create local socket. */
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
			count++;
        	fprintf(stderr,"send count = %d,  total buff = %d\n",count,(count*BUFFER_SIZE));
        	ret = write(data_socket, buffer, sizeof(buffer));
	        if (ret == -1) {
	            perror("write");
	            exit(EXIT_FAILURE);
	        }
        	else
        	{
	        	fprintf(stderr,"writed buffer = %d\n",ret);        		
        	}
	    }
	}

    close(connection_socket);

    unlink(SOCKET_NAME);

	return 0;
}
