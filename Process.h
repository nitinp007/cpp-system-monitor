#include <string>

using namespace std;
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
private:
    string pid;
    string user;
    string cmd;
    string cpu;
    string mem;
    string upTime;

public:
    Process(string pid){
        this->pid = pid;
        this->user = ProcessParser::getProcUser(pid);
        //std::cout<<"user is: "<<this->user<<std::endl;
        //TODOs:
        //complete for mem
        //complete for cmd
        //complete for upTime
        //complete for cpu
        this->mem = ProcessParser::getVmSize(pid);
        //std::cout<<"vmsize is: "<<this->mem<<std::endl;
        this->cmd = ProcessParser::getCmd(pid);
        //std::cout<<"cmd is: "<<this->cmd<<std::endl;
        this->upTime = ProcessParser::getProcUpTime(pid);
        //std::cout<<"uptime: "<<this->upTime<<std::endl;
        this->cpu = ProcessParser::getCpuPercent(pid);
    }
    void setPid(int pid);
    string getPid()const;
    string getUser()const;
    string getCmd()const;
    int getCpu()const;
    int getMem()const;
    string getUpTime()const;
    string getProcess();
};
void Process::setPid(int pid){
    this->pid = pid;
}
string Process::getPid()const {
    return this->pid;
}
string Process::getProcess(){
    //if(!ProcessParser::isPidExisting(this->pid))
      //  return "";
    this->mem = ProcessParser::getVmSize(this->pid);
    this->upTime = ProcessParser::getProcUpTime(this->pid);
    this->cpu = ProcessParser::getCpuPercent(this->pid);
    //std::cout<<"pid is:"<<this->pid<<std::endl; 
    //return (this->pid + "   " + //TODO: finish the string! this->user + "   "+ mem...cpu...upTime...;
    return (this->pid + "   "
            + this->user
            + "   "
            + this->mem.substr(0,5)
            + "     "
            + this->cpu.substr(0,5)
            + "     "
            + this->upTime.substr(0,5)
            + "    "
            + this->cmd.substr(0,30)
            + "...");
}
