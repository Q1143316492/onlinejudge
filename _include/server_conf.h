#ifndef _CSERVER_CONF_H_
#define _CSERVER_CONF_H_

#include <bits/stdc++.h>
#include "server_macro.h"
#include "server_tools.h"

class ServerConf {
private:
    ServerConf();
public:
    ~ServerConf();
private:
    static ServerConf *m_instance;
    static std::mutex m_instance_mux;
    std::unordered_map<std::string, std::string> confkv;
public:
    static ServerConf* getInstance() {
		if(m_instance == NULL) {
            std::unique_lock<std::mutex> uni_lok(m_instance_mux);
			if(m_instance == NULL) {					
				m_instance = new ServerConf();
			}
		}
		return m_instance;
	}	

    int loadCoreConf(std::string strFilePath);

    std::string showAllConf();

    std::string getStrConf(std::string key);

    int getIntConf(std::string key);

};

#endif