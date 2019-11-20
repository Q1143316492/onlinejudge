#include "test_clone.h"

namespace docker {

void test_clone()
{
    cout << "test clone begin..." << endl;

    Container container;
    container.start();
    
    cout << "test clone end..." << endl;
}

Container::Container()
{
    this->host_name = "cwl_server";
    this->root_dir  = "./images";
}

void Container::start()
{
    auto setup = [](void *args) -> int {
        auto _this = reinterpret_cast<Container *>(args);
        _this->set_hostname();
        _this->set_rootdir();
        _this->set_procsys();
        _this->start_bash();
        return PROC_WAIT;
    };

    pid_t child_pid = clone(setup, child_stack + STACK_SIZE,
                        CLONE_NEWUTS| // 添加 UTS namespace, 确定hostname修改会影响父进程
                        CLONE_NEWNS|  // Mount namespace
                        CLONE_NEWPID| // PID   namespace
                        CLONE_NEWNET| // Net   namespace
                        SIGCHLD,
                        this);
    waitpid(child_pid, nullptr, 0);
}

void Container::start_bash() 
{
    //std::string bash = "/bin/bash";
    std::string bash = "/hello";
    char *c_bash = new char[bash.length() + 1];
    strcpy(c_bash, bash.c_str());
    char* const child_args[] = { c_bash, NULL };
    int ret = execv(child_args[0], child_args);
    printf("exec ret = %d\n", ret);
    perror("exec:");
    delete []c_bash;
}

void Container::set_hostname() 
{
    sethostname(host_name.c_str(), host_name.length());
}

void Container::set_rootdir() 
{
    chdir(root_dir.c_str());
    chroot(".");
}

void Container::set_procsys() 
{
    // mount("none", "/proc", "proc", 0, nullptr);
    // mount("none", "/sys", "sysfs", 0, nullptr);
}

}