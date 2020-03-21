#include<netinet/in.h>
#include<errno.h>
#include<netdb.h>
#include<stdio.h>	//for standard things
#include<stdlib.h>	//malloc
#include<string.h>	//strlen

#include<netinet/ip_icmp.h>	//provides declarations for icmp header
#include<netinet/udp.h>	//provides declarations for udp header
#include<netinet/tcp.h>	//provides declarations for tcp header
#include<netinet/ip.h>	//provides declarations for ip header
#include<netinet/if_ether.h>	//for eth_p_all
#include<net/ethernet.h>	//for ether_header
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/ioctl.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>

void processpacket(unsigned char* , int);
void print_ip_header(unsigned char* , int);
void print_tcp_packet(unsigned char * , int );
void print_udp_packet(unsigned char * , int );
void print_icmp_packet(unsigned char* , int );
void printdata (unsigned char* , int);

struct sockaddr_in source,dest;
int tcp=0,udp=0,icmp=0,others=0,igmp=0,total=0,i,j;	

int main()
{
	int saddr_size , data_size;
	struct sockaddr saddr;
		
	unsigned char *buffer = (unsigned char *) malloc(65536); //its big!
	
	printf("starting...\n");
	
	int sock_raw = socket(af_packet , sock_raw , htons(eth_p_all)) ;
	//setsockopt(sock_raw , sol_socket , so_bindtodevice , "eth0" , strlen("eth0")+ 1 );
	
	if(sock_raw < 0)
	{
		//print the error with proper message
		perror("socket error");
		return 1;
	}
	while(1)
	{
		saddr_size = sizeof saddr;
		//receive a packet
		data_size = recvfrom(sock_raw , buffer , 65536 , 0 , &saddr , (socklen_t*)&saddr_size);
		if(data_size <0 )
		{
			printf("recvfrom error , failed to get packets\n");
			return 1;
		}
		//now process the packet
		processpacket(buffer , data_size);
	}
	close(sock_raw);
	printf("finished");
	return 0;
}

void processpacket(unsigned char* buffer, int size)
{
	//get the ip header part of this packet , excluding the ethernet header
	struct iphdr *iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
	++total;
	switch (iph->protocol) //check the protocol and do accordingly...
	{
		case 1:  //icmp protocol
			++icmp;
			print_icmp_packet(buffer , size);
			break;
		
		case 2:  //igmp protocol
			++igmp;
			break;
		
		case 6:  //tcp protocol
			++tcp;
			print_tcp_packet(buffer , size);
			break;
		
		case 17: //udp protocol
			++udp;
			print_udp_packet(buffer , size);
			break;
		
		default: //some other protocol like arp etc.
			++others;
			break;
	}
	printf("tcp : %d   udp : %d   icmp : %d   igmp : %d   others : %d   total : %d\r", tcp , udp , icmp , igmp , others , total);
}

void print_ethernet_header(unsigned char* buffer, int size)
{
	struct ethhdr *eth = (struct ethhdr *)buffer;
	
	printf("\n");
	printf("ethernet header\n");
	printf("   |-destination address : %.2x-%.2x-%.2x-%.2x-%.2x-%.2x \n", eth->h_dest[0] , eth->h_dest[1] , eth->h_dest[2] , eth->h_dest[3] , eth->h_dest[4] , eth->h_dest[5] );
	printf("   |-source address      : %.2x-%.2x-%.2x-%.2x-%.2x-%.2x \n", eth->h_source[0] , eth->h_source[1] , eth->h_source[2] , eth->h_source[3] , eth->h_source[4] , eth->h_source[5] );
	printf("   |-protocol            : %u \n",(unsigned short)eth->h_proto);
}

void print_ip_header(unsigned char* buffer, int size)
{
	print_ethernet_header(buffer , size);
  
	unsigned short iphdrlen;
		
	struct iphdr *iph = (struct iphdr *)(buffer  + sizeof(struct ethhdr) );
	iphdrlen =iph->ihl * 4;
	
	memset(&source, 0, sizeof(source));
	source.sin_addr.s_addr = iph->saddr;
	
	memset(&dest, 0, sizeof(dest));
	dest.sin_addr.s_addr = iph->daddr;
	
	printf("\n");
	printf("ip header\n");
	printf("   |-ip version        : %d\n",(unsigned int)iph->version);
	printf("   |-ip header length  : %d dwords or %d bytes\n",(unsigned int)iph->ihl,((unsigned int)(iph->ihl))*4);
	printf("   |-type of service   : %d\n",(unsigned int)iph->tos);
	printf("   |-ip total length   : %d  bytes(size of packet)\n",ntohs(iph->tot_len));
	printf("   |-identification    : %d\n",ntohs(iph->id));
	//printf("   |-reserved zero field   : %d\n",(unsigned int)iphdr->ip_reserved_zero);
	//printf("   |-dont fragment field   : %d\n",(unsigned int)iphdr->ip_dont_fragment);
	//printf("   |-more fragment field   : %d\n",(unsigned int)iphdr->ip_more_fragment);
	printf("   |-ttl      : %d\n",(unsigned int)iph->ttl);
	printf("   |-protocol : %d\n",(unsigned int)iph->protocol);
	printf("   |-checksum : %d\n",ntohs(iph->check));
	printf("   |-source ip        : %s\n",inet_ntoa(source.sin_addr));
	printf("   |-destination ip   : %s\n",inet_ntoa(dest.sin_addr));
}

void print_tcp_packet(unsigned char* buffer, int size)
{
	unsigned short iphdrlen;
	
	struct iphdr *iph = (struct iphdr *)(buffer  + sizeof(struct ethhdr));
	iphdrlen = iph->ihl*4;
	
	struct tcphdr *tcph=(struct tcphdr*)(buffer + iphdrlen + sizeof(struct ethhdr));
			
	int header_size =  sizeof(struct ethhdr) + iphdrlen + tcph->doff*4;
	
	printf("\n\n***********************tcp packet*************************\n");	
		
	print_ip_header(buffer,size);
		
	printf("\n");
	printf("tcp header\n");
	printf("   |-source port      : %u\n",ntohs(tcph->source));
	printf("   |-destination port : %u\n",ntohs(tcph->dest));
	printf("   |-sequence number    : %u\n",ntohl(tcph->seq));
	printf("   |-acknowledge number : %u\n",ntohl(tcph->ack_seq));
	printf("   |-header length      : %d dwords or %d bytes\n" ,(unsigned int)tcph->doff,(unsigned int)tcph->doff*4);
	//printf("   |-cwr flag : %d\n",(unsigned int)tcph->cwr);
	//printf("   |-ecn flag : %d\n",(unsigned int)tcph->ece);
	printf("   |-urgent flag          : %d\n",(unsigned int)tcph->urg);
	printf("   |-acknowledgement flag : %d\n",(unsigned int)tcph->ack);
	printf("   |-push flag            : %d\n",(unsigned int)tcph->psh);
	printf("   |-reset flag           : %d\n",(unsigned int)tcph->rst);
	printf("   |-synchronise flag     : %d\n",(unsigned int)tcph->syn);
	printf("   |-finish flag          : %d\n",(unsigned int)tcph->fin);
	printf("   |-window         : %d\n",ntohs(tcph->window));
	printf("   |-checksum       : %d\n",ntohs(tcph->check));
	printf("   |-urgent pointer : %d\n",tcph->urg_ptr);
	printf("\n");
	printf("                        data dump                         ");
	printf("\n");
		
	printf("ip header\n");
	printdata(buffer,iphdrlen);
		
	printf("tcp header\n");
	printdata(buffer+iphdrlen,tcph->doff*4);
		
	printf("data payload\n");	
	printdata(buffer + header_size , size - header_size );
						
	printf("\n###########################################################");
}

void print_udp_packet(unsigned char *buffer , int size)
{
	
	unsigned short iphdrlen;
	
	struct iphdr *iph = (struct iphdr *)(buffer +  sizeof(struct ethhdr));
	iphdrlen = iph->ihl*4;
	
	struct udphdr *udph = (struct udphdr*)(buffer + iphdrlen  + sizeof(struct ethhdr));
	
	int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof udph;
	
	printf("\n\n***********************udp packet*************************\n");
	
	print_ip_header(buffer,size);			
	
	printf("\nudp header\n");
	printf("   |-source port      : %d\n" , ntohs(udph->source));
	printf("   |-destination port : %d\n" , ntohs(udph->dest));
	printf("   |-udp length       : %d\n" , ntohs(udph->len));
	printf("   |-udp checksum     : %d\n" , ntohs(udph->check));
	
	printf("\n");
	printf("ip header\n");
	printdata(buffer , iphdrlen);
		
	printf("udp header\n");
	printdata(buffer+iphdrlen , sizeof udph);
		
	printf("data payload\n");	
	
	//move the pointer ahead and reduce the size of string
	printdata(buffer + header_size , size - header_size);
	
	printf("\n###########################################################");
}

void print_icmp_packet(unsigned char* buffer , int size)
{
	unsigned short iphdrlen;
	
	struct iphdr *iph = (struct iphdr *)(buffer  + sizeof(struct ethhdr));
	iphdrlen = iph->ihl * 4;
	
	struct icmphdr *icmph = (struct icmphdr *)(buffer + iphdrlen  + sizeof(struct ethhdr));
	
	int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof icmph;
	
	printf("\n\n***********************icmp packet*************************\n");	
	
	print_ip_header(buffer , size);
			
	printf("\n");
		
	printf("icmp header\n");
	printf("   |-type : %d",(unsigned int)(icmph->type));
			
	if((unsigned int)(icmph->type) == 11)
	{
		printf("  (ttl expired)\n");
	}
	else if((unsigned int)(icmph->type) == icmp_echoreply)
	{
		printf("  (icmp echo reply)\n");
	}
	
	printf("   |-code : %d\n",(unsigned int)(icmph->code));
	printf("   |-checksum : %d\n",ntohs(icmph->checksum));
	//printf("   |-id       : %d\n",ntohs(icmph->id));
	//printf("   |-sequence : %d\n",ntohs(icmph->sequence));
	printf("\n");

	printf("ip header\n");
	printdata(buffer,iphdrlen);
		
	printf("udp header\n");
	printdata(buffer + iphdrlen , sizeof icmph);
		
	printf("data payload\n");	
	
	//move the pointer ahead and reduce the size of string
	printdata(buffer + header_size , (size - header_size) );
	
	printf("\n###########################################################");
}

void printdata(unsigned char* data, int size)
{
	int i, j;
	for(i = 0; i < size; i++)
	{
		if(i != 0 && i %16 ==0)   //if one line of hex printing is complete...
		{
			printf("         ");
			for(j = i - 16; j < i; j++)
			{
				if(data[j] >= 32 && data[j] <= 128)
					printf("%c",(unsigned char)data[j]); //if its a number or alphabet
				
				else printf("."); //otherwise print a dot
			}
			printf("\n");
		} 
		
		if(i%16 == 0) printf("   ");
			printf(" %02x",(unsigned int)data[i]);
				
		if(i == (size-1))  //print the last spaces
		{
			for(j = 0; j < (15 - i % 16); j++) 
			{
			  printf("   "); //extra spaces
			}
			
			printf("         ");
			
			for(j = i - i%16; j <= i; j++)
			{
				if(data[j] >= 32 && data[j] <= 128) 
				{
				  printf("%c",(unsigned char)data[j]);
				}
				else 
				{
				  printf(".");
				}
			}
			
			printf("\n");
		}
	}
}


