
make all: ./serverM ./serverC ./serverEE ./serverCS ./client

./serverM: serverM.cpp serverM.h
	gcc -o serverM serverM.cpp

./serverC: serverC.cpp serverC.h
	gcc -o serverC serverC.cpp

./serverEE: serverEE.cpp serverEE.h
	gcc -o serverEE serverEE.cpp

./serverCS: serverCS.cpp serverCS.h
	gcc -o serverCS serverCS.cpp

./client: client.cpp client.h
	gcc -o client client.cpp