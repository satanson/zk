#ifndef _ZOOKEEPER_HPP_
#define _ZOOKEEPER_HPP_

extern "C" {
    #include <zookeeper/zookeeper.h>
    #include <zookeeper/proto.h>
}

#include <exception>
#include <string>
#include <list>
#include <vector>
#include <sstream>
#include <iostream>
#include <memory>

#include <cassert>
#include <cstdarg>
#include <cerrno>

//extern struct ACL_vector ZOO_OPEN_ACL_UNSAFE;
extern int errno;

#define ZOOKEEPER_NAMESPACE_BEGIN namespace org { namespace apache { namespace zookeeper {
#define ZOOKEEPER_NAMESPACE_END   }}}
#define LOG_NAMESPACE_BEGIN namespace Log {
#define LOG_NAMESPACE_END   }

ZOOKEEPER_NAMESPACE_BEGIN

using namespace std;

template <typename def,typename type = typename def::type>
class safe_enum:public def{
    type val;
public:
    //crafty implicit conversion
    safe_enum(type v):val(v){}
    type value()const{return val;}

    safe_enum(safe_enum const& other):val(other.val){}
    safe_enum& operator=(safe_enum const& other){
        this->val = other.val;
        return *this; 
    }
    bool operator == (safe_enum const& s)const {
        return this->val == s.val;
    }
    bool operator < (safe_enum const& s)const{
        return this->val < s.val;
    }
    bool operator <= (safe_enum const& s)const{
        return *this<s || *this==s;
    }
    bool operator > (safe_enum const& s) const{
        return !(*this<=s);
    }
    bool operator >=(safe_enum const& s) const{
        return !(*this<s);
    }
    bool operator !=(safe_enum const& s) const{
        return !(*this==s);
    }
};

LOG_NAMESPACE_BEGIN

struct LogLevel_Def{
    enum type{
        INFO,
        WARN,
        ERROR,
        ASSERT,
        DEBUG,
        EXCEPTION,
        dummy_HDKHKLFHRJKGFGFWWXZZJLGWWIOJJVX
    };
};
typedef safe_enum<LogLevel_Def> LogLevel;    

static string timestamp(){
    time_t t;
    struct tm tm0;
    char buff[64];
    time(&t);
    size_t n=strftime(buff,64,"%Y-%m-%d %T",localtime_r(&t,&tm0));
    buff[n]=0;
    return string(buff);
}
string vformat(string const&,va_list);
string format(string const&,...);
void vlog(ostream&, LogLevel, const char*, const int,
        const char*,const char* fmt, va_list ap);
void log(ostream&, LogLevel, const char*, const int,
        const char*, const char*, ...);

#define LOGi(msg,...)\
    do{\
        log(cerr,LogLevel::INFO,__FILE__,__LINE__,__FUNCTION__,(msg),##__VA_ARGS__);\
    }while(0);

#define LOGw(msg,...)\
    do{\
        log(cerr,LogLevel::WARN,__FILE__,__LINE__,__FUNCTION__,(msg),##__VA_ARGS__);\
    }while(0);

#define LOGe(msg,...)\
    do{\
        log(cerr,LogLevel::ERROR,__FILE__,__LINE__,__FUNCTION__,(msg),##__VA_ARGS__);\
    }while(0);

#define LOGa(boolexpr,msg,...)\
    do{\
        log(cerr,LogLevel::ASSERT,__FILE__,__LINE__,__FUNCTION__,(msg),##__VA_ARGS__);\
        assert(boolexpr);\
    }while(0);

#ifdef DEBUG
#define LOGd(msg,...)\
    do{\
        log(cerr,LogLevel::DEBUG,__FILE__,__LINE__,__FUNCTION__,(msg),##__VA_ARGS__);\
    }while(0);
#else
#define LOGd(msg,...)do{}while(0);
#endif

#define THROW(msg,...)\
    do{\
        stringstream out;\
        log(out,LogLevel::EXCEPTION,__FILE__,__LINE__,__FUNCTION__,(msg),##__VA_ARGS__);\
        throw out.str();\
    }while(0);

template<typename T>
string to_string(T const& a){
    return ((stringstream&)(stringstream()<<a)).str();
}

string format(const string& fmt,...){
    va_list ap;
    va_start(ap,fmt);
    string s=vformat(fmt,ap);
    va_end(ap);
    return s;
}

string vformat(const string& fmt,va_list ap){
    string s(256,0);
    if(vsnprintf((char*)&*s.begin(),s.size(),fmt.c_str(),ap)<0){
        THROW("failed to format s string");
    }
    s.resize(s.size()-1);
    return s;
}

inline void log(ostream& out, LogLevel level, const char* file,
        int line, const char* func, const char* fmt,...){
    va_list ap;
    va_start(ap,fmt);
    vlog(out,level,file,line,func,fmt,ap);
    va_end(ap);
}

inline void vlog(ostream& out, LogLevel level, const char* file,
        const int line, const char* func, const char* fmt, va_list ap)
{
    const char* logs[]={"INFO","WARN","ERROR","ASSERT","DEBUG","EXCEPTION"};
    stringstream pack;
    string s;
    string msg=vformat(fmt,ap);
    pack<<timestamp()<<" "
        <<logs[level.value()]
        <<" ["<<file<<"] "
        <<func<<"#"<<line<<": "
        <<msg<<endl;
    out<<pack.str();
}
LOG_NAMESPACE_END

using namespace Log;

typedef void (*watch_fn)(zhandle_t*,int,int,const char*,void*);
class ZooKeeper;
struct WatchedEvent;
class Watcher;
static void watch_process(zhandle_t*,int,int, const char*,void*);

struct CreateMode_Def{
    enum type{
        PERSISTENT=0,
        EPHEMERAL=1,
        PERSISTENT_SEQUENTIAL=2,
        EPHEMERAL_SEQUENTIAL=3,
        dummy_PBDFDXBXZWJNVFFJHCGCSSHVVXAAQW
    };
};

struct KeeperState_Def{
    enum type{
        Expired = -112,
        AuthFailed = -113,
        Connecting = 1,
        Associating = 2,
        Connected = 3,
        NotConnected = 999,
        dummy_LHKJITJKGGFHKJFDDHJKGFRDVHHCR
    };
};

struct EventType_Def{
    enum type{
        NodeCreated = 1,
        NodeDeleted = 2,
        NodeDataChanged = 3,
        NodeChildrenChanged = 4,
        Session = -1,
        NotWatching = -2,
        dummy_BFJHVLKCCKFBDSSSLKBCSGFNBXXCT
    };
};

typedef safe_enum<CreateMode_Def> CreateMode;
typedef safe_enum<KeeperState_Def> KeeperState;
typedef safe_enum<EventType_Def> EventType;

struct WatchedEvent{
    WatchedEvent(string const& p,int ks,int et):
        path(p),keeperState(ks),eventType(et){}

    string path;
    int keeperState;
    int eventType;
};

class Watcher{
private:
    ZooKeeper *zookeeper;
public:
    Watcher(){}
    explicit Watcher(ZooKeeper* zk):zookeeper(zk){}
    virtual ~Watcher() {}
    ZooKeeper* getZooKeeper()const{
        return zookeeper;
    }
    void setZooKeeper(ZooKeeper *zk){   
        zookeeper = zk;
    }
    virtual void process(const WatchedEvent& event){
        LOGi("Events are processed by default watcher");
    }
};

class ZooKeeper{
public:
    void watch_callback(const WatchedEvent& event){
        LOGi("zk_watcher->process(event)");
        zk_watcher->process(event);
    }

private:
    zhandle_t* zk_handle;
    clientid_t zk_clientid;
    shared_ptr<Watcher> zk_watcher;

public:
    typedef uint8_t byte_t;
    typedef long long int64_t;
    typedef void (*watch_fn)(zhandle_t*,int,int,const char*,void*);

    ZooKeeper(string const& connStr,int recv_timeout,
            shared_ptr<Watcher> watcher,int64_t sessionId,
            string const& passwd, int flags){

        zoo_set_debug_level(ZOO_LOG_LEVEL_ERROR);
        zk_clientid.client_id=sessionId;

        size_t passwd_len=sizeof(zk_clientid.passwd);
        //cout<<"passwd_len="<<passwd_len<<endl;
        //cout<<"passwd_size()"<<passwd.size()<<endl;
        assert(passwd.size()==passwd_len);
        std::copy(passwd.begin(),passwd.end(),zk_clientid.passwd);
        //LOGi("passwd_len=%u",passwd_len);
        //LOGi("passwd_size()=%u",passwd.size());

        zk_handle = zookeeper_init(connStr.c_str(), watch_process, recv_timeout,
                &zk_clientid, NULL, flags);
        if (!zk_handle){
            THROW(zerror(errno));
        }
        zk_watcher=watcher;
        zk_watcher->setZooKeeper(this);
        zoo_set_context(zk_handle,this);
        LOGi("Succeeded in connecting to ZK specified by %s",connStr.c_str());
    }

    ZooKeeper(string const& connStr, int recv_timeout, shared_ptr<Watcher> watcher,
            int64_t sessionId, string const& passwd):
        ZooKeeper(connStr,recv_timeout,watcher,sessionId,passwd, 0){}

    ZooKeeper(string const& connStr, int recv_timeout, shared_ptr<Watcher> watcher, int flags):
        ZooKeeper(connStr,recv_timeout,watcher,0,string(16,'0'), 0){}

    ZooKeeper(string const& connStr, int recv_timeout,shared_ptr<Watcher> watcher):
        ZooKeeper(connStr,recv_timeout, watcher, 0){}

    ~ZooKeeper(){
        zookeeper_close(zk_handle);
    }
    
    string zk_create(string const& path,string const& data,CreateMode createMode){
        char path_buff[256]={0};
        int rc=zoo_create(zk_handle, path.c_str(), data.c_str(), data.size(),
                &ZOO_OPEN_ACL_UNSAFE, createMode.value(),path_buff,256);
        if (rc!=ZOK){
            LOGe("Failed to create znode %s",path.c_str());
            THROW(zerror(rc));
        }
        LOGi("Succeeded in create zode %s",path.c_str());
        return string(path_buff);
    }
    
    void zk_delete(string const& path,int version){
        int rc=zoo_delete(zk_handle,path.c_str(),version);
        if (rc!=ZOK){
            LOGe("Failed to delete znode %s",path.c_str());
            THROW(zerror(rc));
        }
        LOGi("Succeeded in delete znode %s",path.c_str());
    }

    bool zk_exists(string const& path, bool watch){
        int rc=zoo_exists(zk_handle, path.c_str(), watch, NULL);
        if (rc==ZOK){
            LOGi("Znode %s already exists", path.c_str());
            return true;
        }else if (rc==ZNONODE){
            LOGi("Znode %s no exists yet", path.c_str());
            return false;
        }else {
            LOGe("Failed to invoke zk_exists");
            THROW(zerror(rc));
            return false;
        }
    }

    string zk_getData(string const& path, bool watch){
        char buff[256]={0};
        int bufflen=256;
        int rc=zoo_get(zk_handle,path.c_str(), watch, buff, &bufflen, NULL);
        if (rc!=ZOK){
            LOGe("Failed to get data of znode %s", path.c_str());
            THROW(zerror(rc));
        }
        LOGi("Succeeded in geting data of znode %s", path.c_str());
        string s(bufflen,'0');
        std::copy(buff,buff+bufflen,s.begin());
    }

    void zk_setData(string const& path, string const& data, int version){
        int rc=zoo_set(zk_handle,path.c_str(),data.c_str(),
                data.size(),version);
        if (rc!=ZOK){
            LOGe("Failed to set data of znode %s",path.c_str());
            THROW(zerror(rc));
        }
        LOGi("Succeeded in setting data of znode %s",path.c_str());
    }

    list<string> zk_getChildren(string const& path,bool watch){
        struct String_vector strings;
        int rc=zoo_get_children(zk_handle,path.c_str(),watch,&strings);
        if (rc!=ZOK) {
            LOGe("Failed to get children of znode %s",path.c_str());
            THROW(zerror(rc));
        }
        list<string> children;
        for (int i=0;i<strings.count;++i){
            children.push_back(strings.data[i]);
        }
        deallocate_String_vector(&strings);
        return children;
    }
};

static void watch_process(zhandle_t* zh,int type,int state,
        const char* path,void* zookeeper){
    LOGi("Event triggered");
    ZooKeeper& zk = *(ZooKeeper*)zookeeper;
    WatchedEvent event(path,state,type);
    zk.watch_callback(event);
}

ZOOKEEPER_NAMESPACE_END

#endif
