#include "server_tools.h"

// 字符串相关工具函数

std::string Tools::IntToString(int val)
{
    std::stringstream ss;
    ss << val;
    return ss.str();
}

int Tools::StringToInt(const char *str)
{
    int val = 0;
    for(size_t i = 0; str[i]; i++ ) {
        val = val * 10 + str[i] - '0';
    }
    return val;
}

int Tools::StringToInt(std::string str)
{
    int val = 0;
    for(size_t i = 0; i < str.size(); i++ ) {
        val = val * 10 + str[i] - '0';
    }
    return val;
}

void Tools::clearComment(std::string &str)
{
    char markComment[] = "#[({<";
    for (size_t i = 0; i < str.size(); i++ ) {
        for (size_t j = 0; markComment[j]; j++ ) {
            if (str[i] == markComment[j]) {
                str = str.substr(0, i);
                return ;
            }
        }
    }
}

std::vector<std::string> Tools::splitString(const std::string &str, char splitChar)
{
    std::vector<std::string> vecSplitStr;
    std::stringstream ss;
    for (size_t i = 0; i < str.size(); i++ ) {
        if (str[i] == splitChar) {
            vecSplitStr.push_back(ss.str());
            ss.str("");
        } else {
            ss << str[i];
        }
    }
    if (ss.str().size() != 0) {
        vecSplitStr.push_back(ss.str());
        ss.clear();
    }
    return vecSplitStr;
}

void Tools::trim(std::string &str)
{
    if (str == "" || str.empty()) {
        return ;
    }
    int l = 0, r = str.size() - 1;
    while(str[l] == ' ') { l++; }
    while(str[r] == ' ') { r--; }
    str = str.substr(l, r - l + 1);
}

// 时间相关工具函数

/**
struct tm {
  int tm_sec;   // 秒，正常范围从 0 到 59，但允许至 61
  int tm_min;   // 分，范围从 0 到 59
  int tm_hour;  // 小时，范围从 0 到 23
  int tm_mday;  // 一月中的第几天，范围从 1 到 31
  int tm_mon;   // 月，范围从 0 到 11
  int tm_year;  // 自 1900 年起的年数
  int tm_wday;  // 一周中的第几天，范围从 0 到 6，从星期日算起
  int tm_yday;  // 一年中的第几天，范围从 0 到 365，从 1 月 1 日算起
  int tm_isdst; // 夏令时
}
*/

tm Tools::getDateTime()
{
    time_t now = time(0);
    return *localtime(&now);
}

std::string Tools::serializeDate(char split)
{
    tm ltm = getDateTime();
    std::stringstream ss;
    ss << (ltm.tm_year + 1900) << split << (ltm.tm_mon + 1) << split << ltm.tm_mday;
    return ss.str();
}

std::string Tools::serializeDateTime()
{
    tm ltm = getDateTime();
    std::stringstream ss;
    ss << (ltm.tm_year + 1900) << "/" << (ltm.tm_mon + 1) << "/" << ltm.tm_mday << "-";
    ss << std::setw(2) << std::setfill('0') << ltm.tm_hour << ":";
    ss << std::setw(2) << std::setfill('0') << ltm.tm_min << ":";
    ss << std::setw(2) << std::setfill('0') << ltm.tm_sec;
    return ss.str();
}