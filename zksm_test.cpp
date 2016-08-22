#include "ZKServiceMonitor.hpp"
#include <unistd.h>
int main(int argc,char **argv){
    string baseNode="/hbserver";
    string conStr="localhost:2181";
    ServiceMonitor zksm(conStr,30000,baseNode);
    while(true){
        zksm.registered();
        
        string znode;
        while((znode=zksm.pop())!=""){
            LOGi("zksm.pop() =>%s",znode.c_str());
        }
        sleep(10);
    }
}
