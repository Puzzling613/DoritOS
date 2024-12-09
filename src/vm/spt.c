#include "spt.h"
#include "threads/vaddr.h"

void
spt_init(struct hash * spt)
{
    hash_init(spt, page_hash, page_less, NULL);
}

struct spt * 
spt_find(struct spt * spt, void * va)
{
    struct spt spt;
    struct thread *t = thread_current();
    struct hash_elem *e;

    spt.user_vaddr = pg_round_down(va); //va 시작 주소 page
	e = hash_find(&t->ptable, &spt.spt_hash_elem); //spt에서 key값과 일치하는 entry

	if (e != NULL) return hash_entry(e, struct spt, spt_hash_elem);

    else return NULL;
}

bool 
spt_insert(struct spt * spt, struct spt *page) 
{
    /*새 spt 구조체를 할당한다. member 정보와 file 정보를 저장한다. 생성된 spt entry를 spt 내 hash table에 삽입한다.*/
    struct spt * spt = (struct spt *)malloc(sizeof(struct spt));
    struct thread *t = thread_current();

    spt->user_vaddr = page->user_vaddr;
    //...
    return page_insert(&t->pagetable, spt);

}
bool spt_delete(struct spt * spt);
void spt_free(struct spt * spt);