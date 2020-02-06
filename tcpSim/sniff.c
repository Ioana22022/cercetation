#include<stdio.h>
#include<stdlib.h>
#include<netinet/tcp.h>
#include<netinet/ip.h>
#include "util.h"

struct sockaddr_in source,dest;
int sock_raw;

void processPacket(unsigned char *buf, int len);

int main()
{
	struct sockaddr saddr;
	struct in_addr in;
	int saddr_size , data_size;

	printf("Begging======\n");
	unsigned char *buffer = (unsigned char *)malloc(65536);
	sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
	DIE(sock_raw < 0, "socket creation");
	
	while(1)	// keep on sniffing
	{
		saddr_size = sizeof(saddr);
		data_size = recvfrom(sock_raw, buffer, 65536, 0, &saddr, &saddr_size);	
		DIE(data_size < 0, "recvfrom");
		processPacket(buffer, data_size);
	}
	return 0;
}

void processPacket(unsigned char *buf, int len)
{
	struct iphdr *iph = (struct iphdr*)buf;
	if(iph->protocol == 6) //then packet is TCP
	{
		unsigned short iphdrlen;
		iphdrlen = iph->ihl*4;
		
		struct tcphdr *tcph = (struct tcphdr*)(buf + iphdrlen);

	}
}

