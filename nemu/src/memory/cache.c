#include "nemu.h"
#include "memory/memory.h"
#include "memory/cache.h"
#include <memory.h>
#include <stdlib.h>
#include <time.h>

#define CACHELINE_NUM 1024
CacheLine Cache[CACHELINE_NUM];

void init_cache() 
{
    for (int i = 0; i < CACHELINE_NUM; i++)
        Cache[i].valid = 0;
}

uint32_t cache_read(paddr_t paddr, uint32_t len, CacheLine* cache) 
{
    uint32_t ret;
    // paddr 
    // 0000 0 000 0000 0000 000 0 0000 00 00 0000
    //       [T      A        G;G R O U P;O  F  F]
    // [    5,               14,        7,      6]
    uint32_t tag = (paddr >> 13) & 0x4fff;
    uint32_t group_num = (paddr >> 6) & 0x7f;
    uint32_t offset = paddr & 0x3f;
    int i;
    for (i = 0; i < 8; i++) {
        uint32_t cache_tag = (cache[group_num*8+i].tag) & 0x7ffffff;
        //cache hit.
        if (cache_tag == tag && cache[group_num*8 + i].valid == 1) {
            if (offset + len <= 64)
                memcpy(&ret, cache[group_num*8 + i].block + offset, len);
            else {
                uint32_t ret1 = 0, ret2 = 0;
                uint32_t len1 = 64 - offset;
                memcpy(&ret1, cache[group_num*8 + i].block + offset, len1);    
                ret2 = cache_read(paddr - offset + 64, len - len1, cache);
                // little endian.                
                ret = (ret2 << (8 * len1)) | ret1;       
            }
            break;        
        }    
    }
    // cache miss.
    if (i == 8) {
        memcpy(&ret, hw_mem + paddr, len);    
        // load cache.
        for (i = 0; i < 8; i++) {
            if (cache[group_num * 8 + i].valid == 0) {
                cache[group_num * 8 + i].valid = 1;
                cache[group_num * 8 + i].tag = tag;
                memcpy(cache[group_num * 8 + i].block, hw_mem + paddr - offset, 64);
                break;
            }        
        }
        // cache replace(random strategy).
        if (i == 8) {
            srand((unsigned)time(0));
            i = rand() % 8;
            cache[group_num * 8 + i].valid = 1;
            cache[group_num * 8 + i].tag = tag;
            memcpy(cache[group_num * 8 + i].block, hw_mem + paddr - offset, 64);     
        }
    }
    return ret;
}

void cache_write(paddr_t paddr, size_t len, uint32_t data, CacheLine* cache)
{
    memcpy(hw_mem + paddr, &data, len);
    uint32_t tag = (paddr >> 13) & 0x4fff;
    uint32_t group_num = (paddr >> 6) & 0x7f;
    uint32_t offset = paddr & 0x3f;
    int i;
    for (i = 0; i < 8; i++) {
        if ((cache[group_num * 8 + i].tag = tag) && (cache[group_num * 8 + i].valid == 1)) {
            if (offset + len <= 64)
                memcpy(cache[group_num * 8 + i].block, &data, len);
            else {
                cache_write(paddr, 64 - offset, data, cache);
				cache_write(paddr + 64 - offset, len + offset - 64, data >> (8 * (64 - offset)), cache);
            }
            break;
        }
    }
}


