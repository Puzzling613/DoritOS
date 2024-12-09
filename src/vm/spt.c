#include "spt.h"


void
spt_init(struct spt * spt)
{
    hash_init(&spt->spt_hash, page_hash, page_less, NULL);
}
struct spt * spt_find(struct spt * spt, void * va);
bool spt_insert(struct spt * spt, struct spt *page);
bool spt_delete(struct spt * spt);
void spt_free(struct spt * spt);