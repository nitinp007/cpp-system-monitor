#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "constants.h"


using namespace std;

class ProcessParser{
private:
    std::ifstream stream;
    public:
    static string getCmd(string pid);
    static vector<string> getPidList();
    static std::string getVmSize(string pid);
    static std::string getCpuPercent(string pid);
    static long int getSysUpTime();
    static std::string getProcUpTime(string pid);
    static string getProcUser(string pid);
    static vector<string> getSysCpuPercent(string coreNumber = "");
    static float getSysRamPercent();
    static string getSysKernelVersion();
    static int getTotalThreads();
    static int getTotalNumberOfProcesses();
    static int getNumberOfRunningProcesses();
    static string getOSName();
    static std::string PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2);
    static bool isPidExisting(string pid);
  
    static int getNumOfCores();
    static float getSysActiveCpuTime(std::vector<std::string> values);
    static float getSysIdleCpuTime(std::vector<std::string> values);

};

// TODO: Define all of the above functions below:
long int ProcessParser::getSysUpTime()
{

    std::string line;
    long int result;

    std::ifstream stream = Util::getStream(Path::basePath() + Path::upTimePath());
    std::getline(stream, line);
    std::istringstream buf(line);
    std::istream_iterator<std::string> beg(buf),end;
    vector<string> values(beg,end);

    result = stoi(values[0]);
    return result;

}

std::string ProcessParser::getProcUpTime(std::string pid)
{
    
    std::string line;
    float result;
    std::ifstream stream = Util::getStream(Path::basePath() + pid + Path::statPath());
    std::getline(stream, line);

    std::istringstream buf(line);
    std::istream_iterator<string> beg(buf), end;
    vector<string> values(beg,end);
    
    float freq = sysconf(_SC_CLK_TCK); 
    //float freq = 100;
    result = stof(values[13])/freq;


    return std::to_string(result);

}

std::string ProcessParser::getVmSize(std::string pid)
{
    std::string line;
    std::string name = "VmData";
    std::string value;
    float result;
    ifstream stream = Util::getStream(Path::basePath() + pid + Path::statusPath());
    while(std::getline(stream, line))
    {
        if(line.compare(0,name.size(),name) == 0)
        {
            std::istringstream buf(line);
            std::istream_iterator<string> beg(buf), end;
            vector<string> values(beg,end);
            result = (stof(values[1])/float(1000 * 1000) );
            break;
        }
    }
    return to_string(result);
}

std::string ProcessParser::getCpuPercent(std::string pid)
{
    std::string line;
    float result= 0.0;
    ifstream stream = Util::getStream(Path::basePath() + pid + Path::statPath());
    std::getline(stream, line);
    std::istringstream buf(line);
    std::istream_iterator<string> beg(buf), end;
    vector<string> values(beg,end);

    float utime = stof(ProcessParser::getProcUpTime(pid));
    float stime = stof(values[14]);
    float cutime = stof(values[15]);
    float cstime = stof(values[16]);
    float starttime = stof(values[21]);
    float uptime = ProcessParser::getSysUpTime();

    float freq = sysconf(_SC_CLK_TCK);
    //float freq = 100.0;
    float total_time = utime + stime + cutime + cstime;
    float seconds = uptime - (starttime/freq);
    result = 100.0*((total_time/freq)/seconds);
    return to_string(result);

}

std::string ProcessParser::getProcUser(std::string pid)
{
    std::string line;
    std::string name = "uid";
    string result="";

    ifstream stream = Util::getStream(Path::basePath() + pid + Path::statusPath());
    while (std::getline(stream, line))
    {
        if(line.compare(0,name.size(),name) == 0)
        {
            std::istringstream buf(line);
            std::istream_iterator<string> beg(buf), end;
            vector<string> values(beg,end);
            result = values[1];
            break;
        }
    }

    stream = Util::getStream("/etc/passwd");
    name = ("x:"+ result);
    while (std::getline(stream, line))
    {
        if(line.find(name) != std::string::npos)
        {
            result = line.substr(0,line.find(":"));
        }
    }
    return result;
}

std::string ProcessParser::getCmd(std::string pid)
{
    std::string line;
    ifstream stream = Util::getStream(Path::basePath() + pid + Path::cmdPath());
    std::getline(stream, line);
    return line;
}

int ProcessParser::getNumOfCores()
{
    std::string line;
    std::string name = "cpu cores";
    string result="";
    ifstream stream = Util::getStream(Path::basePath() + "cpuinfo");
    while(std::getline(stream,line))
    {
        if(line.compare(0,name.size(),name) == 0)
        {
            std::istringstream buf(line);
            std::istream_iterator<string> beg(buf), end;
            vector<string> values(beg,end);
            result = values[3];
            break;
        }
    }
    return stoi(result);
}

std::vector<std::string>ProcessParser::getSysCpuPercent(string coreNumber)
{
    std::string line;
    std::string name = "cpu" + coreNumber;

    ifstream stream = Util::getStream(Path::basePath() + Path::statPath());
    while(std::getline(stream,line))
    {
        if(line.compare(0,name.size(),name) == 0)
        {
            std::istringstream buf(line);
            std::istream_iterator<string> beg(buf), end;
            vector<string> values(beg,end);
            return values;
        }
    }
    return (vector<std::string>());

}

float ProcessParser::getSysActiveCpuTime(vector<string> values)
{
    return (stof(values[S_USER]) +
            stof(values[S_NICE]) +
            stof(values[S_SYSTEM]) +
            stof(values[S_IRQ]) +
            stof(values[S_SOFTIRQ]) +
            stof(values[S_STEAL]) +
            stof(values[S_GUEST]) +
            stof(values[S_GUEST_NICE]));
}

float ProcessParser::getSysIdleCpuTime(vector<string>values)
{
    return (stof(values[S_IDLE]) + stof(values[S_IOWAIT]));
}

std::string ProcessParser::PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2)
{
    /*
    Because CPU stats can be calculated only if you take measures in two different time,
    this function has two paramaters: two vectors of relevant values.
    We use a formula to calculate overall activity of processor.
    */
    float active_time = getSysActiveCpuTime(values2)-getSysActiveCpuTime(values1);
    float idle_time = getSysIdleCpuTime(values2) - getSysIdleCpuTime(values1);
    float total_time = active_time + idle_time;
    float result = 100.0*(active_time / total_time);
    return to_string(result);
}

float ProcessParser::getSysRamPercent()
{
    std::string line;
    float result;
    std::string name1 = "MemAvailable:";
    std::string name2 = "MemFree:";
    std::string name3 = "Buffers:";

    float total_mem = 0;
    float free_mem = 0;
    float buffers = 0;

    ifstream stream = Util::getStream(Path::basePath() + Path::memInfoPath());

    while(std::getline(stream,line))
    {
        if(line.compare(0,name1.size(),name1) == 0)
        {
            std::istringstream buf(line);
            std::istream_iterator<string> beg(buf), end;
            vector<string> values(beg,end);
            total_mem = stof(values[1]);
        }
        else if(line.compare(0,name2.size(),name2) == 0)
        {
            std::istringstream buf(line);
            std::istream_iterator<string> beg(buf), end;
            vector<string> values(beg,end);
            free_mem = stof(values[1]);
        }
        else if(line.compare(0,name3.size(),name3) == 0)
        {
            std::istringstream buf(line);
            std::istream_iterator<string> beg(buf), end;
            vector<string> values(beg,end);
            buffers = stof(values[1]);
        }
    }
    result = (100.0*(1-(free_mem/(total_mem-buffers))));
    return result;
}
std::string ProcessParser::getSysKernelVersion()
{

    std::string line;
    std::string name = "Linux version ";
    string result="";

    ifstream stream = Util::getStream(Path::basePath() + Path::versionPath());
    while (std::getline(stream, line))
    {
        if(line.compare(0,name.size(),name) == 0)
        {
            std::istringstream buf(line);
            std::istream_iterator<string> beg(buf), end;
            vector<string> values(beg,end);
            result = values[2];
            break;
        }
    }
    return result;
}

std::string ProcessParser::getOSName()
{
    std::string line;
    std::string name = "PRETTY_NAME=";
    string result="";

    ifstream stream = Util::getStream("/etc/os-release");
    while (std::getline(stream, line))
    {
        if(line.compare(0,name.size(),name) == 0)
        {
            std::size_t found = line.find("=");
            found++;
            result = line.substr(found);
            result.erase(std::remove(result.begin(), result.end(), '"'), result.end());
        }
    }
    return result;
}

int ProcessParser::getTotalThreads()
{
    vector<string> pid_list = ProcessParser::getPidList();

    std::string line;
    std::string name = "Threads:";
    int result=0;
    for (unsigned int i = 0; i < pid_list.size(); i ++)
    {
        string pid = pid_list[i];
        ifstream stream = Util::getStream(Path::basePath() +pid+ Path::statusPath());
        while (std::getline(stream, line))
        {
            if(line.compare(0,name.size(),name) == 0)
            {
                std::istringstream buf(line);
                std::istream_iterator<string> beg(buf), end;
                vector<string> values(beg,end);
                result += stoi(values[1]);
            }
        }
    }
    return result;
}

int ProcessParser::getTotalNumberOfProcesses() {
    std::string line;
    std::string name = "processes";
    int result=0;

    ifstream stream = Util::getStream(Path::basePath() + Path::versionPath());
    while (std::getline(stream, line))
    {
        if(line.compare(0,name.size(),name) == 0)
        {
            std::istringstream buf(line);
            std::istream_iterator<string> beg(buf), end;
            vector<string> values(beg,end);
            result = stoi(values[1]);
            break;
        }
    }
    return result;

}

int ProcessParser::getNumberOfRunningProcesses() {
    std::string line;
    std::string name = "procs_running";
    int result=0;

    ifstream stream = Util::getStream(Path::basePath() + Path::versionPath());
    while (std::getline(stream, line))
    {
        if(line.compare(0,name.size(),name) == 0)
        {
            std::istringstream buf(line);
            std::istream_iterator<string> beg(buf), end;
            vector<string> values(beg,end);
            result = stoi(values[1]);
            break;
        }
    }
    return result;

}
std::vector<std::string> ProcessParser::getPidList() {

    DIR *dir;
    vector<string> container;

    //std::cout<<"getting pid list"<<std::endl;    
    if(!(dir = opendir("/proc"))) {
        throw std::runtime_error(strerror(errno));
    }
    while (dirent* dirp = readdir(dir)) {
        // is this a directory?
        if(dirp->d_type != DT_DIR)
            continue;
        // Is every character of the name a digit?
        if (all_of(dirp->d_name, dirp->d_name + std::strlen(dirp->d_name), [](char c){ return std::isdigit(c); })) {
            container.push_back(dirp->d_name);
        }
    }
    if(closedir(dir))
        throw std::runtime_error(std::strerror(errno));

    return container;

}

bool ProcessParser::isPidExisting(string pid){
    bool result = false;
    vector<string>_list = ProcessParser::getPidList();

    for (int i=0; i<_list.size(); i++){
        if(pid == _list[i]){
            result = true;
            break;
        }
    }

    return result;  
}
