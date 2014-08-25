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
	//printf("%s\n%s\n", argv[1], argv[2]);
	int sockfd;
	struct sockaddr_in dest;
	char sbuffer[128] = "Client connected.";
	char rbuffer[128];
	char* ip = argv[1];
	int port = atoi(argv[2]);
	//printf("%s\n%d\n", ip, port);

	/* create socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	/* initialize value in dest */
	bzero(&dest, sizeof(dest));
	dest.sin_family = AF_INET;
	dest.sin_port = htons(port);
	inet_aton(ip, &dest.sin_addr);
	//dest.sin_addr.s_addr = inet_addr("127.0.0.1");
	/* Connecting to server */

	connect(sockfd, (struct sockaddr*)&dest, sizeof(dest));

	send(sockfd, sbuffer, sizeof(sbuffer), 0);
	while(recv(sockfd, rbuffer, sizeof(rbuffer), 0) != -1)
	{
		printf("length: %d\nServer: %s\n", strlen(rbuffer), rbuffer);
		scanf("%s", sbuffer);
		send(sockfd, sbuffer, sizeof(sbuffer), 0);
	}
	printf("Connect fail.");
	/* Close connection */
	close(sockfd);
	return 0;
}

