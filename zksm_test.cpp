#include "ZKServiceMonitor.hpp"
#include <unistd.h>
int main(int argc,char **argv){

	if (argc != 3) {
		cerr << "Usage: zk_test is a demo that show usage of Zookeeper.hpp\n"
			 << "\tzk_test -help\t\t#print help info.\n"
			 << "\tzk_test <constr> <znode>\t\t#show usage of APIS:  exists, getChildren, delete, create & etc.\n";
		return 1;
	}

	string connstr(argv[1]);
	string znode(argv[2]);
    ServiceMonitor zksm(connstr,30000,znode);
    while(true){
        zksm.registered();
        
        string znode;
        while((znode=zksm.pop())!=""){
            LOGi("zksm.pop() =>%s",znode.c_str());
        }
        sleep(10);
    }
}
