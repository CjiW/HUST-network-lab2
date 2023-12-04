// StopWait.cpp : 定义控制台应用程序的入口点。
//


#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "GBNRdtSender.h"
#include "GBNRdtReceiver.h"
FILE *LOG;

int main(int argc, char* argv[])
{
    if (argc != 3) {
        printf("Usage: %s <inputfile> <outputfile>\n", argv[0]);
        return 0;
    }
    LOG = fopen("gbn_log.txt", "w+");
    RdtSender *ps = new GBNRdtSender();
    RdtReceiver * pr = new GBNRdtReceiver();
    // pns->setRunMode(0);  //VERBOS模式
	pns->setRunMode(1);  //安静模式
    pns->init();
    pns->setRtdSender(ps);
    pns->setRtdReceiver(pr);
    pns->setInputFile(argv[1]);
    pns->setOutputFile(argv[2]);

    pns->start();

    delete ps;
    delete pr;
    delete pUtils;									//指向唯一的工具类实例，只在main函数结束前delete
    delete pns;										//指向唯一的模拟网络环境类实例，只在main函数结束前delete
    
    return 0;
}

