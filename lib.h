#ifndef LIB
#define LIB

#define MSGSIZE		1400
#define PKT_SIZE	1396
#define COUNT		100

enum TYPE {
  DATA, 
  ACK
};

typedef struct {
  int len;
  char payload[MSGSIZE];
} msg;

typedef struct {
  int type;
  char payload[PKT_SIZE];
} pkt;


void init(char* remote,int remote_port);
void set_local_port(int port);
void set_remote(char* ip, int port);
int send_message(const msg* m);
int recv_message(msg* r);

#endif

