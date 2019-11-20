#ifndef _TEST_CLONE_H_
#define _TEST_CLONE_H_

#include "server_logs.h"

#include <sys/wait.h>   // waitpid
#include <sys/mount.h>  // mount
#include <fcntl.h>      // open
#include <unistd.h>     // execv, sethostname, chroot, fchdir
#include <sched.h>      // clone
#include <bits/stdc++.h>

#include <dirent.h>

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
private:

    string host_name;
    string root_dir;

    char child_stack[STACK_SIZE];
};


void test_clone();

}

#endif