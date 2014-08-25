#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>

#define MAXIMUM_BUFFER_SIZE 1024

void* receive_message(void* sfd);

int main(int argc, char* argv[])
{
	char send_buffer[MAXIMUM_BUFFER_SIZE];

	struct sockaddr_in si_server;
	int si_server_len = sizeof(si_server);
	int sfd_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	si_server.sin_family = AF_INET;
	si_server.sin_port = htons(atoi(argv[2]));
	inet_aton(argv[1], &si_server.sin_addr);

	connect(sfd_server, (struct sockaddr*)&si_server, si_server_len);

	pthread_t thrd_recv_msg;
	if(pthread_create(&thrd_recv_msg, NULL, receive_message, (void*)&sfd_server))
	{
		printf("Thread creation fail.\n");
		return 0;
	}

	char* nickname = argv[3];
	strcpy(send_buffer, nickname);
	strcat(send_buffer, " : ");
	int prefix_len = strlen(send_buffer);
	while(1)
	{
		int i = prefix_len;
		char t_c;
		while(scanf("%c", &t_c) != EOF)
		{
			if(t_c == '\n')
				break;
			send_buffer[i] = t_c;
			i++;
		}
		send_buffer[i] = 0;
		send(sfd_server, send_buffer, strlen(send_buffer), 0);
	}

	close(sfd_server);

	return 0;
}

void* receive_message(void* sfd)
{
	char recv_buffer[MAXIMUM_BUFFER_SIZE];
	int recv_len;
	int sfd_comnct = *(int*)sfd;
	while((recv_len = recv(sfd_comnct, recv_buffer, MAXIMUM_BUFFER_SIZE, 0)) > 0)
	{
		recv_buffer[recv_len] = 0;
		printf("%s\n", recv_buffer);
	}
	pthread_exit(NULL);
}
