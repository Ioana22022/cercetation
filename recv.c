#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10001

void check_msg(msg *pt)
{
  pkt p;
  int rc = recv_message(pt);
  if(rc < 0)
  {
    printf("[RECEIVER] Receive error. Exiting.\n");
    exit(1);
  }

  p = *((pkt *) pt->payload);
  if(p.type != (int) DATA)
  {
    printf("[RECEIVER] Did not receive DATA! Exiting. \n");
    exit(1);
  }

  printf("[RECEIVER] Got %s. \n", p.payload);

}

void send_ack(msg *pt, int i)
{
  pkt p;
  /* cleanup msg */
  memset(pt, 0, sizeof(msg));
  memset(&p, 0, sizeof(pkt));

  /* copy buffer to pkt*/
  p.type = (int) ACK;

  /* copy pkt to msg*/
  sprintf(p.payload, "ACK_%d", i);
  pt->len = strlen(p.payload) + sizeof(int) + 1;
  memcpy(pt->payload, &p, pt->len);

  /*send message*/
  int rc = send_message(pt);

  if(rc < 0)
  {
    printf("[RECEIVER] Error while sending. Exiting. \n");
    exit(1);
  }

}

int main(void)
{
	msg r;
	int i;
	
	printf("[RECEIVER] Starting.\n");
	init(HOST, PORT);
	
	for (i = 0; i < COUNT; i++) {
		/* wait for message */
    check_msg(&r);
		
		/* send ACK */
    send_ack(&r, i);
	}

	printf("[RECEIVER] Finished receiving..\n");
	return 0;
}
