#include <bits/stdc++.h>

#include "online_judge.h"
#include "server_tools.h"
#include "server_logs.h"
#include "server_macro.h"

#include<sys/types.h>   // DIR
#include<dirent.h>      // opendir

OnlineJudge judge;

/**
    参数
    【0】...
    【1】时间限制 ms
    【2】内存限制 kb
    【3】沙盒目录绝对路径
    【4】回调ip
    【5】回调端口
    【6】补充参数，上面五个是默认行为，补充参数是额外行为
        actid=1001&input_list=1;2;3;4&param
*/
int main(int argc, char **argv)
{
    if (argc != 7) {
        INFO_LOG("online judge param error. param num = %d not allow", argc);
        exit(0);
    }
    
    // 初始化评测时间限制
    if (!Tools::isNumber(argv[1])) {
        ERR_LOG("main param time limit range error");
        exit(0);
    }
    judge.set_time_limit(Tools::stringToInt(argv[1]));

    // 初始化平常内存限制
    if (!Tools::isNumber(argv[2])) {
        ERR_LOG("main param memery limit range error");
        exit(0);
    }
    judge.set_memery_limit(Tools::stringToInt(argv[2]));

    // 初始化沙盒文件夹目录绝对路径
    if (strlen(argv[3]) > 128 || opendir(argv[3]) == NULL) {
        ERR_LOG("main param sandbox absolute path not exist or error");
        exit(0);
    }
    judge.set_sandbox_absolute_path(argv[3]);

    // 设置回调ip, 端口
    judge.set_callback(argv[4], argv[5]);

    // 设置扩展参数
    judge.set_ext_param(argv[6]);

    judge.print_judge_msg();

    // char *path = argv[1];

    // docker::test_clone();

    
    return 0;
}
