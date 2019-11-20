#include "test_ptrace.h"

void start_bash(std::string bash) {
    char *c_bash = new char[bash.length() + 1];
    strcpy(c_bash, bash.c_str());
    char* const child_args[] = { c_bash, NULL };
    execvp(child_args[0], child_args);
    delete []c_bash;
}

void setRlimit(int pre_time_limit, int pre_memery_limit) {
    // set limit
    struct rlimit rl;

    /**
    struct rlimit {
        rlim_t rlim_cur;
        rlim_t rlim_max;
    }
    */

    // CPU 时间最大量值(秒),当超过此软限制，向该进程发生SIGXCPU
    rl.rlim_cur = pre_time_limit / 1000 + 1;
    if (pre_time_limit % 1000 > 800) {
        rl.rlim_cur += 1;
    }
    rl.rlim_max = rl.rlim_cur + 1;
    if (setrlimit(RLIMIT_CPU, &rl)) {
        DEBUG_LOG("set RLIMIT_CPU failure");
        return;
    }

    // 数据段的最大字节长度。初始化数据，非初始化数据以及堆的总和
    rl.rlim_cur = pre_memery_limit * 1024;
    rl.rlim_max = rl.rlim_cur + 1024;
    if (setrlimit(RLIMIT_DATA, &rl)) {
        DEBUG_LOG("set RLIMIT_DATA failure");
        return;
    }

    // 进程总共的可用存储空间的最大长度(字节)，影响到sbrk函数和mmap函数
    rl.rlim_cur = pre_memery_limit * 1024 * 2;
    rl.rlim_max = rl.rlim_cur + 1024;
    if (setrlimit(RLIMIT_AS, &rl)) {
        DEBUG_LOG("set RLIMIT_AS failure");
        return;
    }

    // 栈的最大字节长度
    rl.rlim_cur = 256 * 1024 * 1024;
    rl.rlim_max = rl.rlim_cur + 1024;
    if (setrlimit(RLIMIT_STACK, &rl)) {
        DEBUG_LOG("set RLIMIT_STACK failure");
        return;
    }

    // 限制子进程数
    rl.rlim_cur = 0;
    rl.rlim_max = 0;
    if (setrlimit(RLIMIT_NPROC, &rl)) {
        DEBUG_LOG("set RLIMIT_STACK failure");
        return;
    }

    // 限制能打开的文件
    // 这个会导致程序打不开动态库，如bits/stdc++.h
    // rl.rlim_cur = 1;
    // rl.rlim_max = 1;
    // if (setrlimit(RLIMIT_NOFILE, &rl)) {
    //     DEBUG_LOG("set RLIMIT_STACK failure");
    //     return;
    // }
}

int test_without_syscall(const char *path)
{
    pid_t pid;
    
    int pre_time_limit   = 1000;           // ms
    int pre_memery_limit = 128 * 1024;     // kb

    pid = fork();
    if (pid == -1) {
        perror("fork() == -1");
        return -1;
    }
    if (pid == 0) { // child

        setRlimit(pre_time_limit, pre_memery_limit);

        freopen("test/data/1.in", "r", stdin);
        freopen("test/data/1.out", "w", stdout);
        start_bash(path);

    } else {        // father
        
        int status = 0;
        struct rusage ru;
        struct user_regs_struct regs;

        /*
        struct rusage {
            struct timeval ru_utime; // user time used 
            struct timeval ru_stime; // system time used 
            long ru_maxrss; // maximum resident set size 
            long ru_ixrss; // integral shared memory size
            long ru_idrss; // integral unshared data size 
            long ru_isrss; // integral unshared stack size 
            long ru_minflt; // page reclaims 
            long ru_majflt; // page faults 
            long ru_nswap;// swaps
            long ru_inblock; // block input operations 
            long ru_oublock; // block output operations 
            long ru_msgsnd; // messages sent 
            long ru_msgrcv; //messages received 
            long ru_nsignals; // signals received 
            long ru_nvcsw; // voluntary context switches 
            long ru_nivcsw; // involuntary context switches 
        };
        struct timeval {
            time_t tv_sec; // seconds 
            long tv_usec;  // microseconds 
        };
        */
        pid_t ret_pid = wait4(pid, &status, WSTOPPED, &ru);
        if (ret_pid == -1) {
            perror("wait4 error");
            return -1;
        }
        int time_used = ru.ru_utime.tv_sec * 1000
            + ru.ru_utime.tv_usec / 1000
            + ru.ru_stime.tv_sec * 1000
            + ru.ru_stime.tv_usec / 1000;
        int memery_used = ru.ru_maxrss;

        printf("times = %d ms. %d kb\n", time_used, memery_used);

        // 信号说明在 《UNIX环境高级编程》 251页
        if (WIFSIGNALED(status)) {
            int signum = WTERMSIG(status);
            switch (signum) {
                case SIGSEGV:   // 内存访问错误
                    printf("SIGSEGV\n");
                    if (memery_used > pre_memery_limit) {
                        // MLE
                        printf("MLE");
                    } else {
                        // RE
                        printf("RE");
                    }
                    break;
                case SIGALRM:   // 定时器超时
                    printf("SIGALRM TLE\n");
                case SIGXCPU:   // 超出CPU时间限制 setrlimit
                    printf("SIGXCPU TLE\n");
                    // TLE
                    break;
                default:
                    printf("default RE\n");
                    // RE
                    break;
            }
        } else {
            if (time_used > pre_time_limit) {
                // TLE
                printf("TLE");
            } else if (memery_used > pre_memery_limit) {
                // MLE
                printf("MLE");
            } else {
                // AC
            }
        }
    }
    return 0;
}

int test_with_syscall(const char *path)
{
    pid_t pid;
    
    int pre_time_limit   = 1000;           // ms
    int pre_memery_limit = 128 * 1024;     // kb

    pid = fork();
    if (pid == -1) {
        DEBUG_LOG("fork() == -1");
        return -1;
    }
    if (pid == 0) { // child

        setRlimit(pre_time_limit, pre_memery_limit);

        freopen("test/data/1.in", "r", stdin);
        freopen("test/data/1.out", "w", stdout);
        
        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1) {
            DEBUG_LOG("TRACEME failure");
            return -1;
        }
        
        start_bash(path);

    } else {        // father

        int status = 0;
        int incall = 0;
        struct rusage ru;
        struct user_regs_struct regs;

        while (true) {

            pid_t ret_pid = wait4(pid, &status, WSTOPPED, &ru);
            if (ret_pid == -1) {
                DEBUG_LOG("wait4() fail");
                return -1;
            }
            if (WIFEXITED(status)) {    // 正常退出
                DEBUG_LOG("success exit");
                break;
            }
            if (WSTOPSIG(status) != SIGTRAP) { // 不是由于trap导致在结束
                DEBUG_LOG("WSTOPSIG(status) != SIGTRAP");
                ptrace(PTRACE_KILL, pid, NULL, NULL);
                waitpid(pid, NULL, 0);

                // int time_used = ru.ru_utime.tv_sec * 1000
                //     + ru.ru_utime.tv_usec / 1000
                //     + ru.ru_stime.tv_sec * 1000
                //     + ru.ru_stime.tv_usec / 1000;
                // int memery_used = ru.ru_maxrss;
                // int signum = WSTOPSIG(status);
                // switch (signum) {
                //     case SIGSEGV:
                //         if (memery_used > pre_memery_limit) {
                //             // MLE
                //         } else {
                //             // RE
                //         }
                //         break;
                //     case SIGALRM:
                //     case SIGXCPU:
                //         // TLE
                //         break;
                //     default:
                //         // RE
                //         break;
                // }
                return 0;
            }

            if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) == -1) {
                DEBUG_LOG("PTRACE_GETREGS failure");
                return -1;
            }

            if (incall) {
                DEBUG_LOG("in call = 1");

                int ret = REG_SYS_CALL(&regs);
                DEBUG_LOG("sys call = %d", ret);

                // if (ret == SYS_) {
                //     // 先默认搞死所有系统调用
                    // ptrace(PTRACE_KILL, pid, NULL, NULL);
                    // waitpid(pid, NULL, 0);
                // }

                incall = 0;
            } else {
                DEBUG_LOG("in call = 0");
                
                incall = 1;
            }
            ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
        }
    }
}
