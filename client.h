
/*----------function for authentication----------*/

// combine username and password into a single string
char* info(char* info, char* username, char* password){
	int i;

	memset(info,'-', 100);
	for (i=0;username[i];i++)		// don't include the last '\0'
		memcpy(info+i,username+i, sizeof(char));

	for (i=50;password[i-50];i++)
		memcpy(info+i,password+i-50, sizeof(char));
	
	return info;
}

// print feedback
int feedback(char result, int chance, char* username, int portnum){;
	int i;

	if (result == '2'){
		chance = -2;
		printf("%s received the result of authentication using TCP over port %d. \
Authentication is successful\n", username, ntohs(portnum));
	 	return chance;
	}


	else{

		if (result == '1'){
			chance--;
			printf("%s received the result of authentication using TCP over port %d. \
Authentication failed: Password does not match\n\
Attempt remaining: %d\n", username, ntohs(portnum), chance+1);
		}

		if (result == '0'){
			chance--;
			printf("%s received the result of authentication using TCP over port %d. \
Authentication failed: Username Does not exist\n\
Attempt remaining: %d\n", username, ntohs(portnum), chance+1);
		}

		if (chance == -1){
			printf("Authentication Failed for 3 attempts. Client will shut down.\n");
	
		}
		return chance;
	}
}

/*----------function for course query----------*/

// standardize the query message
void query(char* code, char* cate, char* req){
	char s;
	memcpy(req, code, 5);

	switch(cate[2]){
		case 'e':s='c';break;	// Credit
		case 'o':s='p';break;	// Professor
		case 'y':s='d';break;	// Days
		case 'u':s='n';break;	// CourseName
		default :s='a';break;
	}
	req[5] = s;
	return;
}

