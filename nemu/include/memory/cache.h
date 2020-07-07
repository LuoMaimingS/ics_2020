#ifndef __CACHE_H__
#define __CACHE_H__

#include "nemu.h"

typedef struct {
	bool valid;
	uint32_t tag;
	uint8_t block[64];
}CacheLine;

void init_cache();

void cache_write(paddr_t paddr, size_t len, uint32_t data, CacheLine* cache);
uint32_t cache_read(paddr_t paddr, size_t len, CacheLine* cache);

extern CacheLine Cache[];

#endif
