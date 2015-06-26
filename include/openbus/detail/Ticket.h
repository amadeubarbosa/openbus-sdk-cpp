// -*- coding: iso-8859-1-unix -*-

#ifndef TECGRAF_SDK_OPENBUS_TICKET_H
#define TECGRAF_SDK_OPENBUS_TICKET_H

#include "openbus/detail/decl.hpp"

#include <limits.h>

#define TICKETS_SIZE (sizeof(tickets_BitMap) * CHAR_BIT)

typedef unsigned long tickets_BitMap;
typedef unsigned long tickets_Value;
typedef unsigned char bitidx;

struct tickets_HistoryData 
{
	tickets_Value base;
	tickets_BitMap bits;
	bitidx index;
};

typedef struct tickets_HistoryData tickets_History;

OPENBUS_SDK_DECL void tickets_init(tickets_History *);
OPENBUS_SDK_DECL int tickets_check(tickets_History *, tickets_Value id);
OPENBUS_SDK_DECL void tickets_print(tickets_History *);

#endif
