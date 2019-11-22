#include "server_conf.h"

ServerConf* ServerConf::m_instance = nullptr;

std::mutex ServerConf::m_instance_mux;

ServerConf::ServerConf()
{
}

ServerConf::~ServerConf()
{
}

int ServerConf::loadCoreConf(std::string strFilePath)
{
    std::ifstream fp(strFilePath);
    std::string strLine;
    int iLineNum = 0;
    int iConfCount = 0;
    while (getline(fp, strLine)) {
        iLineNum++;
        if (strLine.size() > MAX_CONF_LINE_SIZE) {
            continue;
        }
        Tools::clearComment(strLine);
        if (strLine.size() == 0 || strLine == "") {
            continue;
        }
        std::vector<std::string>vecKv = Tools::splitString(strLine, '=');
        if (vecKv.size() != 2) {
            continue;
        }
        Tools::trim(vecKv[0]);
        Tools::trim(vecKv[1]);
        if (!vecKv[0].size() || !vecKv[1].size()) {
            continue;
        }
        iConfCount++;
        if (confkv.count(vecKv[0])) {
            confkv[ vecKv[0] ] += ";";
            confkv[ vecKv[0] ] += vecKv[1];
        } else {
            confkv[ vecKv[0] ] = vecKv[1];
        }
    }
    return iConfCount;
}

std::string ServerConf::showAllConf()
{
    std::stringstream ss("");
    for (std::unordered_map<std::string, std::string>::iterator it = confkv.begin(); it != confkv.end(); it++ ) {
        ss << "[" << it->first << "=" << it->second << "]\n";
    }
    return ss.str();
}

std::string ServerConf::getStrConf(std::string key)
{
    if (this->confkv.count(key)) {
        return this->confkv[key];
    }
    return "";
}

int ServerConf::getIntConf(std::string key)
{
    std::string strVal = this->getStrConf(key);
    int iVal = ERROR_CONF_VALUE;
    if (!strVal.empty() && strVal != "") {
        iVal = Tools::stringToInt(strVal);
    }
    return iVal;
}