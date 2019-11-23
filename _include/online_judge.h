#ifndef _ONLINE_JUDGE_H_
#define _ONLINE_JUDGE_H_

#include <sys/wait.h>   // waitpid
#include <sys/mount.h>  // mount
#include <fcntl.h>      // open
#include <unistd.h>     // execv, sethostname, chroot, fchdir
#include <sched.h>      // clone

#include <dirent.h>

#include <sys/ptrace.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/user.h>

#include <bits/stdc++.h>

#include "server_logs.h"
#include "server_tools.h"
#include "server_macro.h"

using namespace std;

class OnlineJudge {
public:
    OnlineJudge();
    virtual ~OnlineJudge();

    void prepare_run();
    void start_run();
    void finish_run();

    void run_default_action();

    void set_time_limit(int iTimeLimit);
    void set_memery_limit(int iMemeryLimit);
    void set_sandbox_absolute_path(const string &path);
    void set_callback(const string &ip, const string &port);
    void set_ext_param(const string &strExtParam);
    void set_judge_id(const string &id);
    void set_judging(const string &actid, const string &files);

    void set_hostname();
    void set_rootdir();
    void set_procsys();

    int set_rlimit();
    int set_lower_power();
    int set_freopen(string in, string out);
    int start_main(string bash);
    int compare_answer(string &msg);
    int compile_cpp(string file);

    string wait_result(pid_t pid);
    string pack_result(string ret, int time, int memery, string msg);
    string pack_result(string ret, string time, string memery, string msg);

    void print_judge_msg();

private:

    int32_t                 m_time_limit;
    int32_t                 m_memery_limit;
    int32_t                 m_stack_limit;      // kb 
    int32_t                 m_output_limit;     // kb

    string                  m_sandbox_absolute_path;
    string                  m_callback_ip;
    string                  m_callback_port;
    string                  m_str_ext_param;
    string                  m_result_msg;
    string                  m_hostname;
    string                  m_exec_file;

    string                  m_judge_id;
    string                  m_act_id;
    string                  m_str_judge_file;
    string                  m_judge_result;
    string                  m_compile_error;

    map<string, string>     m_map_ext_param;
    
    char        *m_child_stack;
    int32_t     m_pipe_fds[2];

    bool        m_is_stop;
};

#endif