#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	int sockfd;
	struct sockaddr_in dest;
	char sbuffer [128] = "Connecting successful.";
	char rbuffer [128];

	/* create socket , same as client */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	/* initialize structure dest */
	bzero(&dest, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(atoi(argv[1]));
	/* this line is different from client */
	dest.sin_addr.s_addr = INADDR_ANY;

	/* Assign a port number to socket */
	bind(sockfd, (struct sockaddr*)&dest, sizeof(dest));

	/* make it listen to socket with max 20 connections */
	listen(sockfd, 20);
	int clientfd;
	struct sockaddr_in client_addr;
	int addrlen = sizeof(client_addr);
	while(1)
	{
		clientfd = accept(sockfd, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen);

		/* infinity loop -- accepting connection from client forever */
		send(clientfd, sbuffer, sizeof(sbuffer), 0);
		while(recv(clientfd, rbuffer, sizeof(rbuffer), 0) != -1)
		{
			printf("length: %d\nClient: %s\n",strlen(rbuffer),rbuffer);
			scanf("%s", sbuffer);
			send(clientfd, sbuffer, sizeof(sbuffer), 0);
		}
		close(clientfd);
	}

	/* close(server) , but never get here because of the loop */
	close(sockfd);
	return 0;
}

