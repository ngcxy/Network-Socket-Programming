
/*----------function for authentication----------*/

//extract username and password from the string
struct authentication{
	char username[51];
	char password[51];
};

struct authentication deinfo(char* info){
	struct authentication au;
	int i;

	memset(au.username, 0, 51);
	memset(au.password, 0, 51);

	for(i=0;info[i]!='-';i++)
		memcpy(au.username+i,info+i, sizeof(char));
	
	for(i=50;info[i]!='-';i++)
		memcpy(au.password+i-50,info+i, sizeof(char));

	return au;
}

// encrypt the authentication info
void encrypt(char* pw){
	int i=0;

	while(*pw){
		if (*pw>47 && *pw<58) *pw = *pw+4-*pw/54*10;
		if (*pw>64 && *pw<91) *pw = *pw+4-*pw/87*26;
        if (*pw>96 && *pw<123) *pw = *pw+4-*pw/119*26;
       
        *pw++; i++;
	}
    pw = pw-i;
}

/*----------function for course info----------*/

// normalize the category
void category(char* recv, char* cate){

	switch(recv[5]){
		case 'c':memcpy(cate,"Credits",8);break;
		case 'p':memcpy(cate,"Professor",10);break;
		case 'd':memcpy(cate,"Days",5);break;
		case 'n':memcpy(cate,"Course Name",12);break;
		default :memcpy(cate,"not found",10);break;
	}
	return;
}

/*----------function for socket----------*/

// loop to bind the first socket we can
// learned from Beej's Network  Instruction
int l_socket_bind(struct addrinfo *servinfo){
	struct addrinfo *p;
	int fd;		//file descriptor
	int yes=1;
	for (p=servinfo; p!=NULL; p = p->ai_next) {
		if ((fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			printf("create socket error: %s (error: %d)\n", strerror(errno),errno);
			exit(0);
		}

		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
			printf("set socketopt error: %s (error: %d)\n", strerror(errno),errno);
			exit(1);
		}

		if (bind(fd, p->ai_addr, p->ai_addrlen) == -1) {
			printf("bind socket error: %s (error: %d)\n", strerror(errno),errno);
			exit(0);
		}

		break;
	}

	if (p == NULL){
		return -1;
	}

	return fd;

}

