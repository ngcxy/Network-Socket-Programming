/** Socket for ServerEE*/

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
# include "serverEE.h"

# define PORT_EE "23713"		// the last three digits of USC ID: 713
# define MAXLINE 256

int main(int argc, char** argv){
	struct course data[100];
	char code[10], cate[20], courres[60];

	int fd_EE;
	struct addrinfo addr_EE, *res;
	struct sockaddr_storage addr_M;
	socklen_t addrsize_M;
	char recvbuf[MAXLINE], sendbuf[MAXLINE];
	int m, n;
	int yes=1;

	// read the ee.txt file
	read_cour(data);

/*--------------------start initializing UDP socket----------------------------*/

	memset(&addr_EE, 0, sizeof addr_EE);
	addr_EE.ai_family = AF_INET; // use IPv4 or IPv6, whichever
	addr_EE.ai_socktype = SOCK_DGRAM;
	addr_EE.ai_flags = AI_PASSIVE;

	if ((m = getaddrinfo(NULL, PORT_EE, &addr_EE, &res)) == -1){
		printf("get addrinfo error: %s (error: %d)\n", strerror(errno),errno);
		exit(0);
	}	

	if ((fd_EE = l_socket_bind(res)) == -1){
		printf("serverEE: failed to bind\n");
		exit(0);
	}

	freeaddrinfo(res);

	printf("The serverEE is up and running using UDP on port %s\n", PORT_EE);

	addrsize_M = sizeof addr_M;

/*---------------------go to the loop of serving-----------------------------*/
	while(1){

		memset(recvbuf, 0, MAXLINE);

		if ((n = recvfrom(fd_EE, recvbuf, MAXLINE-1, 0, ((struct sockaddr *)&addr_M), &addrsize_M)) == -1){
			printf("receive msg error: %s (error: %d)\n", strerror(errno), errno);
			exit(1);
		}

		memset(cate,0,sizeof cate);
		memcpy(code, recvbuf, 3);
		category(recvbuf, cate);
		printf("The ServerEE received a request from the Main Server about the %s of EE%s.\n", cate, code);
		
		/*----------------find the course info---------------------*/
		courinfo(recvbuf, data, courres);
		memset(sendbuf,0,MAXLINE);
		memcpy(sendbuf, courres, sizeof courres);
		if (courres[0] == '0') 
			printf("Didn't find the course: EE%s\n", code);
		else {
			printf("The information has been found: The %s of EE%s is %s\n", cate, code, courres);
		}

		if (sendto(fd_EE, sendbuf, strlen(sendbuf), 0, ((struct sockaddr *)&addr_M), addrsize_M) == -1){
			printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
		}

		printf("The ServerEE finished sending the response to the Main Server.\n");
		
	}
	close(fd_EE);
}
