/*----------function for course info----------*/

struct course{
	char code[10];
	char credit;
	char professor[50];
	char day[10];
	char name[50];
};

// read and store the course info
void read_cour(course* cour_info){
	char *filename = "ee.txt";
	FILE *f = fopen(filename, "r");

	char buff[256];
	struct course *temp;
	int i,j;

	temp = cour_info;
	if (f == NULL){
		printf("failed to read file ee.txt\n");
		return;
	}

	while (fgets(buff, 256, f)){
		for (i=2;i<5;i++)
			(*temp).code[i-2] = buff[i];
		(*temp).credit = buff[6];
		i=8;
		while (buff[i] != ','){
			(*temp).professor[i-8]=buff[i];
			i++;
		}
		j=++i;
		while (buff[i] != ','){
			(*temp).day[i-j] = buff[i];
			i++;
		}
		j=++i;
		while (buff[i+1]  != '\n'){
			(*temp).name[i-j] = buff[i];
			i++;
		}
		temp++;
	}
	(*temp).code[0] = '0';
	(*temp).credit = '0';
	(*temp).professor[0] = '0';
	(*temp).day[0] = '0';
	(*temp).name[0] = '0';
	
	fclose(f);
	return;
}

// extract the request info
void courinfo(char* recv, course* course, char* result){
	int i=0;
  
	memset(result,0,60);
	memcpy(result, recv, 3);
	
	while (course[i].code[0] != '0'){
		if (strcmp(result, course[i].code) == 0){
			break;
		}
		i++;
	}
	
	memset(result,0,60);
	switch(recv[3]){
		case 'c':result[0]=course[i].credit; break;
		case 'p':memcpy(result, course[i].professor,50); break;
		case 'd':memcpy(result, course[i].day,50); break;
		case 'n':memcpy(result, course[i].name,50); break;
		default :memcpy(result, "not found",50); break;
	}

	return;		// if not found, return the one with all '0'
}

// normalize the category
void category(char* recv, char* cate){

	switch(recv[3]){
		case 'c':memcpy(cate,"Credits",7);break;
		case 'p':memcpy(cate,"Professor",9);break;
		case 'd':memcpy(cate,"Days",4);break;
		case 'n':memcpy(cate,"Course Name",11);break;
		default :memcpy(cate,"not found",9);break;
	}
	return;
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