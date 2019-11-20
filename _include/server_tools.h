#ifndef _CSERVER_EASY_TOOLS_H_
#define _CSERVER_EASY_TOOLS_H_

#include <bits/stdc++.h>
#include <ctime>

class Tools {
public:
    static std::string IntToString(int val);

    static int StringToInt(const char *str);
    
    static int StringToInt(std::string str);

    /**
        清除某一行的注释
        1, 第一版只清除# 后面的东西
    */
    static void clearComment(std::string &str);

    /**
        按照某个符号切割字符串
    */
    static std::vector<std::string> splitString(const std::string &str, char splitChar = ';');

    /**
        去除前后空格符    
    */
    static void trim(std::string &str);

    // 时间函数
    static tm getDateTime();

    // 获取日期
    static std::string serializeDate(char split = '/');

    // 获取日期和时间
    static std::string serializeDateTime();
};

#endif