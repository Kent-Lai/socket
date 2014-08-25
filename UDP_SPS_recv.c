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

struct packet
{
	unsigned long long timestamp;
	unsigned short seq;
	char bit[12];
	int BPD;
};

int main(int argc, char* argv[])
{
	struct sockaddr_in si_receiver, si_sender;
	si_receiver.sin_family = AF_INET;
	si_receiver.sin_port = htons(atoi(argv[1]));
	si_receiver.sin_addr.s_addr = htonl(INADDR_ANY);

	int sfd_receiver = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP), si_sender_len = sizeof(si_sender);

	bind(sfd_receiver, (struct sockaddr*)&si_receiver, sizeof(si_receiver));

	char recv_buffer[MAX_BUFFER_SIZE];
	char bit_str[MAX_BUFFER_SIZE];
	int mask = 0x8000;
	int bit_count = 0;
	int BITcount = 0;
	int i,j;
	struct packet fram[MAX_BUFFER_SIZE];
	int fram_use = 0; 
	int recv_len;

	while((recv_len = recvfrom(sfd_receiver, recv_buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr*)&si_sender, (socklen_t*)&si_sender_len)) > 0)
	{
		unsigned long long t_timestamp;
    		memcpy(&t_timestamp, recv_buffer + 1, sizeof(unsigned long long));

        	unsigned short t_seq_num;
        	memcpy(&t_seq_num, recv_buffer + 9, sizeof(unsigned short));

		char rem_msg[MAX_BUFFER_SIZE];
        	memcpy(rem_msg, recv_buffer + 13, recv_len - 13);
        	rem_msg[recv_len - 13] = 0;

		printf(" : %s\n", rem_msg);

		short secret = ntohs(si_sender.sin_port);
		char temp_bit[16];
		int bpd;

		for(i = 0; i < 16; i++)
			temp_bit[i] = (secret & (mask >> i)) >> (15 - i);

		if(temp_bit[0] == 0 && temp_bit[1] == 0)
		{
			unsigned long long end_time = t_timestamp;
			int sort[100];
			for(i = 0; i < fram_use; i++)
				if(fram[i].timestamp == end_time)
				{
					sort[fram[i].seq] = i;		
				}

			for(i = 0; i < t_seq_num; i++)
				for(j = 0; j < fram[sort[i]].BPD; j++)
				{
					bit_str[BITcount] = fram[sort[i]].bit[j];
					BITcount++;
				}

			char *byte_str = (char*)malloc(BITcount / 8 * sizeof(char));

			for(i = 0; i < BITcount / 8; i++)
			{
				char d = 0;
				for(j = 0; j < 8; j++)
					d = (d << 1) | bit_str[i * 8 + j];
				byte_str[i] = d;
				printf("%c", byte_str[i]);
			}
			printf("\n");

			free(byte_str);
			BITcount = 0;
			continue;
		}
		else if(temp_bit[0] == 0 && temp_bit[1] == 1) bpd = 4;
		else if(temp_bit[0] == 1 && temp_bit[1] == 0) bpd = 8;
		else if(temp_bit[0] == 1 && temp_bit[1] == 1) bpd = 12;

		for(i = 16 - bpd; i < 16; i++)
		{
			fram[fram_use].bit[bit_count] = temp_bit[i];
			bit_count++;
		}

		fram[fram_use].timestamp = t_timestamp;
		fram[fram_use].seq = t_seq_num;
		fram[fram_use].BPD = bpd;

		bit_count = 0;
		fram_use++;
	}

	return 0;
}
