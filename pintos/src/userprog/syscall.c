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

void
halt (void) 
{
  shutdown_power_off();
}

void
exit (int status)
{
  struct thread *t=thread_current();
  printf("%s: exit(%d)\n", thread_name(), status);
  t->exit_flag=status;
  thread_exit();
}

pid_t
exec (const char *file)
{
  tid_t tid=process_execute(file);
  struct thread* t=get_child(tid);

  if(tid!=NULL){
    sema_down(&(t->sema_load));
    if(t->is_load==false) return -1;
    else return tid;
  }
  return -1;
}

int
wait (pid_t pid)
{
  process_wait(pid);
}

bool
create (const char *file, unsigned initial_size)
{
  if (file==NULL) exit(-1);
  return filesys_create(file, initial_size);
}

bool
remove (const char *file)
{
  if (file==NULL) exit(-1);
  return filesys_remove(file);
}

int
open (const char *file)
{
  struct file* f;
  int fd;
  if (file==NULL) exit(-1);
  
  lock_acquire(&file_lock);
  f=filesys_open(file);
  if (f==NULL){
    lock_release(&file_lock);
    return -1;
  }
  fd=create_file(f);
  lock_release(&file_lock);
  return fd;
}

int
filesize (int fd) 
{
  struct file* f=get_file(fd);
  if(f==NULL) exit(-1);
  return file_length(f);
}

int
read (int fd, void *buffer, unsigned size)
{
  int res;
  uint8_t tmp;
  if (fd==1 || fd<0 || fd>=130) exit(-1);
  addr_check(buffer);
  lock_acquire(&file_lock);
  if(fd!=0){
    struct file* f=get_file(fd);
    if(f==NULL){
      lock_release(&file_lock);
      exit(-1);
    }
    res=file_read(f, buffer, size);
  }
  else{
    for (res=0;(tmp=input_getc())&&(res<size);res++){
      *(uint8_t*)(buffer+res)=tmp;
    }
  }
  lock_release(&file_lock);
  return res;
}

int
write (int fd, const void *buffer, unsigned size)
{
  int res;
  struct file* f;
  if (fd<=0 || fd>=130) exit(-1);
  addr_check;
  lock_acquire(&file_lock);
  if(fd!=1){
    f=get_file(fd);
    if(f==NULL){
      lock_release(&file_lock);
      exit(-1);
    }
    res=file_write(f, buffer, size);
    lock_release(&file_lock);
    return res;
  }
  else{
    putbuf(buffer, size);
    lock_release(&file_lock);
    return size;
  }
  lock_release(&file_lock);
  return res;
}

void
seek (int fd, unsigned position) 
{
  struct file* f=get_file(fd);
  if(f==NULL) exit(-1);
  file_seek(f, position);
}

unsigned
tell (int fd) 
{
  struct file* f=get_file(fd);
  if(f==NULL) exit(-1);
  return file_tell(f);
}

void
close (int fd)
{
  close_file(fd);
}

mapid_t
mmap (int fd, void *addr)
{
  return syscall2 (SYS_MMAP, fd, addr);
}

void
munmap (mapid_t mapid)
{
  syscall1 (SYS_MUNMAP, mapid);
}

bool
chdir (const char *dir)
{
  return syscall1 (SYS_CHDIR, dir);
}

bool
mkdir (const char *dir)
{
  return syscall1 (SYS_MKDIR, dir);
}

bool
readdir (int fd, char name[READDIR_MAX_LEN + 1]) 
{
  return syscall2 (SYS_READDIR, fd, name);
}

bool
isdir (int fd) 
{
  return syscall1 (SYS_ISDIR, fd);
}

int
inumber (int fd) 
{
  return syscall1 (SYS_INUMBER, fd);
}

