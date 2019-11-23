#include "online_judge.h"

OnlineJudge::OnlineJudge()
{
    m_child_stack  = new char[STACK_SIZE];
    m_stack_limit  = 256 * 1024;
    m_output_limit = 512 * 1024;
    m_is_stop      = false;
}

OnlineJudge::~OnlineJudge()
{
    delete [] m_child_stack;
}

int OnlineJudge::compare_answer(string &msg)
{
    std::stringstream cmpFileAns;
    std::stringstream cmpFileStd;

    cmpFileAns << m_sandbox_absolute_path << "/" << m_str_judge_file << ".out";
    cmpFileStd << m_sandbox_absolute_path << "/" << m_str_judge_file << ".ans";
    
    int ansfd = open(cmpFileAns.str().c_str(), O_RDONLY);
    int stdfd = open(cmpFileStd.str().c_str(), O_RDONLY);

    char chans = 0, chstd = 0;
    int retans = 0, retstd = 0;

    if (ansfd == -1 || stdfd == -1) {
        msg = "open judge file first error";
        return -1;
    }

    // #define CMP_RET_AC 0
    // #define CMP_RET_PE 1
    // #define CMP_RET_WA 2

    int flag = 0;
    while (true) {
        retans = read(ansfd, &chans, 1);
        retstd = read(stdfd, &chstd, 1);
        if (retans == 0 && retstd == 0) {
            break;
        }
        if (retans && retstd && chans == chstd) {
            continue;
        } else {
            flag = CMP_RET_WA;
            break;
        }
    }
    close(ansfd);
    close(stdfd);
    if (flag == CMP_RET_AC) {
        return flag;
    }
    ansfd = open(cmpFileAns.str().c_str(), O_RDONLY);
    stdfd = open(cmpFileStd.str().c_str(), O_RDONLY);
    
    if (ansfd == -1 || stdfd == -1) {
        msg = "open judge file second error";
        return -1;
    }
    flag = 1;
    while (true) {
        while (true) {
            retans = read(ansfd, &chans, 1);
            if (retans == 0 || (chans != ' ' && chans != '\n' && chans != '\t')) {
                break;
            }
        }
        while (true) {
            retstd = read(stdfd, &chstd, 1);
            if (retstd == 0 || (chstd != ' ' && chstd != '\n' && chstd != '\t')) {
                break;
            }
        }
        if (retans == 0 && retstd == 0) break;
        if (retans == 0 || retstd == 0 || chans != chstd) {
            flag = CMP_RET_WA;
            break;
        } 
    } 
    close(ansfd);
    close(stdfd);
    return flag;
}

string OnlineJudge::pack_result(string ret, int time, int memery, string msg)
{
    stringstream ss;
    ss << m_judge_id << "," << ret << "," << time << "," << memery << "," << msg;
    return ss.str();
}

string OnlineJudge::pack_result(string ret, string time, string memery, string msg)
{
    stringstream ss;
    ss << ret << "," << time << "," << memery << "," << msg;
    return ss.str();
}

string OnlineJudge::wait_result(pid_t child_pid)
{
    int status = 0;
    struct rusage ru;
    struct user_regs_struct regs;

    string run_result = AC;
    int    run_time;
    int    run_memery;

    pid_t ret_pid = wait4(child_pid, &status, WSTOPPED, &ru);
    if (ret_pid == -1) {
        ERR_LOG("wait4 ERROR");
        perror("wait4 ERROR");
        return this->pack_result(SE1, 0, 0, "");
    }

    run_time = ru.ru_utime.tv_sec * 1000
        + ru.ru_utime.tv_usec / 1000
        + ru.ru_stime.tv_sec * 1000
        + ru.ru_stime.tv_usec / 1000;
    run_memery = ru.ru_maxrss;

    INFO_LOG("success run time = %d ms, memery = %d kb", run_time, run_memery);

    if (WIFSIGNALED(status)) {
        int signum = WTERMSIG(status);
        switch (signum) {
            case SIGXFSZ:
                run_result = OLE;
                break;
            case SIGSEGV:   // 内存访问错误
                if (run_memery > m_memery_limit) {
                    run_result = MLE;
                } else {
                    run_result = RE;
                }
                break;
            case SIGALRM:   // 定时器超时
            case SIGXCPU:   // 超出CPU时间限制 setrlimit
                run_result = TLE;
                break;
            default:
                run_result = RE;
                break;
        }
    } else {
        if (run_time > m_time_limit) {
            run_result = TLE;
        } else if (run_memery > m_memery_limit) {
            run_result = MLE;
        }
    }
    return pack_result(run_result, run_time, run_memery, "");
}

int OnlineJudge::start_main(string bash)
{
    char *c_bash = new char[bash.length() + 1];
    strcpy(c_bash, bash.c_str());
    char* const child_args[] = { c_bash, NULL };
    int ret = execv(child_args[0], child_args);
    if (ret == -1) {
        ERR_LOG("exec fail");
        perror("exec fail");
    }
    delete []c_bash;
    return ret;
}

int OnlineJudge::set_freopen(string in, string out)
{
    freopen(in.c_str(),  "r", stdin);
    freopen(out.c_str(), "w", stdout);
}

int OnlineJudge::set_lower_power()
{
    return setuid(10000);
}

int OnlineJudge::set_rlimit()
{
    // time_limit       ms
    // memery_limit     kb
    struct rlimit rl;
    // CPU 时间最大量值(秒),当超过此软限制，向该进程发生SIGXCPU
    rl.rlim_cur = m_time_limit / 1000 + 1;
    if (m_time_limit % 1000 > 800) {
        rl.rlim_cur += 1;
    }
    rl.rlim_max = rl.rlim_cur + 1;
    if (setrlimit(RLIMIT_CPU, &rl)) {
        ERR_LOG("set RLIMIT_CPU fail");
        return -1;
    }
    // 数据段的最大字节长度。初始化数据，非初始化数据以及堆的总和
    rl.rlim_cur = m_memery_limit * 1024;
    rl.rlim_max = rl.rlim_cur + 1024;
    if (setrlimit(RLIMIT_DATA, &rl)) {
        ERR_LOG("set RLIMIT_DATA fail");
        return -1;
    } 
    // 进程总共的可用存储空间的最大长度(字节)，影响到sbrk函数和mmap函数
    rl.rlim_cur = m_memery_limit * 1024 * 2;
    rl.rlim_max = rl.rlim_cur + 1024;
    if (setrlimit(RLIMIT_AS, &rl)) {
        ERR_LOG("set RLIMIT_AS fail");
        return -1;
    }
    // 栈的最大字节长度
    rl.rlim_cur = m_stack_limit * 1024;
    rl.rlim_max = rl.rlim_cur + 1024;
    if (setrlimit(RLIMIT_STACK, &rl)) {
        ERR_LOG("set RLIMIT_STACK fail");
        return -1;
    }
    // 可以创建文件的最大字节数 SIGXFSZ
    rl.rlim_cur = m_output_limit * 1024;
    rl.rlim_max = rl.rlim_cur + 1024;
    if (setrlimit(RLIMIT_FSIZE, &rl)) { 
        ERR_LOG("set RLIMIT_FSIZE fail");
        return -1;
    }
    // 限制子进程数
    // rl.rlim_cur = 3;
    // rl.rlim_max = 5;
    // if (setrlimit(RLIMIT_NPROC, &rl)) {
    //     ERR_LOG("set RLIMIT_STACK fail");
    //     return -1;
    // }
    INFO_LOG("set rlimit success");
}

void OnlineJudge::set_hostname()
{
    sethostname(m_hostname.c_str(), m_hostname.length());
}

void OnlineJudge::set_rootdir()
{
    chdir(m_sandbox_absolute_path.c_str());
    chroot(".");
}

void OnlineJudge::set_procsys()
{
    mount("none", "/proc", "proc", 0, nullptr);
    mount("none", "/sys", "sysfs", 0, nullptr);
}

void OnlineJudge::run_default_action()
{
    this->m_hostname = "default_judge";

    if (pipe(this->m_pipe_fds) == -1) {
        ERR_LOG("default judge init pipe fail");
        return;
    }

    auto setup = [](void *args) -> int {
        auto _this = reinterpret_cast<OnlineJudge *>(args);
        _this->set_hostname();
        _this->set_rootdir(); 
        _this->set_procsys();
        close(_this->m_pipe_fds[0]);
        
        if (_this->m_act_id == OJ_RUN_BASH) {
            _this->start_main(_this->m_exec_file);
            return 0;
        }

        pid_t pid = fork();
        if (pid == -1) {
            ERR_LOG("contain fork() error");
            return 1;
        }
        if (pid == 0) { // child   
            if (_this->set_rlimit() == -1) {
                return 1;
            }
            if (_this->set_lower_power() == -1) {
                return 1;
            }
            stringstream infile;
            stringstream outfile;
            infile << _this->m_str_judge_file << ".in";
            outfile << _this->m_str_judge_file << ".out";
            _this->set_freopen(infile.str(), outfile.str());
            if (_this->start_main(_this->m_exec_file) == -1) {
                return 1;
            }
            return 0;
        } else {       // father
            string result = _this->wait_result(pid);
            close(_this->m_pipe_fds[0]);
            write(_this->m_pipe_fds[1], result.c_str(), result.size());
        }
        return 0;
    };
    if (m_child_stack == NULL) {
        ERR_LOG("child_stack null");
        return;
    }
    pid_t child_pid = clone(setup, m_child_stack + STACK_SIZE,
                        CLONE_NEWUTS| // 添加 UTS namespace, 确定hostname修改会影响父进程
                        CLONE_NEWNS|  // Mount namespace
                        CLONE_NEWPID| // PID   namespace
                        CLONE_NEWNET| // Net   namespace
                        SIGCHLD,
                        this);
    close(this->m_pipe_fds[1]);

    // 管道读取容器内评测结果
    char buf[128] = {};
    read(this->m_pipe_fds[0], buf, 128);
    DEBUG_LOG("pipe (%s)", buf);
    waitpid(child_pid, nullptr, 0);

    // 评测id,结果,时间,内存,[额外消息]
    vector<string>vecRet = Tools::splitString(buf, ',');
    if (vecRet.size() < 4) {
        ERR_LOG("judge error");
        return;
    }
    string run_msg = "";
    if (vecRet[1] == AC) {
        int ret = this->compare_answer(run_msg);
        if (ret == -1) {
            vecRet[1] = SE1;
        }
        if (ret == CMP_RET_PE) {
            vecRet[1] = PE;
        }
        if (ret == CMP_RET_WA) {
            vecRet[1] = WA;
        }
    }
    m_judge_result = pack_result(vecRet[0], vecRet[1], vecRet[2], run_msg);
}

void OnlineJudge::prepare_run()
{
    m_map_ext_param.clear();
    vector<string> paramkv = Tools::splitString(m_str_ext_param, '&');
    for (size_t i = 0; i < paramkv.size(); i++ ) {
        vector<string> kv = Tools::splitString(paramkv[i], '=');
        if (kv.size() == 2) {
            m_map_ext_param[kv[0]] = kv[1];
        }
    }
    stringstream judge_file_abs_path;
    judge_file_abs_path << m_sandbox_absolute_path << "/" << "main.cpp";
    if (access(judge_file_abs_path.str().c_str(), F_OK)) {
        ERR_LOG("judge file not exist.");
        m_is_stop = true;
        return;
    }
}

int OnlineJudge::compile_cpp(string compilefile)
{
    stringstream cmd;
    cmd << "g++ " << (m_sandbox_absolute_path + compilefile) << " -o";
    cmd << (m_sandbox_absolute_path + "/main");
    cmd << " -Wall -std=c++14 ";
    cmd << " 1>" << (m_sandbox_absolute_path + "/msg.log");
    cmd << " 2>" << (m_sandbox_absolute_path + "/err.log");
    system(cmd.str().c_str());
    
    std::string errlog = m_sandbox_absolute_path + "/err.log";

    std::ifstream file(errlog.c_str());
    std::string tmp;
    std::string msg;
    m_compile_error = "";
    while(file.is_open() && !file.eof()) {
        getline(file, tmp);
        m_compile_error += tmp;
    }
    file.close();
    if (m_compile_error.size() != 0) {
        return -1;
    } 
    std::string execfile = m_sandbox_absolute_path + "/main";
    if (access(execfile.c_str(), F_OK)) {
        m_compile_error = "compile fail. reason unknow";
    }
    return m_compile_error.size();
}

void OnlineJudge::start_run()
{
    if (m_is_stop) {
        INFO_LOG("start_run() exit.");
        return; 
    }
    if (m_act_id == OJ_DEFAULT_ACT) {
        INFO_LOG("online default action.");
        m_exec_file = "/main";
        INFO_LOG("begin compile");
        int cnt = compile_cpp("/main.cpp");
        if (cnt != 0) {
            INFO_LOG("compile_cpp fail [%s] %d %d", m_compile_error.c_str(), m_compile_error.size(), cnt);
            m_judge_result = pack_result(CE, "0", "0", m_compile_error);
            return;
        }
        run_default_action();
        return;
    }    
    if (m_act_id == OJ_RUN_BASH) {
        INFO_LOG("online default action.");
        m_exec_file = "/bin/bash";
        run_default_action();
        return;
    }
    INFO_LOG("online mode action not define");
}

void OnlineJudge::finish_run()
{
    Tools::callback_client(m_judge_result, m_callback_ip, m_callback_port, JUDGE_CALLBACK_TYPE);
}

void OnlineJudge::print_judge_msg()
{
    cout << "time_limit: "            << m_time_limit << endl;
    cout << "memery_limit: "          << m_memery_limit << endl;
    cout << "sandbox_absolute_path: " << m_sandbox_absolute_path << endl;
    cout << "callback_ip: "           << m_callback_ip << endl;
    cout << "callback_port: "         << m_callback_port << endl;
    cout << "ext_param: "             << m_str_ext_param << endl;
    cout << "m_act_id: "              << m_act_id << endl;
    cout << "m_str_judge_file: "     << m_str_judge_file << endl;
}

void OnlineJudge::set_judging(const string &actid, const string &files)
{
    m_act_id = actid;
    m_str_judge_file = files;
}

void OnlineJudge::set_judge_id(const string &id)
{
    m_judge_id = id;
}

void OnlineJudge::set_time_limit(int iTimeLimit)
{
    m_time_limit = iTimeLimit;
}

void OnlineJudge::set_memery_limit(int iMemeryLimit)
{
    m_memery_limit = iMemeryLimit;
}

void OnlineJudge::set_sandbox_absolute_path(const string &path)
{
    m_sandbox_absolute_path = path;
}

void OnlineJudge::set_callback(const string &ip, const string &port)
{
    m_callback_ip = ip;
    m_callback_port = port;
}

void OnlineJudge::set_ext_param(const string &strExtParam)
{
    m_str_ext_param = strExtParam;
}