#ifndef HAMPOD_QUEUE
#define HAMPOD_QUEUE
#include "hampod_firm_packet.h"

typedef struct Node {
  Inst_packet *packet;
  struct Node *next;
} Node;

typedef struct Packet_queue {
  Node *head;
  Node *tail;
} Packet_queue;

Packet_queue *create_packet_queue();
void enqueue(Packet_queue *queue, Inst_packet *packet);
Inst_packet *dequeue(Packet_queue *queue);
void destroy_queue(Packet_queue *queue);
int is_empty(Packet_queue *queue);
void clear_queue(Packet_queue *queue);
#ifndef SHAREDLIB
#include "hampod_queue.c"
#endif

#endif
