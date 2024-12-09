#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <hash.h>
#include "filesys/file.h"
#include "filesys/off_t.h"

struct SPT //page
{
	void *user_vaddr;
    int page_location; // (file: 0/memory: 1/swap table:2)
    bool writable;
    struct file *file;
    off_t offset;
    size_t read_bytes;
    bool is_loaded;
    struct hash_elem spt_hash_elem;
};

void spt_init(struct hash * spt);
struct SPT * spt_find(struct SPT * spt, void * va);
bool spt_insert(struct SPT *page, struct file *f, off_t offset, size_t read_bytes, bool writable);
bool spt_delete(struct SPT * spt, void *va);
void spt_free(struct SPT * spt);

unsigned page_hash (const struct hash_elem *p_, void *aux UNUSED);
bool page_less (const struct hash_elem *a_, const struct hash_elem *b_, void *aux UNUSED);
bool page_insert(struct hash *h, struct SPT *p);
bool page_delete(struct hash *h, struct SPT *p);

#endif