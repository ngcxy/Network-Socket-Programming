/*----------function for credit----------*/

struct authentication{
	char username[51];
	char password[51];
};

//extract username and password from the string
struct authentication deinfo(char* info){
	struct authentication au;
	int i;

	memset(au.username, '\0', 51);
	memset(au.password, '\0', 51);

	for(i=0;info[i]!='-';i++)
		memcpy(au.username+i,info+i, sizeof(char));
	
	for(i=50;info[i]!='-';i++)
		memcpy(au.password+i-50,info+i, sizeof(char));

	return au;
}

// read and store the credit data
void read_cred(authentication* data_info){
	char *filename = "cred.txt";
	FILE *f = fopen(filename, "r");

	char buff[256];
	struct authentication *temp;
	int i,j;

	temp = data_info;
	if (f == NULL){
		printf("failed to read file cred.txt\n");
		return;
	}

	while (fgets(buff, 256, f)){	
		i=j=0;
		(*temp).username[0] = '0';
		while (buff[i] != ','){
			(*temp).username[i] = buff[i];
			i++;
		}
		j=i+1;
		while (buff[j+1] != '\n'){
			//(*temp).password[0] = '0';
			(*temp).password[j-i-1] = buff[j];
			j++;
		}
		temp++;
	}

	(*temp).username[0] = '0';
	(*temp).password[0] = '0';

	fclose(f);
	
	return;
}


// compare the credit data
int data_comp(struct authentication* data, struct authentication client){
	int i=0,j=0;
	int com, flag=0;
	while (data[i].username[0] != '0'){
		if(com = strcmp(data[i].username, client.username) == 0){
			flag = 1;
			break;
		}
		i++;
	}

	if (flag == 0)
		return 0;

	if(com = strcmp(data[i].password, client.password) == 0){
		return 2;
	}

	return 1;
}



/*----------function for socket----------*/

// loop to bind the first socket we can
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
	if (p == NULL)
		return -1;
	
	return fd;
}
