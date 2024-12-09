#include "threads/thread.h"

struct frame_table{
    struct list_elem frame_e;
    struct thread *whose_frame;
    void *page_addr;
    void *virtual_addr;
};

void init_frame(void);
void get_page (void *user_page);
void free_page (void *kernel_addr);
void free_frame (void *user_page, bool isLoad);
void evict_frame(void);
struct frame* find_kernel_page (void* kernel_addr);
struct frame* find_user_page (void *user_addr);
struct frame* last_frame(void);
