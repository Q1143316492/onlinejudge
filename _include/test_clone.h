#ifndef _TEST_CLONE_H_
#define _TEST_CLONE_H_

#include <sys/wait.h>   // waitpid
#include <sys/mount.h>  // mount
#include <fcntl.h>      // open
#include <unistd.h>     // execv, sethostname, chroot, fchdir
#include <sched.h>      // clone
#include <bits/stdc++.h>

#include <dirent.h>

#include <sys/ptrace.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/user.h>

#include "server_logs.h"
#include "server_macro.h"

using namespace std;

namespace docker {

#define STACK_SIZE (512 * 512) // 定义子进程空间大小

const int PROC_ERR = -1;
const int PROC_EXIT = 0;
const int PROC_WAIT = 1;

class Container {
public:
    Container();

    void start();

    void start_bash();

    void set_hostname();
    void set_rootdir();
    void set_procsys();

    void setRlimit(int pre_time_limit, int pre_memery_limit);
private:

    string host_name;
    string root_dir;

    char child_stack[STACK_SIZE];

    char buf[128];
    int fds[2];
};


void test_clone();

}

#endif