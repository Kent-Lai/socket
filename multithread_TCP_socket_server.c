#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>

#define MAXIMUM_CONNECTIONS 10
#define MAXIMUM_BUFFER_SIZE 1024
#define MAXIMUM_PREFIX_SIZE 32

struct socket_info
{
	int index;
	int sfd_comnct;
	struct sockaddr_in si_client;
};

void* client_connection(void* socket_info);

int connection_count = 0;
struct socket_info server_accept_info[MAXIMUM_CONNECTIONS];

int main(int argc, char* argv[])
{
	struct sockaddr_in si_server;
	si_server.sin_family = AF_INET;
	si_server.sin_port = htons(atoi(argv[1]));
	si_server.sin_addr.s_addr = htonl(INADDR_ANY);

	int sfd_server_listen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	bind(sfd_server_listen, (struct sockaddr*)&si_server, sizeof(si_server));
	listen(sfd_server_listen, MAXIMUM_CONNECTIONS);

	struct sockaddr_in si_client;
	int si_client_len = sizeof(si_client);

	pthread_t thrd_client[MAXIMUM_CONNECTIONS];
	while((connection_count < MAXIMUM_CONNECTIONS) && ((server_accept_info[connection_count].sfd_comnct = accept(sfd_server_listen, (struct sockaddr*)&server_accept_info[connection_count].si_client, (socklen_t*)&si_client_len/*sizeof(server_accept_info[connection_count].si_client)*/)) != -1))
	{
		server_accept_info[connection_count].index = connection_count;
		if(pthread_create(&thrd_client[connection_count], NULL, client_connection, (void*)&server_accept_info[connection_count]))
		{
			printf("Thread creation fail.\n");
			return 0;
		}
		connection_count++;
	}

	return 0;
}

void* client_connection(void* socket_info)
{
	char recv_buffer[MAXIMUM_BUFFER_SIZE];
	int recv_len;
	struct socket_info client_info = *(struct socket_info*)socket_info;
	//client_info.sfd_comnct = (*(socket_info*)socket_info).sfd_comnct;
	//client_info.si_client = (*(socket_info*)socket_info).si_client;

	char msg_prefix[MAXIMUM_PREFIX_SIZE]; 
	strcpy(msg_prefix, "From ");
	char* host_ip_addr = inet_ntoa(client_info.si_client.sin_addr);
	strcat(msg_prefix, host_ip_addr);
	strcat(msg_prefix, " ");
	char msg[MAXIMUM_PREFIX_SIZE + MAXIMUM_BUFFER_SIZE];
	while((recv_len = recv(client_info.sfd_comnct, recv_buffer, MAXIMUM_BUFFER_SIZE, 0)) > 0)
	{
		recv_buffer[recv_len] = 0;
		strcpy(msg, msg_prefix);
		strcat(msg, recv_buffer);
		printf("%s\n", msg);
		int i;
		for(i = 0; i < connection_count; i++)
		{
			if(server_accept_info[i].sfd_comnct != client_info.sfd_comnct)
			{
				send(server_accept_info[i].sfd_comnct, msg, strlen(msg), 0);
			}
		}
	}
	close(client_info.sfd_comnct);
	int i;
	for(i = client_info.index + 1; i < connection_count; i++)
	{
		server_accept_info[i - 1] = server_accept_info[i];
	}
	connection_count--;
	pthread_exit(NULL);
} 
