#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <math.h>
#include <iomanip>
#include <iostream>
using namespace std;
// basic server and client code from Professor Rincon

void error(string msg) {
	perror(msg.c_str());
	exit(1);
}


class container {
	public:
	int value, sock;
	string bin, e_Gamma;
};

void fireman(int x)
{
   while (waitpid(-1, NULL, WNOHANG) > 0)
	 {}
      // printf("A child process ended\n");
}

int main(int argc, char *argv[]) {
	int sockfd, newsockfd, portno, clilen;
	// char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	/*
	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}
	*/
	sockfd = socket(AF_INET, SOCK_STREAM, 0); // open socket to receive requests
	if (sockfd < 0)
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	// portno = atoi(argv[1]);
	portno = 6900; // local debug
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno); // guarantee we represent this port in internet common lang.
	if (bind(sockfd, (struct sockaddr *) &serv_addr, // bind socket to ^
		 sizeof(serv_addr)) < 0)
		error("ERROR on binding");

	
	listen(sockfd,100); // link listener, backlog 5
	clilen = sizeof(cli_addr);

  signal(SIGCHLD, fireman);           
	while (1) { // loop here to close(sockfd) i - infinite requests
		int status;
	//     v struct, like in thread
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
		// ^ suspend server execution - wait til it gets a request, an create a new socket

		// free up parent to receive more conn.
		if (fork() == 0) {
			if (newsockfd < 0)
				error("ERROR on accept");
			
			class container message;
			// bzero(buffer,256);
			// n = read(newsockfd,buffer,255);
			status = read(newsockfd, &message, sizeof(struct container));
			if (status < 0) error("ERROR reading from socket");
			printf("\nEcho: %d",message.value);
			
			long long int bin = 0, place = 0;

			while (message.value > 0) { 
				bin += (message.value % 2) * pow(10, place);
				place += 1;
				message.value /= 2; // floor
			}

			message.bin = to_string(bin);
			message.e_Gamma = string(to_string(bin).length() - 1, '0') + to_string(bin);
			cout << '\n' << message.bin << " - " << message.e_Gamma << '\n';
			/*
			// segmentation faults 
			sprintf(message.bin, "%d", bin);
			fflush(stdout); 
			sprintf(message.e_Gamma, "%0*d%s", (int)(strlen(message.bin)- 1), 0, message.bin);
			fflush(stdout); 
			printf("\n %s - %s", message.bin, message.e_Gamma);
			fflush(stdout); 
			*/
			const char* c_bin = (message.bin).c_str();
			const char* c_e_Gamma = (message.e_Gamma).c_str();
			status = write(newsockfd, c_bin, sizeof(string));
			if (status < 0) printf("ERROR writing to socket\n");

			status = write(newsockfd, c_e_Gamma, sizeof(string));
			if (status < 0) printf("ERROR writing to socket\n");


			// status = write(newsockfd,"I got your message",18); // acknowledge
			// if (status < 0) error("ERROR writing to socket");
			close(newsockfd);
			// fireman - matches system signal to function
			// catch status of child process ending
			// guarantee we're not generating a zombie process
			// makes sure no extra resources
		}
	close(newsockfd);
	// wait(0); // fork
	// fireman activated by _exit(0);
	// we terminate, it cleans
	}
	_exit(0);
	close(sockfd); // outside of inf loop

	return 0;
}
