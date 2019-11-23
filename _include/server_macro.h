#ifndef _SERVER_MACRO_H_
#define _SERVER_MACRO_H_

// tools

#define ERROR_CONF_VALUE 0x7FFFFFFF

// conf

#define MAX_CONF_LINE_SIZE 1024
#define JUDGE_ID        "judge_id"
#define TIME_LIMIT      "time_limit"
#define MEMERY_LIMIT    "memery_limit"
#define SANDBOX_PATH    "sandbox_path"
#define CALLBACK_IP     "callback_ip"
#define CALLBACK_PORT   "callback_port"
#define EXT_PARAM       "ext_param"
#define JUDGE_ACTION    "judge_action"
#define JUDGE_FILES     "judge_files"

// AC, WA, CE, RE, TLE, OLE, MLE, PE, SE

#define AC    "AC"
#define WA    "WA"
#define CE    "CE"
#define RE    "RE"
#define PE    "PE"
#define SE1   "SYSTEM_ERROR"
#define SE2   "SUBMIT_ERROR"
#define TLE   "RE"
#define OLE   "PE"
#define MLE   "PE"

#define CMP_RET_AC 0
#define CMP_RET_PE 1
#define CMP_RET_WA 2

// 评测机默认行为，评测沙盒下 main.cpp < data.in > data.out 答案文件 ans.txt

#define OJ_DEFAULT_ACT  "1001"
#define OJ_RUN_BASH     "1002"

#define JUDGE_CALLBACK_TYPE 1001

#define STACK_SIZE (512 * 512)

#endif