#include "test_clone.h"

namespace docker {

const string pwd = "/home/weilinchen/workspace/sandbox/ojsandbox/";

void test_clone()
{
    cout << "test clone begin..." << endl;

    Container container;
    container.start();
    
    cout << "test clone end..." << endl;
}

Container::Container()
{
    this->host_name = "cwl_server";
    this->root_dir  = pwd + string("sandbox");
}

void Container::setRlimit(int pre_time_limit, int pre_memery_limit) {
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
    // rl.rlim_cur = 0;
    // rl.rlim_max = 0;
    // if (setrlimit(RLIMIT_NPROC, &rl)) {
    //     DEBUG_LOG("set RLIMIT_STACK failure");
    //     return;
    // }
    puts("===");
}

void father_process(int child_pid)
{
    int pre_time_limit   = 1000;           // ms
    int pre_memery_limit = 128 * 1024;     // kb
    int status = 0;
    struct rusage ru;
    struct user_regs_struct regs;
    // test time
    pid_t ret_pid = wait4(child_pid, &status, WSTOPPED, &ru);
    if (ret_pid == -1) {
        perror("wait4 error");
        return ;
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
                    printf("MLE\n");
                } else {
                    // RE
                    printf("RE\n");
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
            printf("TLE\n");
        } else if (memery_used > pre_memery_limit) {
            // MLE
            printf("MLE\n");
        } else {
            // AC
            printf("AC\n");
        }
    }
}

void Container::start()
{
    if (pipe(this->fds) == -1) {
        perror("pipe error");
        return;
    }
    auto setup = [](void *args) -> int {
        auto _this = reinterpret_cast<Container *>(args);

        _this->set_hostname();
        _this->set_rootdir(); 
        _this->set_procsys();
        
        close(_this->fds[0]);

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork() error:");
            return PROC_WAIT;
        }
        if (pid == 0) {
            // child
            int pre_time_limit   = 1000;           // ms
            int pre_memery_limit = 128 * 1024;     // kb
            _this->setRlimit(pre_time_limit, pre_memery_limit);

            // 降低权限
            setuid(10000);
            
            // 读取文件
            freopen("1.in", "r", stdin);
            freopen("1.out", "w", stdout);
            _this->start_bash();
            
        } else {
            // father
            father_process(pid);
            
            close(_this->fds[0]);
            write(_this->fds[1], "hello", 5);
        }

        return PROC_WAIT;
    };

    pid_t child_pid = clone(setup, child_stack + STACK_SIZE,
                        CLONE_NEWUTS| // 添加 UTS namespace, 确定hostname修改会影响父进程
                        CLONE_NEWNS|  // Mount namespace
                        CLONE_NEWPID| // PID   namespace
                        CLONE_NEWNET| // Net   namespace
                        SIGCHLD,
                        this);
    
    close(this->fds[1]);
    memset(this->buf, 0, sizeof(this->buf));
    read(this->fds[0], this->buf, 128);
    printf("pipe [%s]\n", this->buf);

    waitpid(child_pid, nullptr, 0);

    

}

void Container::start_bash() 
{
    // std::string bash = "/bin/bash";
    std::string bash = "/main";
    char *c_bash = new char[bash.length() + 1];
    strcpy(c_bash, bash.c_str());
    char* const child_args[] = { c_bash, NULL };
    int ret = execv(child_args[0], child_args);
    printf("ret = %d\n", ret);
    perror("exec err:");
    delete []c_bash;
}

void Container::set_hostname() 
{
    sethostname(host_name.c_str(), host_name.length());
}

void Container::set_rootdir() 
{
    chdir(root_dir.c_str());
    chroot(".");
}

void Container::set_procsys() 
{
    // mount("none", "/proc", "proc", 0, nullptr);
    // mount("none", "/sys", "sysfs", 0, nullptr);
}

}