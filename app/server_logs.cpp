#include "server_logs.h"

void print_log(const char*file, const char* func, int line, const char* level, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    char buf[MAX_LOG_MSG_SIZE] = {};
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    write_log_to_file(file, func, line, level, buf);
}

void write_log_to_stdout(char *buf)
{
    printf("%s\n", buf);
}

void write_log_to_file(const char*file, const char* func, int line, const char* level, const char* buf)
{
    std::ofstream fs;
    std::stringstream ss;
    ss << DEFAULT_LOG_PATH;
    ss << Tools::serializeDate('-') << ".log";
    fs.open(ss.str(), std::ios::app);
    if (!fs.is_open()) {
        return ;
    }
    fs << "[" << Tools::serializeDateTime() << "]";
    fs << "[" << level << "]";
    fs << "[" << file << " " << func << "()" << " :" << line << "]";
    fs << "[" << buf << "]" << std::endl;
    fs.close();
}