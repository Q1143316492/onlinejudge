#include "online_judge.h"

void OnlineJudge::print_judge_msg()
{
    cout << "time_limit: "            << m_time_limit << endl;
    cout << "memery_limit: "          << m_memery_limit << endl;
    cout << "sandbox_absolute_path: " << m_sandbox_absolute_path << endl;
    cout << "callback_ip: "           << m_callback_ip << endl;
    cout << "callback_port: "         << m_callback_port << endl;
    cout << "ext_param: "             << m_ext_param << endl;
}

void OnlineJudge::set_time_limit(int iTimeLimit)
{
    m_time_limit = iTimeLimit;
}

void OnlineJudge::set_memery_limit(int iMemeryLimit)
{
    m_memery_limit = iMemeryLimit;
}

void OnlineJudge::set_sandbox_absolute_path(string path)
{
    m_sandbox_absolute_path = path;
}

void OnlineJudge::set_callback(string ip, string port)
{
    m_callback_ip = ip;
    m_callback_port = port;
}

void OnlineJudge::set_ext_param(string strExtParam)
{
    m_ext_param = strExtParam;
}