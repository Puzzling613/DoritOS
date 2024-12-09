#include "frame.h"
#include "list.h"
#include "fliesys/file.h"
#include "threads/palloc.h"
#include "threads/malloc.h"
#include "userprog/pagedir.h"
#include "userprog/syscall.h"
#include "swap.h"
#include "page.h"

struct lock frame_lock;
struct list f;
static int cnt;

//초기화
void init_frame(void){
    list_init(&f);
    lock_init(&frame_lock);
}

//page allocator func 호출, **User Pool**
void get_page (void *user_page){
    void *kernel_page = palloc_get_page(PAL_USER);
    cnt++;

    //frame table에 user page 정보 입력
    struct frame *fr = (struct frame*)malloc(sizeof(struct frame));
    fr->page_addr = user_page;
    fr->whose_frame = thread_current();

    //no page available
    while (!kernel_page){
        if (!lock_held_by_current_thread(&frame_lock)){
            lock_acquire(&frame_lock);
        }
        kernel_page = evict_page();
        if(lock_held_by_current_thread(&frame_lock)){
            lock_release(&frame_lock);
        }

        fr->virtual_addr = kernel_page;

        if (!lock_held_by_current_thread(&frame_lock)){
            lock_acquire(&frame_lock);
        }
        list_push_back(&f, &fr->frame_e);
        if(lock_held_by_current_thread(&frame_lock)){
            lock_release(&frame_lock);
        }
        return kernel_page;
    }
}

void free_page (void *kernel_addr){
    if(!lock_held_by_current_thread(&frame_lock)){
        lock_acquire(&frame_lock);
    }
    struct list_elem *cur = list_head(&f)->next;
    while(cur->next){
        struct frame *fr;
        fr = list_entry(cur, struct struct frame, frame_e);
        cur = cur->next;
        if (kernel_addr == fr->page_addr){
            palloc_free_page(kernel_addr);
            list_remove(&fr->frame_e);
            free(fr);
            break;
        }
    }
    if (lock_held_by_current_thread(&frame_lock)){
        lock_release(&frame_lock);
    }
}

void free_frame (void *user_page, bool is_loaded){
    if (is_loaded){
        struct thread *t;
        t = thread_current();
        void *kernel_page;
        kernel_page = pagedir_get_page(t->pagedir, user_page);
        free_page(kernel_page);
        pagedir_clear_page(t->pagedir, user_page);
    }
}

void evict_frame(void){
    struct list_elem *last;
    struct frame *fr;
    struct SPT *spt;
    last = list_begin(&f);
    while (1){
        if (last == list_end(&f)){
            last = list_begin(&f);
        }
        fr = list_entry(last, struct frame, frame_e);
        spt = spt_find(fr->page_addr);
        last = list_next(last);
        bool is_accessed = pagedir_is_accessed(fr->whose_frame->pagedir, fr->page_addr);
        bool is_dirty = pagedir_is_dirty(fr->whose_frame->pagedir, fr->page_addr);
        if(!is_accessed){
            // if(spt->isMmaped){
            //     if (is_dirty){
            //         if (!lock_held_by_current_thread(&sync)){
            //             lock_acquire(&sync);
            //         }
            //         file_write_at(spt->file, spt->page_location, spt->re_bytes, spt->offset);
            //         if (lock_held_by_current_thread(&sync)){
            //             lock_release(&sync);
            //         }
            //     }
            //     else{
            //         spt->is_swapped = true;
            //         spt->swap_idx = swap_out(fr->virtual_addr);
            //     }
            //     //page not loaded
            //     spt->is_loaded = false;
            //     pagedir_clear_page(fr->whose_frame->pagedir, spt->page_location);
            //     palloc_free_page(fr->virtual_addr);
            //     list_remove(&fr->frame_e);
            //     free(fr);

            //     void *kernel_page = palloc_get_page(PAL_USER);
            //     return kernel_page;
            // }
            // else{
                pagedir_set_accessed(fr->whose_frame->pagedir, fr->page_addr, false);
            // }
        }
    }
}

struct frame* find_kernel_page (void *kernel_addr){
    struct list_elem *cur;
    cur = list_head(&f)->next;
    while(cur->next){
        struct frame *fr = list_entry(cur, struct frame, frame_e)
        cur = cur->next;
        if (kernel_addr == fr->page_addr){
            list_remove(&fr->frame_e)
            list_push_back(&f, &fr->frame_e);
            return fr;
        }
    }
    return NULL;
}

struct frame* find_user_page (void *user_addr){
    struct list_elem *cur;
    cur = list_head(&f)->next;
    while(cur->next){
        struct frame *fr = list_entry(cur, struct frame, frame_e)
        cur = cur->next;
        if (user_addr == fr->page_addr){
            return fr;
        }
    }
    return NULL;
}

struct frame* last_frame(void){
    struct list_elem *last;
    last = list_end(&f)->prev;
    while (last!=list_begin(&f)){
        struct frame *fr;
        struct SPT *spt = spt_find(fr->page_addr);
        last = list_prev(last);
        bool is_accessed = pagedir_is_accessed(fr->whose_frame->pagedir, fr->page_addr);
        bool is_dirty = pagedir_is_dirty(fr->whose_frame->pagedir, fr->page_addr);
        if(!is_accessed){
            return fr;
        }
        else{
            pagedir_set_accessed(fr->whose_frame->pagedir, fr->page_addr, false);
        }
        struct list_elem *random_frame;
        random_frame = list_end(&f)->prev;
        struct frame *frm = list_entry(random_frame, struct frame, frame_e);

        return fr;
    }
}