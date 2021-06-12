#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"

#define HOST "127.0.0.1"
#define PORT 10000

void send_frame(msg *pt, char buf[], int bufsize, int type)
{
  pkt p;
  /* cleanup msg */
  memset(pt, 0, sizeof(msg));
  memset(&p, 0, sizeof(pkt));

  /* copy buffer to pkt*/
  memcpy(p.payload, buf, bufsize);
  p.type = type;

  /* copy pkt to msg*/
  pt->len = strlen(p.payload) + sizeof(int) + 1;
  memcpy(pt->payload, &p, pt->len);

  /*send message*/
  int rc = send_message(pt);

  if(rc < 0)
  {
    printf("[SENDER] Error while sending. Exiting. \n");
    exit(1);
  }

}

void check_ack(msg *pt)
{
  pkt p;
  /* cleanup msg */
  memset(pt, 0, sizeof(msg));
  memset(&p, 0, sizeof(pkt));
  int rc = recv_message(pt);
  if(rc < 0)
  {
    printf("[SENDER] Receive error. Exiting.\n");
    exit(1);
  }

  p = *((pkt *) pt->payload);
  if(p.type != (int) ACK)
  {
    printf("[SENDER] Did not receive ACK! Exiting. \n");
    exit(1);
  }
  printf("[SENDER] Received %s: \n", p.payload);
}

int main(int argc, char *argv[])
{
	msg t;
	int i;


  printf("argc = %d", argc);
  /*if(argc != 2)
  {
    printf("Usage: %s <bdp>. Exiting. \n", argv[0]);
    exit(1);
  }
*/
	printf("[SENDER] Starting.\n");
	init(HOST, PORT);
  char buf[256];

	/* printf("[SENDER]: BDP=%d\n", atoi(argv[1])); */
  int bdp = atoi(argv[1]);
  int w = bdp / (8 * MSGSIZE);
  printf("[SENDER] w is %d and bdp is %d\n", w, bdp);

  for(i = 0; i < w; i++)
  {
		/* send frame */
    sprintf(buf, "ceva_%d", i);
    send_frame(&t, buf, strlen(buf), (int)DATA);
    printf("[SENDER] Sent data: %s.\n", buf);
  }


  for (i = w; i < (int)(COUNT); i++)
  {
		/* wait for ACK */
    check_ack(&t);

		/* send frame */
    sprintf(buf, "ceva_%d", i);
    send_frame(&t, buf, strlen(buf), (int)DATA);
    printf("[SENDER] Sent data: %s.\n", buf);

  }

#if 1
	for (i = 0; i < w; i++) {
		/* send frame */
    //sprintf(buf, "ceva_%d", (int)(COUNT) - w + i);
    //send_frame(&t, buf, strlen(buf), (int)DATA);

		/* wait for ACK */
    check_ack(&t);
	}
#endif

	printf("[SENDER] Job done, all sent.\n");

	return 0;
}
