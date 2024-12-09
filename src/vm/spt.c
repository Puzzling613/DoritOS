#include "spt.h"
#include "threads/vaddr.h"

void
spt_init(struct spt * spt)
{
    hash_init(&spt->spt_hash, page_hash, page_less, NULL);
}

struct spt * 
spt_find(struct spt * spt, void * va)
{
    struct spt * page = (struct spt *)malloc(sizeof(struct spt));
    struct hash_elem *e;

    page->va = pg_round_down(va); //va 시작 주소 page
	e = hash_find(&spt->spt_hash, &page->hash_elem); //spt에서 key값과 일치하는 entry
    
	free(page);

	if (e != NULL) return e
    else return NULL;
}

bool spt_insert(struct spt * spt, struct spt *page);
bool spt_delete(struct spt * spt);
void spt_free(struct spt * spt);