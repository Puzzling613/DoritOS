#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <hash.h>
#include "filesys/file.h"
#include "filesys/off_t.h"

struct spt //page
{
	void *user_vaddr;
    int page_location; // (file: 0/memory: 1/swap table:2)
    bool writable;
    struct file *file;
    struct frame *frame;
    off_t offset;
    size_t read_bytes;
    struct hash_elem spt_hash_elem;
};

void spt_init(struct hash * spt);
struct spt * spt_find(struct spt * spt, void * va);
bool spt_insert(struct spt * spt, struct spt *page);
bool spt_delete(struct spt * spt, void *va);
void spt_free(struct spt * spt);

unsigned page_hash (const struct hash_elem *p_, void *aux UNUSED);
bool page_less (const struct hash_elem *a_, const struct hash_elem *b_, void *aux UNUSED);
bool page_insert(struct hash *h, struct spt *p);
bool page_delete(struct hash *h, struct spt *p);

#endif