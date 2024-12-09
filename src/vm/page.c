#include "page.h"
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

    spt->user_vaddr = page->user_vaddr;
    //...

    return page_insert(&thread_current()->pagetable, spt);
}
bool 
spt_delete(struct spt * spt, void *va)
{
    struct spt *temp = spt_find(spt,va);

    if (e != NULL) {
        page_delete(&thread_current()->pagetable,temp);
        return true;
    }
    return false;
}
void
spt_free(struct spt * spt) 
{
    struct hash_iterator i;
    struct hash_elem *e;

    hash_first(&i, spt);
    while (hash_next(&i)) {
        e = hash_cur(&i);
        struct spt *entry = hash_entry(e, struct spt, spt_hash_elem);
        hash_delete(spt, e);
        free(entry);
    }
}