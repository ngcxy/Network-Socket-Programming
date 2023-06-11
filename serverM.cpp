
/* Socket for ServerM*/

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
# include "serverM.h"

# define TCP_PORT "25713"	// the last three digits of USC ID: 713
# define UDP_PORT "24713"
# define MAXLINE 256
# define BACKLOG 10		//pending connection queue



int main(int argc, char** argv){
	int fd_M_TCP, fd_accept, fd_M_UDP, fd_C, fd_EE, fd_CS, fd_EEorCS;		// fd: file descriptor
	struct addrinfo addr_tcp, addr_udp, addr_C, addr_EE, addr_CS;
	struct addrinfo *res_tcp, *res_udp, *res_C, *res_EE, *res_CS, *res_EEorCS;
	struct sockaddr_storage addr_clit;
	socklen_t addrsize_clit;
	int n;
	int yes=1;

	struct authentication clientinfo;
	char encryptinfo[100];
	char recvbuf[MAXLINE], sendbuf[MAXLINE];
	int chance=2;

	char code[10], cate[20];

/*-------------initialize and get addrinfo--------------*/

	memset(&addr_tcp, 0, sizeof addr_tcp);
	addr_tcp.ai_family = AF_UNSPEC; // use IPv4 or IPv6, whichever
	addr_tcp.ai_socktype = SOCK_STREAM;
	addr_tcp.ai_flags = AI_PASSIVE;
	if (getaddrinfo(NULL, TCP_PORT, &addr_tcp, &res_tcp) == -1){
		printf("(TCP)get addrinfo error: %s (error: %d)\n", strerror(errno),errno);
		exit(0);
	}


	memset(&addr_udp, 0, sizeof addr_udp);
	addr_udp.ai_family = AF_INET; // use IPv4
	addr_udp.ai_socktype = SOCK_DGRAM;
	addr_udp.ai_flags = AI_PASSIVE;
	if (getaddrinfo(NULL, UDP_PORT, &addr_udp, &res_udp) == -1){
			printf("(UDP)get addrinfo error: %s (error: %d)\n", strerror(errno),errno);
			exit(0);
	}


	memset(&addr_C, 0, sizeof addr_C);
	addr_C.ai_family = AF_INET;
	addr_C.ai_socktype = SOCK_DGRAM;
	if (getaddrinfo("127.0.0.1", "21713", &addr_C, &res_C) == -1){
			printf("(to serverC)get addrinfo error: %s (error: %d)\n", strerror(errno),errno);
			exit(0);
	}

	memset(&addr_EE, 0, sizeof addr_EE);
	addr_EE.ai_family = AF_INET;
	addr_EE.ai_socktype = SOCK_DGRAM;
	if (getaddrinfo("127.0.0.1", "23713", &addr_EE, &res_EE) == -1){
			printf("(to serverEE)get addrinfo error: %s (error: %d)\n", strerror(errno),errno);
			exit(0);
	}

	memset(&addr_CS, 0, sizeof addr_CS);
	addr_CS.ai_family = AF_INET;
	addr_CS.ai_socktype = SOCK_DGRAM;
	if (getaddrinfo("127.0.0.1", "22713", &addr_CS, &res_CS) == -1){
			printf("(to serverCS)get addrinfo error: %s (error: %d)\n", strerror(errno),errno);
			exit(0);
	}

/*-------------create socket and bind (for both tcp and udp)--------------*/	

	if((fd_M_TCP=l_socket_bind(res_tcp)) == -1)
		printf("(TCP)serverM: failed to bind\n");

	freeaddrinfo(res_tcp);

	if((fd_M_UDP=l_socket_bind(res_udp)) == -1)
		printf("(UDP)serverM: failed to bind\n");	

/*-------------------TCP socket start listening for client------------------*/	

	if( listen(fd_M_TCP, BACKLOG) == -1){
		printf("listen socket error: %s (error: %d)\n", strerror(errno), errno);
		exit(0);
	}

	printf("The main server is up and running.\n");

	// go to the loop of serving
	while(1){

		// start infinite loop
		addrsize_clit = sizeof addr_clit;
		if ((fd_accept = accept(fd_M_TCP, (struct sockaddr*)&addr_clit, &addrsize_clit)) == -1){
			printf("accept socket error: %s (error: %d)\n", strerror(errno), errno);
			continue;
		}

		// client arrives
		memset(recvbuf, ' ', MAXLINE);
		if ((n = recv(fd_accept, recvbuf, MAXLINE, 0)) == -1){
			printf("receive msg error: %s (error: %d)\n", strerror(errno), errno);
			continue;
		}

/*-----------------------------------child socket-----------------------------------*/

		if (!fork()){		//returns 0 to the child process and process ID of the child process to the parent process. 
			close(fd_M_TCP);
			recvbuf[n-1]='\0';
			clientinfo = deinfo(recvbuf);
			printf("The main server received the authentication for %s using TCP over port %s.\n", clientinfo.username, TCP_PORT);

			// encrypt and send to serverC
			memcpy(encryptinfo, recvbuf, 100);
			encrypt(encryptinfo);
			memcpy(sendbuf, encryptinfo, 100);	
			
			/*----------------------UDP to serverC---------------------*/

			if((fd_M_UDP=socket(res_udp->ai_family, res_udp->ai_socktype, res_udp->ai_protocol)) == -1)
				printf("(UDP)create socket error: %s (error: %d)", strerror(errno), errno);
			
			if (sendto(fd_M_UDP, sendbuf, MAXLINE, 0, res_C->ai_addr, res_C->ai_addrlen) == -1)
				printf("(UDP)send msg error: %s(errno: %d)\n", strerror(errno), errno);
			printf("The main server sent an authentication request to serverC.\n");


			memset(recvbuf, 0, MAXLINE);
			if ((n=recvfrom(fd_M_UDP, recvbuf, MAXLINE-1, 0, res_C->ai_addr,&(res_C->ai_addrlen))) == -1)
				printf("(UDP)recv msg error: %s(errno: %d)\n", strerror(errno), errno);
			printf("The main server received the result of the authentication request \
from serverC using UDP over port %s.\n", UDP_PORT);
			
			/*-----------------authentication feedback--------------------*/
			memset(sendbuf, 0, MAXLINE);
			memcpy(sendbuf, recvbuf, MAXLINE);
			if (send(fd_accept, sendbuf, 1, 0) == -1)
				printf("(TCP)send msg error: %s(errno: %d)\n", strerror(errno), errno);
			printf("The main server sent the authentication result to the client.\n");			

/*--------------------------------if not pass at the first time-----------------------------------*/
			while ((recvbuf[0] != '2') && chance){
				chance--;
				
				/*--------------------receive from client-------------------------*/
				memset(recvbuf, 0, MAXLINE);
				if ((n = recv(fd_accept, recvbuf, MAXLINE, 0)) == -1)
					printf("(TCP)receive msg error: %s (error: %d)\n", strerror(errno), errno);
				
				recvbuf[n-1]='\0';
				clientinfo = deinfo(recvbuf);
				printf("The main server received the authentication for %s using TCP over port %s.\n", clientinfo.username, TCP_PORT);
				
				/*------------------encrypt and process the info----------------------*/
				memcpy(encryptinfo, recvbuf, 100);
				encrypt(encryptinfo);	

				memset(sendbuf, 0, MAXLINE);
				memcpy(sendbuf, encryptinfo, 100);

				if (sendto(fd_M_UDP, sendbuf, MAXLINE, 0, res_C->ai_addr, res_C->ai_addrlen) == -1)
					printf("(UDP)send msg error: %s(errno: %d)\n", strerror(errno), errno);
				printf("The main server sent an authentication request to serverC.\n");


				memset(recvbuf, 0, MAXLINE);
				if ((n=recvfrom(fd_M_UDP, recvbuf, MAXLINE-1, 0, res_C->ai_addr,&(res_C->ai_addrlen))) == -1)
					printf("(UDP)recv msg error: %s(errno: %d)\n", strerror(errno), errno);
				printf("The main server received the result of the authentication request \
from serverC using UDP over port %s.\n", UDP_PORT);

				/*--------------------send the result to client-------------------------*/
				memset(sendbuf, 0, MAXLINE);
				memcpy(sendbuf, recvbuf, MAXLINE);
				if (send(fd_accept, sendbuf, 1, 0) == -1)
					printf("(TCP)send msg error: %s(errno: %d)\n", strerror(errno), errno);
				printf("The main server sent the authentication result to the client.\n");

			}

/*--------------------------recv & standardize the course request-------------------------------*/

			if((fd_M_UDP=socket(res_udp->ai_family, res_udp->ai_socktype, res_udp->ai_protocol)) == -1)
				printf("(UDP)create socket error: %s (error: %d)", strerror(errno), errno);


			while(1){
				memset(recvbuf, 0, MAXLINE);
				if ((n = recv(fd_accept, recvbuf, MAXLINE, 0)) == -1)
					printf("(TCP)receive msg error: %s (error: %d)\n", strerror(errno), errno);

				else{
					memcpy(code, recvbuf, 5);
					category(recvbuf, cate);
					printf("The main server received from %s to query course %s about %s using TCP over port %s.\n", clientinfo.username, code, cate, TCP_PORT);
				}

				/*---------------------------UDP to serverEE/CS-----------------------------*/
			
			if (code[0] == 'E')	{fd_EEorCS = fd_EE;res_EEorCS = res_EE;}
			if (code[0] == 'C')	{fd_EEorCS = fd_CS;res_EEorCS = res_CS;}

			memcpy(sendbuf, recvbuf+2, MAXLINE);

			if (sendto(fd_M_UDP, sendbuf, MAXLINE, 0, res_EEorCS->ai_addr, res_EEorCS->ai_addrlen) == -1)
				printf("(UDP)send msg error: %s(errno: %d)\n", strerror(errno), errno);
			printf("The main server sent a request to server%c%c.\n", code[0], code[1]);

			memset(recvbuf, 0, MAXLINE);
			if ((n=recvfrom(fd_M_UDP, recvbuf, MAXLINE-1, 0, res_EEorCS->ai_addr,&(res_EEorCS->ai_addrlen))) == -1)
				printf("(UDP)recv msg error: %s(errno: %d)\n", strerror(errno), errno);
			printf("The main server received from the server%c%c using UDP over port %s.\n",code[0], code[1], UDP_PORT);		

			memset(sendbuf, 0, MAXLINE);
			memcpy(sendbuf, recvbuf, MAXLINE);
			if (send(fd_accept, sendbuf, MAXLINE-1, 0) == -1)
				printf("(TCP)send msg error: %s(errno: %d)\n", strerror(errno), errno);
			printf("The main server sent the query infomation to the client.\n");
			}


			close(fd_M_UDP);
			exit(0);
		
		}// end fork()

		close(fd_accept);

	}

	close(fd_M_TCP);

}
