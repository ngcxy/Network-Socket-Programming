/** Socket for the Server*/

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <errno.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <sys/wait.h>
# include <signal.h>
# include "serverC.h"

# define PORT_C "21713"		// the last three digits of USC ID: 713
# define MAXLINE 256

int main(int argc, char** argv){
	
	struct authentication data[100], client;
	int comres;

	int fd_C;		// fd: file descriptor
	struct addrinfo addr_C, *res, *p;
	struct sockaddr_storage addr_M;
	socklen_t addrsize_M;
	char recvbuf[MAXLINE], sendbuf[MAXLINE];
	int m, n;
	int yes=1;

	//read the cred.txt file
	read_cred(data);

/*--------------------start initializing UDP socket----------------------------*/

	memset(&addr_C, 0, sizeof addr_C);
	addr_C.ai_family = AF_INET; // use IPv4 or IPv6, whichever
	addr_C.ai_socktype = SOCK_DGRAM;
	addr_C.ai_flags = AI_PASSIVE;

	if ((m = getaddrinfo(NULL, PORT_C, &addr_C, &res)) == -1){
		printf("get addrinfo error: %s (error: %d)\n", strerror(errno),errno);
		exit(0);
	}

	if ((fd_C = l_socket_bind(res)) == -1){
		printf("serverC: failed to bind\n");
		exit(0);
	}

	freeaddrinfo(res);

	printf("The serverC is up and running using UDP on port %s\n", PORT_C);

	addrsize_M = sizeof addr_M;

/*---------------------go to the loop of serving-----------------------------*/
	while(1){

		memset(recvbuf, 0, MAXLINE);

		if ((n = recvfrom(fd_C, recvbuf, MAXLINE-1, 0, ((struct sockaddr *)&addr_M), &addrsize_M)) == -1){
			printf("receive msg error: %s (error: %d)\n", strerror(errno), errno);
			exit(1);
		}

		recvbuf[n] = '\0';
		printf("The ServerC received an authentication request from the Main Server.\n");
		
		/*----------------process & compare the data---------------------*/
		client = deinfo(recvbuf);
		comres = data_comp(data, client);
		memset(sendbuf, 0, MAXLINE);
		memset(sendbuf, comres+48, 1);

		if (sendto(fd_C, sendbuf, MAXLINE, 0, ((struct sockaddr *)&addr_M), addrsize_M) == -1){
			printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
		}

		printf("The ServerC finished sending the response to the Main Server.\n");
		
	}
	close(fd_C);
}