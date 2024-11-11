#include <syscall.h>
#include "../syscall-nr.h"

/* Invokes syscall NUMBER, passing no arguments, and returns the
   return value as an `int'. */
#define syscall0(NUMBER)                                        \
        ({                                                      \
          int retval;                                           \
          asm volatile                                          \
            ("pushl %[number]; int $0x30; addl $4, %%esp"       \
               : "=a" (retval)                                  \
               : [number] "i" (NUMBER)                          \
               : "memory");                                     \
          retval;                                               \
        })

/* Invokes syscall NUMBER, passing argument ARG0, and returns the
   return value as an `int'. */
#define syscall1(NUMBER, ARG0)                                           \
        ({                                                               \
          int retval;                                                    \
          asm volatile                                                   \
            ("pushl %[arg0]; pushl %[number]; int $0x30; addl $8, %%esp" \
               : "=a" (retval)                                           \
               : [number] "i" (NUMBER),                                  \
                 [arg0] "g" (ARG0)                                       \
               : "memory");                                              \
          retval;                                                        \
        })

/* Invokes syscall NUMBER, passing arguments ARG0 and ARG1, and
   returns the return value as an `int'. */
#define syscall2(NUMBER, ARG0, ARG1)                            \
        ({                                                      \
          int retval;                                           \
          asm volatile                                          \
            ("pushl %[arg1]; pushl %[arg0]; "                   \
             "pushl %[number]; int $0x30; addl $12, %%esp"      \
               : "=a" (retval)                                  \
               : [number] "i" (NUMBER),                         \
                 [arg0] "r" (ARG0),                             \
                 [arg1] "r" (ARG1)                              \
               : "memory");                                     \
          retval;                                               \
        })

/* Invokes syscall NUMBER, passing arguments ARG0, ARG1, and
   ARG2, and returns the return value as an `int'. */
#define syscall3(NUMBER, ARG0, ARG1, ARG2)                      \
        ({                                                      \
          int retval;                                           \
          asm volatile                                          \
            ("pushl %[arg2]; pushl %[arg1]; pushl %[arg0]; "    \
             "pushl %[number]; int $0x30; addl $16, %%esp"      \
               : "=a" (retval)                                  \
               : [number] "i" (NUMBER),                         \
                 [arg0] "r" (ARG0),                             \
                 [arg1] "r" (ARG1),                             \
                 [arg2] "r" (ARG2)                              \
               : "memory");                                     \
          retval;                                               \
        })

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
