#ifndef _ONLINE_JUDGE_H_
#define _ONLINE_JUDGE_H_

#include <bits/stdc++.h>

using namespace std;

class OnlineJudge {
public:


    void set_time_limit(int iTimeLimit);
    void set_memery_limit(int iMemeryLimit);
    void set_sandbox_absolute_path(string path);
    void set_callback(string ip, string port);
    void set_ext_param(string strExtParam);

    void print_judge_msg();

private:
    int32_t     m_time_limit;
    int32_t     m_memery_limit;
    string      m_sandbox_absolute_path;
    string      m_callback_ip;
    string      m_callback_port;
    string      m_ext_param;
};

#endif