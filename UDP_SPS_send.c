#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> //socket
#include <sys/socket.h> //socket
#include <arpa/inet.h> //htons htonl
#include <netinet/in.h> //inet_aton
#include <unistd.h> //close
#include <pthread.h>
#include <sys/time.h>

typedef unsigned char bit;
typedef unsigned char byte;

#define MAX_BUFFER_SIZE 1024
#define mega 1000000
#define DEBUG

unsigned long long get_timestamp();

int main(int argc, char* argv[])
{
	printf("Data : ");
	byte byte_data[MAX_BUFFER_SIZE];
	char t_c;
	int byte_count = 0;
	while(scanf("%c", &t_c) != EOF)
	{
		if(t_c == '\n')
			break;
		byte_data[byte_count] = t_c;
		byte_count++;
	}

	bit* bit_data = (bit*)malloc(byte_count * 8 * sizeof(bit));
	int i;
	int j;
	int bit_count = 0;
	int mask = 0x80; // 1000 0000
	for(i = 0; i < byte_count; i++)
	{
		for(j = 0; j < 8; j++)
		{
			bit_data[bit_count] = (byte_data[i] & (mask >> j)) >> (7 - j);
			bit_count++;
		}
	}

	printf("BPD(4, 8, 12) : ");
	int bpd;
	scanf("%d", &bpd);

	int q = bit_count / bpd;
	int r = bit_count % bpd;
	int total_ports = r == 0? q : q + 1;
	unsigned short* source_ports = (unsigned short*)malloc(total_ports * sizeof(unsigned short));
	int hd_v;
	for(i = 0; i < q; i++)
	{
		hd_v = 0;
		for(j = 0; j < bpd; j++)
		{
			hd_v = (hd_v << 1) | bit_data[i * bpd + j];
		}
		source_ports[i] = (bpd / 4  << 14) | hd_v;
	}
	if(r != 0)
	{
		hd_v = 0;
		for(i = 0; i < r; i++)
		{
			hd_v = (hd_v << 1) | bit_data[q * bpd + i];
		}
		source_ports[q] = (r / 4 << 14) | hd_v;
	}

	struct sockaddr_in si_receiver, si_sender;
	si_receiver.sin_family = AF_INET;
	si_receiver.sin_port = htons(atoi(argv[2]));
	inet_aton(argv[1], &si_receiver.sin_addr);

	si_sender.sin_family = AF_INET;
	si_sender.sin_addr.s_addr = htonl(INADDR_ANY);

	int sfd_comnct;
	int si_receiver_len = sizeof(si_receiver);

	char send_buffer[MAX_BUFFER_SIZE]; //byte 0 undefined
	unsigned long long timestamp = get_timestamp();
	memcpy(send_buffer + 1, &timestamp, sizeof(unsigned long long)); //byte 1~8 timestamp
#ifdef DEBUG
	printf("Timstamp : %u\n", timestamp);
	unsigned long long t_timestamp;
	memcpy(&t_timestamp, send_buffer + 1, sizeof(unsigned long long));
	printf("Check timestamp : %u\n", t_timestamp);
#endif

	char input_buffer[MAX_BUFFER_SIZE];
	
	printf("Send Messages : \n");
	char end_flag = 0;
	unsigned short seq_num = 0;
	while(scanf("%s", input_buffer) != EOF)
	{
		memcpy(send_buffer + 9, &seq_num, sizeof(unsigned short)); //byte 9~10 sequence number
#ifdef DEBUG
		printf("Sequence number : %u\n", seq_num);
		unsigned short t_seq_num;
		memcpy(&t_seq_num, send_buffer + 9, sizeof(unsigned short));
		printf("Check sequence number : %u\n", t_seq_num);
#endif
		//byte 11~12 ack
		
		memcpy(send_buffer + 13, input_buffer, strlen(input_buffer)); //byte 13~n remaining message
#ifdef DEBUG
		printf("Remaining message : %s\n", input_buffer);
		char rem_msg[MAX_BUFFER_SIZE];
		memcpy(rem_msg, send_buffer + 13, strlen(input_buffer));
		rem_msg[strlen(input_buffer)] = 0;
		printf("Check remaining message : %s\n", rem_msg);
#endif
		if(seq_num < total_ports)
		{
			si_sender.sin_port = htons(source_ports[seq_num]);
			printf("Remaining %d datagram\n", total_ports - seq_num);
		}
		else
		{
			printf("Complete\n");
			si_sender.sin_port = htons(1234);
			end_flag = 1;
		}
		sfd_comnct = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if(bind(sfd_comnct, (struct sockaddr*)&si_sender, sizeof(si_sender)) != 0)
			printf("bind error\n");
		sendto(sfd_comnct, send_buffer, strlen(input_buffer) + 13, 0, (struct sockaddr*)&si_receiver, si_receiver_len);
		close(sfd_comnct);
		seq_num++;

		if(end_flag == 1)
			break;
	}

	return 0;
}

unsigned long long get_timestamp()
{
	struct timeval tv_temp;
	gettimeofday(&tv_temp, NULL);
	return (unsigned long long)tv_temp.tv_sec * mega + tv_temp.tv_usec;
}
