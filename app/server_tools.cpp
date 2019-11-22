#include "server_tools.h"


bool Tools::isNumber(char *num)
{
    for (int i = 0; num[i]; i++ ) {
        if (num[i] < '0' || num[i] > '9') {
            return false;
        }
    }
    return true;
}

std::string Tools::intToString(int val)
{
    std::stringstream ss;
    ss << val;
    return ss.str();
}

int Tools::callback_client(std::string msg, std::string ip, std::string port, int callbasktype)
{
    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    inet_pton( AF_INET, ip.c_str(), &server_address.sin_addr );
    server_address.sin_port = htons(Tools::stringToInt(port.c_str()));

    int sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        ERR_LOG("callbask socket create fail, msg = %s, ip = %s, port = %s", msg.c_str(), ip.c_str(), port.c_str());
        return -1;
    }
    if (connect(sockfd, (struct sockaddr*)&server_address, sizeof( server_address )) < 0)
    {
        ERR_LOG("callbask connect fail, msg = %s, ip = %s, port = %s", msg.c_str(), ip.c_str(), port.c_str());
        return -1;
    }
    else
    {
        std::stringstream ss;
        ss << "ret=0&msg=" << msg.c_str();
        char buf[128] = {};
        uint32_t len  = htonl(ss.str().size());
        uint32_t type = htonl(callbasktype);
        write(sockfd, &len, 4);
        write(sockfd, &type, 4);
        write(sockfd, ss.str().c_str(), len);
    }
    close(sockfd);
    return 0;
}

int Tools::stringToInt(const char *str)
{
    int val = 0;
    for(size_t i = 0; str[i]; i++ ) {
        val = val * 10 + str[i] - '0';
    }
    return val;
}

int Tools::stringToInt(std::string str)
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