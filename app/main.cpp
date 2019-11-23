#include <bits/stdc++.h>

#include "online_judge.h"
#include "server_conf.h"
#include "server_tools.h"
#include "server_logs.h"
#include "server_macro.h"

#include "union_test.h"

#include <sys/types.h>   // DIR
#include <dirent.h>      // opendir
#include <sys/stat.h>
#include <sys/types.h>

OnlineJudge *judge;
ServerConf  *sconf;

std::string g_log_path = DEFAULT_LOG_PATH;

/**
    参数                        参考值
    【0】...
    【1】时间限制 ms                1000 ms
    【2】内存限制 kb                128 * 1024 kb ==> (131072kb)
    【3】沙盒目录绝对路径
    【4】回调ip
    【5】回调端口
    【6】补充参数，上面五个是默认行为，补充参数是额外行为
        actid=1001&input_list=1;2;3;4&param

    参考值
    =======================
    ./oj 1000 131072 /home/weilinchen/workspace/sandbox/sandbox 127.0.0.1 7736 actid=1001&file=1;2

    ps : main输入参数内存不足，改到配置文件
    time_limit    = 1000 ms
    memery_limit  = 131072 kb
    sandbox_path  = ...
    callback_ip   = ...
    callback_port = ... 

    actid = ...
    file  = ...;...;... // 用;切割文件名  name.in name.out name.ans

*/
int main(int argc, char **argv)
{
    if (argc != 2) {
        INFO_LOG("online judge param error. param num = %d not allow", argc);
        exit(0);
    }
    if (access(argv[1], F_OK)) {
        INFO_LOG("config file not exist", argc);
        exit(0);
    }
    if (mkdir(DEFAULT_LOG_PATH, S_IWUSR) == -1) {
        INFO_LOG("create log dir fail.");
    } else {
        INFO_LOG("log file not exist. create log dir success.");
    }
    if (opendir(DEFAULT_LOG_PATH) == NULL) {
        INFO_LOG("log path not exist. use local path.");
        g_log_path = "./";
    }

    judge = new OnlineJudge();
    // 加载配置文件
    sconf = ServerConf::getInstance();
    sconf->loadCoreConf(argv[1]);
    
    // 设置评测机id
    if (sconf->getStrConf(JUDGE_ID) == "") {
        ERR_LOG("judge id not set");
        exit(0);
    }
    judge->set_judge_id(sconf->getStrConf(JUDGE_ID));

    // 初始化评测时间限制
    if (sconf->getIntConf(TIME_LIMIT) == ERROR_CONF_VALUE) {
        ERR_LOG("main param time limit range error");
        exit(0);
    }
    judge->set_time_limit(sconf->getIntConf(TIME_LIMIT));

    // 初始化评测内存限制
    if (sconf->getIntConf(MEMERY_LIMIT) == ERROR_CONF_VALUE) {
        ERR_LOG("main param memery limit range error");
        exit(0);
    }
    judge->set_memery_limit(sconf->getIntConf(MEMERY_LIMIT));

    // 初始化沙盒文件夹目录绝对路径
    std::string sandbox_path = sconf->getStrConf(SANDBOX_PATH);
    if (sandbox_path.size() == 0 || sandbox_path.size() > 128 || opendir(sandbox_path.c_str()) == NULL) {
        ERR_LOG("main param sandbox absolute path not exist or error");
        exit(0);
    }
    judge->set_sandbox_absolute_path(sandbox_path);

    // 设置回调ip, 端口
    judge->set_callback(sconf->getStrConf(CALLBACK_IP), sconf->getStrConf(CALLBACK_PORT));

    // 设置扩展参数 这个暂时没用
    judge->set_ext_param(sconf->getStrConf(EXT_PARAM));
    
    std::string actid = sconf->getStrConf(JUDGE_ACTION);
    std::string files = sconf->getStrConf(JUDGE_FILES);
    judge->set_judging(actid, files);

    judge->prepare_run();
    judge->start_run();
    judge->finish_run();
    
    return 0;
}
