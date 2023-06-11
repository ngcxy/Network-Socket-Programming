/** Socket for ServerCS*/

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
# include "serverCS.h"

# define PORT_CS "22713"		// the last three digits of USC ID: 713
# define MAXLINE 256

int main(int argc, char** argv){
	struct course data[100];
	char code[10], cate[20], courres[60];

	int fd_CS;
	struct addrinfo addr_CS, *res;
	struct sockaddr_storage addr_M;
	socklen_t addrsize_M;
	char recvbuf[MAXLINE], sendbuf[MAXLINE];
	int m, n;
	int yes=1;

	// read the ee.txt file
	read_cour(data);

/*--------------------start initializing UDP socket----------------------------*/

	memset(&addr_CS, 0, sizeof addr_CS);
	addr_CS.ai_family = AF_INET; // use IPv4 or IPv6, whichever
	addr_CS.ai_socktype = SOCK_DGRAM;
	addr_CS.ai_flags = AI_PASSIVE;

	if ((m = getaddrinfo(NULL, PORT_CS, &addr_CS, &res)) == -1){
		printf("get addrinfo error: %s (error: %d)\n", strerror(errno),errno);
		exit(0);
	}	

	if ((fd_CS = l_socket_bind(res)) == -1){
		printf("serverCS: failed to bind\n");
		exit(0);
	}

	freeaddrinfo(res);

	printf("The serverCS is up and running using UDP on port %s\n", PORT_CS);

	addrsize_M = sizeof addr_M;

/*---------------------go to the loop of serving-----------------------------*/
	while(1){

		memset(recvbuf, 0, MAXLINE);

		if ((n = recvfrom(fd_CS, recvbuf, MAXLINE-1, 0, ((struct sockaddr *)&addr_M), &addrsize_M)) == -1){
			printf("receive msg error: %s (error: %d)\n", strerror(errno), errno);
			exit(1);
		}

		memset(cate,0,sizeof cate);
		memcpy(code, recvbuf, 3);
		category(recvbuf, cate);
		printf("The ServerCS received a request from the Main Server about the %s of CS%s.\n", cate, code);
		
		/*----------------find the course info---------------------*/
		courinfo(recvbuf, data, courres);
		memset(sendbuf,0,MAXLINE);
		memcpy(sendbuf, courres, sizeof courres);
		if (courres[0] == '0') 
			printf("Didn't find the course: CS%s\n", code);
		else {
			printf("The information has been found: The %s of CS%s is %s\n", cate, code, courres);
		}

		if (sendto(fd_CS, sendbuf, strlen(sendbuf), 0, ((struct sockaddr *)&addr_M), addrsize_M) == -1){
			printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
		}

		printf("The ServerCS finished sending the response to the Main Server.\n");
		
	}
	close(fd_CS);
}
