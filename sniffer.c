#include <stdio.h>
#include "util.h"
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>

#define DEBUG 0 
#define BYTE_SZ (int)(sizeof(unsigned int)/sizeof(unsigned char))

typedef struct pkg
{

	int addr;
	int func_code;
	uint8_t data;
	int crc;

} pkg;

int fd_send[2], fd_dest[2]; // one pipe from ch_send to parent and from parent to ch_dest

static void childSender()
{

	int i;
	unsigned int n = 0xDEADBEEF; // dummy var to send
	unsigned char *w = (unsigned char *)&n; // got to send it byte by byte
	
#if DEBUG	
	printf("I is Ch_sender \n");
#endif

	close(fd_send[0]); // close read end 
	for(i = 0; i < BYTE_SZ; i++)
	{
		write(fd_send[1], (w + i), sizeof(char));
		printf("Sender: %x\n", w[i]);
	}
}
static void childDestination()
{
#if DEBUG
	printf("I is Ch_destination \n");
#endif

	unsigned char *w;
	unsigned int buf;
	int i;

	w = (unsigned char *)&buf;

	close(fd_dest[1]); // close write end;
	for(i = 0; i < BYTE_SZ; i++)
	{
		read(fd_dest[0], (w + i), sizeof(char));
		printf("Receiver: %x \n", *(w + i));
	}

}

int main()
{
	int ch_sender, ch_dest;
	int rc, i = 0;
	unsigned int buf;
	unsigned char *w;

	w = (unsigned char*)&buf;
	rc = pipe(fd_send);
	DIE(rc == -1, "sender pipe");

	ch_sender = fork();	
	if(!ch_sender) childSender();
	else
	{

		rc = pipe(fd_dest); // create receiver pipe
		DIE(rc == -1, "receiver pipe");

		ch_dest = fork();
		if(!ch_dest) childDestination();
		else
		{
			// parent code
			close(fd_send[1]); // close writing end of sender
			close(fd_dest[0]); // close reading end of receiver

			for(i = 0; i < (int)(sizeof(unsigned int)/sizeof(unsigned char)); i++)
			{
				read(fd_send[0], (w + i), sizeof(char));
#if DEBUG
				printf("Parent here: I received %x\n", w[i]);
#endif
				write(fd_dest[1], (w + i), sizeof(char));
			}
			rc = pipe(fd_dest);
			DIE(rc == -1, "pipe");

			wait();
#if DEBUG
			printf("I am parent \n");
#endif
		}
	}
	return 0;
}
