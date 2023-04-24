//#include "common/CoorBench.hh"
#include "common/Config.hh"
#include "protocol/CoorCommand.hh"

#include "inc/include.hh"
#include "util/RedisUtil.hh"
#include "util/DistUtil.hh"

using namespace std;

void usage() {
  cout << "       ./DistClient degradeRead blockname method" << endl;
  cout << "       ./DistClient nodeRepair ip code method" << endl;
}

void degradeRead_1(string blockname, int offset, int length, string method) {
    struct timeval time1, time2, time3;
    gettimeofday(&time1, NULL);

    string confpath("./conf/sysSetting.xml");
    Config* conf = new Config(confpath);
    
    CoorCommand* cmd = new CoorCommand();
    cmd->buildType41(41, conf->_localIp, blockname, offset, length, method);
    cmd->dump();
    cmd->sendTo(conf->_coorIp);
    
    cout << " degradeRead_1 111" << endl;
    delete cmd;

    // wait for finish flag?
    redisContext* waitCtx = RedisUtil::createContext(conf->_localIp);
    string wkey = "writefinish:"+blockname;
    redisReply* fReply = (redisReply*)redisCommand(waitCtx, "blpop %s 0", wkey.c_str());
    cout << "222" << endl;
    freeReplyObject(fReply);
    redisFree(waitCtx);

    gettimeofday(&time2, NULL);
    cout << "repairBlock::repair time: " << DistUtil::duration(time1, time2) << endl;

    delete conf;
}

void degradeRead(string blockname,string method) {
    struct timeval time1, time2, time3;
    gettimeofday(&time1, NULL);

    string confpath("./conf/sysSetting.xml");
    Config* conf = new Config(confpath);
    
    CoorCommand* cmd = new CoorCommand();
    cout <<  RedisUtil::ip2Str(conf->_localIp) << endl;
    cout << blockname << endl;
    cout << method << endl;
    cmd->buildType4(4, conf->_localIp, blockname, method);
    cmd->sendTo(conf->_coorIp);
    
    cout << "111" << endl;
    delete cmd;

    // wait for finish flag?
    redisContext* waitCtx = RedisUtil::createContext(conf->_localIp);
    string wkey = "writefinish:"+blockname;
    redisReply* fReply = (redisReply*)redisCommand(waitCtx, "blpop %s 0", wkey.c_str());
    cout << "222" << endl;
    freeReplyObject(fReply);
    redisFree(waitCtx);

    gettimeofday(&time2, NULL);
    cout << "repairBlock::repair time: " << DistUtil::duration(time1, time2) << endl;

    delete conf;
}

void standbyRepair(string nodeipstr, string code, string method) {

  string confpath("./conf/sysSetting.xml");
  Config* conf = new Config(confpath);

  unsigned int ip = inet_addr(nodeipstr.c_str());

  CoorCommand* cmd = new CoorCommand();
  cmd->buildType3(3, conf->_localIp, ip, code, method);
  cmd->sendTo(conf->_coorIp);

  delete cmd;
  delete conf;
}
// degradeRead stripe-0-0 byteFrom byteTo dist

int main(int argc, char** argv) {
    
    if (argc < 2) {
        usage();
        return -1;
    }
    
    string reqType(argv[1]);
    if (reqType == "degradeRead") {     
        if (argc != 4) {
            usage();
            return -1;
        }      
        string blockname(argv[2]);
        string method(argv[3]);
        degradeRead(blockname,method);
    } else if (reqType == "degradeRead1"){
        if (argc != 6) {
            usage();
            return -1;
        }     
        string blockname(argv[2]);
        int offset = atoi(argv[3]);  
        int length = atoi(argv[4]);  
        string method(argv[5]);
        degradeRead_1(blockname, offset, length, method);
    } else if (reqType == "nodeRepair") {
        if (argc != 5) {
            usage();
            return -1;
        }

        string nodeip(argv[2]);
        string code(argv[3]);
        string method(argv[4]);
        standbyRepair(nodeip, code, method);
    }

  return 0;
}
