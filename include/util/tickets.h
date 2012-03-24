#ifndef TICKET_HISTORY_H
#define TICKET_HISTORY_H

#include <limits.h>

#define TICKETS_SIZE (sizeof(tickets_BitMap) * CHAR_BIT)

typedef unsigned long tickets_Value;
typedef unsigned long tickets_BitMap;
typedef unsigned char bitidx;

struct tickets_HistoryData {
	tickets_Value base;
	tickets_BitMap bits;
	bitidx index;
};

typedef struct tickets_HistoryData tickets_History;

void tickets_init(tickets_History *h);
tickets_Value tickets_check(tickets_History *h, tickets_Value id);
tickets_Value tickets_expected(tickets_History *h);
tickets_Value tickets_received(tickets_History *h);

void tickets_print(tickets_History *h);

#endif
