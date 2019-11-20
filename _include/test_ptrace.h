#ifndef _TEST_PTRACE_H_
#define _TEST_PTRACE_H_

#include <bits/stdc++.h>

#include "server_logs.h"
#include "server_macro.h"
#include <sys/ptrace.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

#if __WORDSIZE == 64
    #define REG_SYS_CALL(x) ((x)->orig_rax)
    #define REG_ARG_1(x) ((x)->rdi)
    #define REG_ARG_2(x) ((x)->rsi)
#else
    #define REG_SYS_CALL(x) ((x)->orig_eax)
    #define REG_ARG_1(x) ((x)->ebx)
    #define REG_ARG_2(x) ((x)->ecx)
#endif

int test_without_syscall(const char *path);

int test_with_syscall(const char *path);

int test_clone();

#endif
