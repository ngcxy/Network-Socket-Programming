
a. Full Name:	Xinyi Cai

b. Student ID:	2657935713

c. Code files:	serverM.cpp：	
								TCP socket on static port connecting to the client.
								UDP socket on static port connecting to the serverC, serverEE and serverCS.
								Exchanges messages of authentication and course between client and backend servers.
				serverM.h :		
								Struct: 	authentication - username and password of the client
								Function: 	deinfo() - turn a single string of ID infomation into struct authentication
											encrypt() - encrypt the username and password with the given method
											category()- turn a single char corresponding to a category into full string
											l_socket_bind() - goes into a loop to create socket and find the first to bind
				serverC.cpp:	
								UDP socket on static port connecting to serverM to check the creditential info and feedback based on data in cred.txt
				serverC.h:		
								Struct: 	authentication - username and password of the client
								Function:	read_cred() - read the cred.txt file
											data_comp() - compare the credential info received from serverM with data in the file
											l_socket_bind() - goes into a loop to create socket and find the first to bind
				serverEE.cpp:	
								UDP socket on static port connecting to serverM to search and give the required info based on data in ee.txt
				serverEE.h:		
								Struct:		course - course code, credit, professor, day and name of the course
								Function:	read_cour() - read the ee.txt file
											courinfo() - search and return the required course info based on data in ee.txt
											category()- turn a single char corresponding to a category into full string
											l_socket_bind() - goes into a loop to create socket and find the first to bind
				serverCS.cpp:
								UDP socket on static port connecting to serverM to search and give the required info based on data in cs.txt				
				serverCS.h:
								Struct:		course - course code, credit, professor, day and name of the course
								Function:	read_cour() - read the cs.txt file
											courinfo() - search and return the required course info based on data in cs.txt
											category()- turn a single char corresponding to a category into full string
											l_socket_bind() - goes into a loop to create socket and find the first to bind				
				client.cpp:
								TCP socket on dynamic port connecting to serverM. Input credential information and query on course and output the result from backend server.
				client.h:		
								Function:	info() - combine the info of username and password into a single string
											feedback() - output the authentication result according to received message
											query() - turn the required category into one charactor for exchanging

d. Message format:	
					authentication infomation: 	
											when exchaneged by socket, username and password are combined into one string, each consumes 50 charactors
											the empty space is filled by '-'
											example: "james---------------------------------------------2kAnsa7s)-----------------------------------------"
					authentication result:		
											int 0: username does not exit, 1: password does not match, 2: all infomation is correct
					course query:	
											category is described as one charactor
											'c'-credit, 'p'-professor, 'd'-days, 'n'-coursename
											example: (between client-serverM) "EE450p" (between serverM-serverEE) "450p"

e. Idiosyncrasy:	The input username and password cannot exceed 50 charactors.
					The input course code can only begins with 'EE' or 'CS', others will fail.
					The input category of query can only be 'credit', professor', 'days' or 'coursename', others will fail.
					When compiling, there will be warnings: deprecated conversion from string constant to 'char*', which concerns about the filename type, but it won't affect the working of the whole code. Just dismiss it.

f. Reused Code: 	the function 'l_socket_bind()'' in serverM.h, serverC.h, serverEE.h, serverCS.h is learned from the block in Beej's 					Guide to Network Programming, in page 31.