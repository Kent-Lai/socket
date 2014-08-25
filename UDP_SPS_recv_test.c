#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> //socket
#include <sys/socket.h> //socket
#include <arpa/inet.h> //htons htonl
#include <netinet/in.h> //inet_aton
#include <unistd.h> //close
#include <pthread.h>

#define MAX_BUFFER_SIZE 1024

int main(int argc, char* argv[])
{
	struct sockaddr_in si_receiver, si_sender;
	si_receiver.sin_family = AF_INET;
	si_receiver.sin_port = htons(atoi(argv[1]));
	si_receiver.sin_addr.s_addr = htonl(INADDR_ANY);

	int sfd_receiver = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP), si_sender_len = sizeof(si_sender);

	bind(sfd_receiver, (struct sockaddr*)&si_receiver, sizeof(si_receiver));

	char recv_buffer[MAX_BUFFER_SIZE];
	
	while(recvfrom(sfd_receiver, recv_buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr*)&si_sender, (socklen_t*)&si_sender_len))
	{
		printf("%d\n", ntohs(si_sender.sin_port));
	}

	return 0;
}
