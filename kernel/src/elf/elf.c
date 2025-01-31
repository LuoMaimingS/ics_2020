#include "common.h"
#include "memory.h"
#include "string.h"

#include <elf.h>
#include <string.h>

#ifdef HAS_DEVICE_IDE
#define ELF_OFFSET_IN_DISK 0
#endif

#define STACK_SIZE (1 << 20)

void ide_read(uint8_t *, uint32_t, uint32_t);
void create_video_mapping();
uint32_t get_ucr3();

uint32_t loader()
{
	Elf32_Ehdr *elf;
	Elf32_Phdr *ph, *eph;

#ifdef HAS_DEVICE_IDE
	uint8_t buf[4096];
	ide_read(buf, ELF_OFFSET_IN_DISK, 4096);
	elf = (void *)buf;
	Log("ELF loading from hard disk.");
#else
	elf = (void *)0x0;
	Log("ELF loading from ram disk.");
#endif

	/* Load each program segment */
	ph = (void *)elf + elf->e_phoff;
	eph = ph + elf->e_phnum;
	for (; ph < eph; ph++)
	{
		if (ph->p_type == PT_LOAD)
		{

        // Log("type: %x,  offset: %x, vaddr: %x, paddr: %x, file size: %x, mem size: %x.", ph->p_type, ph->p_offset, ph->p_vaddr, ph->p_paddr, ph->p_filesz, ph->p_memsz);

#ifndef IA32_PAGE
            /* copy the segment from the ELF file to its proper memory area */
            if (ph->p_filesz != 0) {
                // Log("copy: %x -> %x,  len:%x", ph->p_offset, ph->p_paddr, ph->p_filesz);
                memcpy((void*)ph->p_paddr, (void*)ph->p_offset, ph->p_filesz);
                
                /* zeror the memory area [vaddr + file_sz, vaddr + mem_sz) */
			    if(ph->p_memsz > ph->p_filesz) {
                    // Log("clear from %x to %x", ph->p_filesz, ph->p_memsz);
			        memset((void*)ph->p_paddr + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
                    // Log("Clear.")
                }
            }
#else
            if (ph->p_filesz != 0) {
                uint32_t paddr = mm_malloc(ph->p_vaddr, ph->p_memsz);
#ifndef HAS_DEVICE_IDE        
                memcpy((void *)paddr, (void*)ph->p_offset, ph->p_filesz);
#else
                ide_read((uint8_t *)paddr, ph->p_offset, ph->p_filesz);
#endif
                // Log("off:%x  copy: %x %x -> %x,  len:%x", ph->p_offset, ph->p_vaddr, ph->p_paddr, paddr, ph->p_filesz);

                if(ph->p_memsz > ph->p_filesz) {
			        memset((void*)paddr + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
                    // Log("clear from %x to %x", ph->p_filesz, ph->p_memsz);
                }
            }

			/* Record the program break for future use */
			extern uint32_t brk;
			uint32_t new_brk = ph->p_vaddr + ph->p_memsz - 1;
			if (brk < new_brk)
			{
				brk = new_brk;
			}
#endif
		}
	}

	volatile uint32_t entry = elf->e_entry;

#ifdef IA32_PAGE
	mm_malloc(KOFFSET - STACK_SIZE, STACK_SIZE);
#ifdef HAS_DEVICE_VGA
	create_video_mapping();
#endif
	write_cr3(get_ucr3());
#endif
	return entry;
}
