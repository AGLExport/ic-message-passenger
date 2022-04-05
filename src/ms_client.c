#include <stdio.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_NAME "/tmp/9Lq7BNBnBycd6nxy.socket"
#define BUFFER_SIZE (1024*64)

int main(int argc, char *argv[]) {
    struct sockaddr_un addr;
    int ret,req;
    int data_socket;
    char buffer[BUFFER_SIZE*2];


    data_socket = socket(AF_UNIX, SOCK_SEQPACKET, AF_UNIX);
    if (data_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path) - 1);


    ret = connect(data_socket, (const struct sockaddr *) &addr,
                   sizeof(addr));
    if (ret == -1) {
        fprintf(stderr, "The server is down.\n");
        exit(EXIT_FAILURE);
    }


	while(1) {;
	req = BUFFER_SIZE;
	ret = read(data_socket, buffer, req);
	fprintf(stderr,"rcv size = %d (req: %d) topdata=%x\n", ret, req, buffer[0]);
	
	req = BUFFER_SIZE/2;
	ret = read(data_socket, buffer, req);
	fprintf(stderr,"rcv size = %d (req: %d) topdata=%x\n", ret, req, buffer[0]);

	req = BUFFER_SIZE/2;
	ret = read(data_socket, buffer, req);
	fprintf(stderr,"rcv size = %d (req: %d) topdata=%x\n", ret, req, buffer[0]);
	
	req = BUFFER_SIZE*2;
	ret = read(data_socket, buffer, req);
	fprintf(stderr,"rcv size = %d (req: %d) topdata=%x\n", ret, req, buffer[0]);
	
	req = BUFFER_SIZE/8;
	ret = read(data_socket, buffer, req);
	fprintf(stderr,"rcv size = %d (req: %d) topdata=%x\n", ret, req, buffer[0]);

	req = BUFFER_SIZE/8;
	ret = read(data_socket, buffer, req);
	fprintf(stderr,"rcv size = %d (req: %d) topdata=%x\n", ret, req, buffer[0]);

	req = BUFFER_SIZE/8;
	ret = read(data_socket, buffer, req);
	fprintf(stderr,"rcv size = %d (req: %d) topdata=%x\n", ret, req, buffer[0]);

	req = BUFFER_SIZE/8;
	ret = read(data_socket, buffer, req);
	fprintf(stderr,"rcv size = %d (req: %d) topdata=%x\n", ret, req, buffer[0]);

	req = BUFFER_SIZE/8;
	ret = read(data_socket, buffer, req);
	fprintf(stderr,"rcv size = %d (req: %d) topdata=%x\n", ret, req, buffer[0]);

	req = BUFFER_SIZE/8;
	ret = read(data_socket, buffer, req);
	fprintf(stderr,"rcv size = %d (req: %d) topdata=%x\n", ret, req, buffer[0]);

	req = BUFFER_SIZE/8;
	ret = read(data_socket, buffer, req);
	fprintf(stderr,"rcv size = %d (req: %d) topdata=%x\n", ret, req, buffer[0]);

	req = BUFFER_SIZE/8;
	ret = read(data_socket, buffer, req);
	fprintf(stderr,"rcv size = %d (req: %d) topdata=%x\n", ret, req, buffer[0]);
	}
	
    close(data_socket);


	return 0;
}
	
