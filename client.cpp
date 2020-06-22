#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <pthread.h>
#include <string>
#include <iostream>
using namespace std;
// basic server and client code from Professor Rincon

void error(string msg) {
	perror(msg.c_str());
	exit(0);
}

class container {
	public:
	int value, sock;
	string bin, e_Gamma; // the second * hurt me for so long
};

void *transmit(void *cont_void)
{
	class container *cont = (class container *)cont_void;
	int status;
	status = write(cont->sock, &cont->value, sizeof(int));
	if (status < 0) printf("ERROR writing to socket\n");

	// overcompensate for these sizes jic
	char bin[50], e_Gamma[50]; // poopy sockets compat w c-strings only
	status = read(cont->sock, &bin, sizeof(string));
	if (status < 0) error("ERROR reading from socket");
	status = read(cont->sock, &e_Gamma, sizeof(string));
	if (status < 0) error("ERROR reading from socket");
	string s_bin(bin), s_e_Gamma(e_Gamma); 
	// now construct std strings w/ c-strings 
	cout << bin << " - " << e_Gamma << '\n';
	cont->bin= s_bin;
	cont->e_Gamma = s_e_Gamma;

  return NULL;
}


int main(int argc, char *argv[]) { // ip add, port e.g. ./client localhost 1234
	int sockfd, portno, n;

	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[256];
	/*
	if (argc < 3) {
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}
	*/

	// EACH THREAD CREATES ITS OWN SOCKET

	// portno = atoi(argv[2]); 
	portno = 6900; // local debug
	// server = gethostbyname(argv[1]); // pass server name, use DNS server
	server = gethostbyname("127.0.0.1"); // local debug
	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, // server contains info
	      (char *)&serv_addr.sin_addr.s_addr,
	      server->h_length);
	serv_addr.sin_port = htons(portno);

	int values = 7;
	// fgets(values,sizeof(int),stdin);

	
 	pthread_t tid[values];
	class container conts[values];
	
	int control[7] = {16,32,64,128,256,512,1024}; ///

	int sockfd;
	

	for (int i = 0; i < values; i++) {
		printf("Sending %d\n", control[i]);///

		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0)
			error("ERROR opening socket");
		if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
			error("ERROR connecting");

		conts[i].sock = sockfd;
		conts[i].value = control[i];
		// fgets(conts[i].value,sizeof(int),stdin);

		if(pthread_create(&tid[i], NULL, transmit,(void *)&conts[i])) 
		{
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		// officially accessible by main thread
		// printf("Value: %d, Binary Code: %s, Elias-Gamma code: %s\n", conts[i].value, conts[i].bin, conts[i].e_Gamma);
		// pinche puta madre wrong place
		// bzero(buffer,256);
		// fgets(buffer,255,stdin);

		// n = write(sockfd,buffer,strlen(buffer)); // no need for newsockfdA[i], use sockfdA[i] directly
		
		// bzero(buffer,256);
		// n = read(sockfdA[i],buffer,255);
		// printf("%s\n",buffer);

	}
	for (int i = 0; i < values; i++)
        	pthread_join(tid[i], NULL);

	for (int i = 0; i < values; i++) {
		printf("Value: %d, Binary Code: %s, Elias-Gamma code: %s\n", conts[i].value, conts[i].bin.c_str(), conts[i].e_Gamma.c_str());
		close(sockfdA[i]);
	}



	// print storage to stdout

	return 0;
}