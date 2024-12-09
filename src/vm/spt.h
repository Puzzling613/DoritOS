#include <hash.h>
#include "filesys/off_t.h"

struct spt //page
{
	const void *user_vaddr,
    int page_location, // (file: 0/memory: 1/swap table:2)
    bool writable, 
    struct file *file,
    struct frame *frame,
    off_t offset,
    size_t read_bytes,
    int swap_idx,
    struct hash_elem spt_hash_elem
};

void spt_init(struct spt * spt);
struct spt * spt_find(struct spt * spt, void * va);
bool spt_insert(struct spt * spt, struct spt *page);
bool spt_delete(struct spt * spt);
void spt_free(struct spt * spt);

/* Returns a hash value for page p. */
unsigned
page_hash (const struct hash_elem *p_, void *aux UNUSED)
{
    const struct spt *p = hash_entry (p_, struct spt, hash_elem);
    return hash_bytes (&p->addr, sizeof p->addr);
}
/* Returns true if page a precedes page b. */
bool
page_less (const struct hash_elem *a_, const struct hash_elem *b_,
void *aux UNUSED)
{
    const struct spt *a = hash_entry (a_, struct spt, hash_elem);
    const struct spt *b = hash_entry (b_, struct spt, hash_elem);
    return a->addr < b->addr;
}
bool 
page_insert(struct hash *h, struct spt *p) 
{
    if(!hash_insert(h, &p->hash_elem)) return true;
	else return false;
}
bool 
page_delete(struct hash *h, struct spt *p) 
{
	if(!hash_delete(h, &p->hash_elem)) return true;
	else return false;
}
