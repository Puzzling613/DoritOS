#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  lock_init(&file_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  switch(*(int32_t*)(f->esp)){
    case SYS_HALT:
      halt();
      break;
    case SYS_EXIT:
      addr_check(f->esp+4);
      exit(*(int*)(f->esp+4));
      break;
    case SYS_EXEC:
      addr_check(f->esp+4);
      f->eax=exec((char*)*(uint32_t*)(f->esp+4));
      break;
    case SYS_WAIT:
      addr_check(f->esp+4);
      f->eax=wait(*(uint32_t*)(f->esp+4));
      break;
    case SYS_CREATE:
      addr_check(f->esp+4);
      addr_check(f->esp+8);
      f->eax = create((char*)*(uint32_t*)(f->esp+4), *(uint32_t*)(f->esp+8));
      break;
    case SYS_REMOVE:
      addr_check(f->esp+4);
      f->eax = remove((char*)*(uint32_t*)(f->esp+4));
      break;
    case SYS_OPEN:
      addr_check(f->esp+4);
      f->eax = open((char*)*(uint32_t*)(f->esp+4));
      break;
    case SYS_FILESIZE:
      addr_check(f->esp+4);
      f->eax = filesize(*(uint32_t*)(f->esp+4));
      break;
    case SYS_READ:
      addr_check(f->esp+4);
      addr_check(f->esp+8);
      addr_check(f->esp+12);
      f->eax = read((int)*(uint32_t*)(f->esp+4), (void*)*(uint32_t*)(f->esp+8),
					(unsigned)*(uint32_t*)(f->esp+12));
      break;
    case SYS_WRITE:
      addr_check(f->esp+4);
      addr_check(f->esp+8);
      addr_check(f->esp+12);
      f->eax = read((int)*(uint32_t*)(f->esp+4), (const void*)*(uint32_t*)(f->esp+8),
					(unsigned)*(uint32_t*)(f->esp+12));
      break;
    case SYS_SEEK:
      addr_check(f->esp+4);
      addr_check(f->esp+8);
      seek((int)*(uint32_t*)(f->esp+4), (unsigned)*(uint32_t*)(f->esp+8));
      break;
    case SYS_TELL:
      addr_check(f->esp+4);
      f->eax = tell((int)*(uint32_t*)(f->esp+4));
      break;
    case SYS_CLOSE:
      addr_check(f->esp+4);
      close(*(uint32_t*)(f->esp+4));
      break;
    default:
      exit(-1);
      break;
  }
}
