// -*- coding: iso-8859-1 -*-
#include "openbus/detail/ticket.h"

#define FLAG(B, I) (B & (1UL << I))
#define SET(B, I) (B |= (1UL << I))
#define CLEAR(B, I) (B &= ~(1UL << I))
#define ROL(B, S) ((B << S) | (B >> ((sizeof(B) * CHAR_BIT) - S)))


static void discardBase(tickets_History *h)
{
	++h->base;
	if (h->bits) 
  {
		bitidx i;
		for (i = 0; i < TICKETS_SIZE; ++i) 
    {
			if (!FLAG(h->bits, h->index)) break;
			CLEAR(h->bits, h->index);
			h->index = (h->index + 1) % TICKETS_SIZE;
			++h->base;
		}
		h->index = (h->index + 1) % TICKETS_SIZE;
	}
}


void tickets_init(tickets_History *h)
{
	h->bits = 0;
	h->index = 0;
	h->base = 0;
}

int tickets_check(tickets_History *h, tickets_Value id)
{
	if (id < h->base) 
  {
		return 0;
	}
  else if (id == h->base) 
  {
		discardBase(h);
		return 1;
	} 
  else 
  { /* id > h->base */
		tickets_Value shift = id - h->base - 1;
		if (shift < TICKETS_SIZE) 
    {
			bitidx index = (h->index + shift) % TICKETS_SIZE;
			if (FLAG(h->bits, index)) return 0;
			SET(h->bits, index);
			return 1;
		}
    else
    {
			unsigned int i;
			tickets_Value extra = shift - TICKETS_SIZE;
			if (extra < TICKETS_SIZE) 
      {
				for (i = 0; i < extra; ++i) 
        {
					CLEAR(h->bits, h->index);
					h->index = (h->index + 1) % TICKETS_SIZE;
				}
			} 
      else 
      {
				h->bits = 0;
				h->index = 0;
			}
			h->base += extra;
			discardBase(h);
			return tickets_check(h, id);
		}
	}
}
