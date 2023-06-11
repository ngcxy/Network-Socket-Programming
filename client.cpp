
 /* Socket for the client */

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
# include "client.h"

# define CONNET_PORT "25713"
# define MAXLINE 256



int main(int argc, char** argv){
	int fd_client, n;
	struct addrinfo clitaddr, *conninfo, *p;		//conninfo: client & server's connect info
	char buf[MAXLINE];
	int getaddrvalue;
	int portnum;
	socklen_t len=sizeof(clitaddr);


	char username[51], password[51], IDinfo[100];
	char sendbuf[MAXLINE], recvbuf[MAXLINE];
	int chance=2, wrong=1;

	char code[10], cate[20], req[30];

	/*----------initiate the client & server address info----------*/

	memset(&clitaddr, 0, sizeof clitaddr);
	clitaddr.ai_family = AF_UNSPEC;		// means whatever
	clitaddr.ai_socktype = SOCK_STREAM;

	if ((getaddrvalue = getaddrinfo("127.0.0.1", CONNET_PORT, &clitaddr, &conninfo)) == -1) {
		// argv[1] : input the ip address to connect to
		printf ("get addrinfo error: %s (error: %d)", strerror(errno), errno);
		exit(0);
	}

	/*----------loop to use the first socket to connect----------*/

	for (p=conninfo; p!=NULL; p = p->ai_next) {
		if ((fd_client = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			printf("create socket error: %s (error: %d)", strerror(errno), errno);
			exit(0);
		}


		if (connect(fd_client, p->ai_addr, p->ai_addrlen) == -1){
			printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
			exit(0);
		}

		break;

	}

	if (p == NULL) {
		printf("client: fail to connect. Error: %s(errno: %d)\n",strerror(errno),errno);
		return 2;
	}
	
	getsockname(portnum,(struct sockaddr*)&clitaddr, &len);

	printf("The client is up and running.\n");

	/*-------------input & send the authentication info--------------*/

	memset(IDinfo, '-', 100);
    
    while (wrong){

	    memset(username,0,51);
	    printf("Please enter the username: ");
	    scanf("%s", username);
	    
	    memset(password,0,51);
	    printf("please enter the password: ");
	    scanf("%s", password);

	    info(IDinfo,username,password);
	    memcpy(sendbuf, IDinfo, sizeof(IDinfo));

		if (send(fd_client, IDinfo, sizeof(IDinfo), 0) == -1)
			printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);

		/*-------------receive authentication result-------------*/
		memset(recvbuf, '-', MAXLINE);
		if ((n = recv(fd_client, recvbuf, MAXLINE-1, 0)) == -1)
			printf("recv error");

		chance = feedback(recvbuf[0], chance, username, portnum);
		if (chance==-2)	wrong=0; 
		if (chance==-1) {wrong=0; close(fd_client);exit(0);}
		
	}

/*-----------------------input & send & receive course info--------------------------*/
	while (1){
		printf("Please enter the course code to query: ");
		scanf("%s", code);

		printf("Please enter the category(Credit / Professor / Days / CourseName): ");
		scanf("%s", cate);

		query(code, cate, req);

		if (send(fd_client, req, sizeof(req), 0) == -1)
			printf("send msg error: %s(errno: %d)\n", strerror(errno), errno);
		else
			printf("%s sent a request to the main server.\n", username);

		memset(recvbuf, 0, MAXLINE);
		if ((n = recv(fd_client, recvbuf, MAXLINE-1, 0)) == -1)
			printf("recv error\n");
		else
			printf("The client received the response from thr Main Server using TCP over port %d.\n", ntohs(portnum));

		if (recvbuf[0]!='0')
			printf("The %s of %s is %s.\n\n", cate, code, recvbuf);
		else
			printf("Didn't find the course: %s.\n\n", code);
		
		printf("---Start a new request----\n");

	}

	close(fd_client);
	exit(0);
}