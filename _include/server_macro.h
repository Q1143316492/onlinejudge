#ifndef _SERVER_MACRO_H_
#define _SERVER_MACRO_H_

#include <bits/stdc++.h>

#define CHECKER_ERR_1(x) { if (x == -1) { return 1 } else {} }

// AC, WA, CE, RE, TLE, OLE, MLE, PE, SE

const std::string AC = "AC";
const std::string WA = "WA";
const std::string CE = "CE";
const std::string RE = "RE";
const std::string PE = "PE";
const std::string SE1 = "SYSTEM_ERROR";
const std::string SE2 = "SUBMIT_ERROR";
const std::string TLE = "RE";
const std::string OLE = "PE";
const std::string MLE = "PE";

// 评测机默认行为，评测沙盒下 main.cpp < data.in > data.out 答案文件 ans.txt

const std::string OJ_DEFAULT_ACT = "1001";

#define STACK_SIZE (512 * 512)

#endif