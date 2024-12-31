/*
 * contains the implementation of all syscalls.
 */

#include <stdint.h>
#include <errno.h>

#include "util/types.h"
#include "syscall.h"
#include "string.h"
#include "process.h"
#include "util/functions.h"
#include "elf.h"


#include "spike_interface/spike_utils.h"

extern elf_ctx elfloader;
//
// implement the SYS_user_print syscall
//
ssize_t sys_user_print(const char* buf, size_t n) {
  sprint(buf);
  return 0;
}

//
// implement the SYS_user_exit syscall
//
ssize_t sys_user_exit(uint64 code) {
  sprint("User exit with code:%d.\n", code);
  // in lab1, PKE considers only one app (one process). 
  // therefore, shutdown the system when the app calls exit()
  shutdown(code);
}
ssize_t sys_user_backtrace(uint64 n) {
  riscv_regs regs =  current->trapframe->regs;
  uint64 ra = regs.ra;
  uint64 fp = regs.s0;
  uint64 sp = regs.sp;
  uint64 p = sp + 40;
  //打印用户栈100字节
  //for(int i = 0; i < 100; i++) {
  //  sprint("0x%lx: 0x%lx\n", sp, *((uint64*)sp));
  //  sp += 8;
  //}
  for(int i = 0; i < n; i++) {
    //为上一层函数返回值
    for(int i = 0; i < elfloader.symtab_num; i++) {
      elf_sym sym = elfloader.symtab[i];
      if (*(uint64*)p >= sym.value && *(uint64*)p < sym.value + sym.size) {
        sprint("%s\n",elfloader.strtab + sym.name);
        break;
      }
    } 
    
    //打印用户栈
    //sprint("0x%lx: ra\n", ra);
    //sprint("0x%lx: fp\n", fp);
    //下一层函数
    if (p == 0) break;
    p += 16;
    //ra = *((uint64*)fp + 1);
    //fp = *(uint64*)fp;
  }
  return 0;
}

//
// [a0]: the syscall number; [a1] ... [a7]: arguments to the syscalls.
// returns the code of success, (e.g., 0 means success, fail for otherwise)
//
long do_syscall(long a0, long a1, long a2, long a3, long a4, long a5, long a6, long a7) {
  switch (a0) {
    case SYS_user_print:
      return sys_user_print((const char*)a1, a2);
    case SYS_user_exit:
      return sys_user_exit(a1);
    case SYS_user_backtrace:
      return sys_user_backtrace(a1);
    default:
      panic("Unknown syscall %ld \n", a0);
  }
}
