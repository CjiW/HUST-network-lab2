/**
 * 报文段格式、接收方缓冲区大小和 GBN 协议一样保持不变；
 * @ 报文段序号按照报文段为单位进行编号；
 * @ 单一的超时计时器，不需要估算 RTT 动态调整定时器 Timeout 参数；
 * @ 支持快速重传和超时重传，重传时只重传最早发送且没被确认的报文段；
 * @ 确认号为收到的最后一个报文段序号；
 * @ 不考虑流量控制、拥塞控制。
*/
#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "TCPRdtSender.h"
#include "TCPRdtReceiver.h"
FILE *LOG;

int main(int argc, char* argv[])
{
    if (argc != 3) {
        printf("Usage: %s <inputfile> <outputfile>\n", argv[0]);
        return 0;
    }
    LOG = fopen("tcp_log.txt", "w+");

    RdtSender *ps = new TCPRdtSender();
    RdtReceiver * pr = new TCPRdtReceiver();
    pns->setRunMode(0);  //VERBOS模式
//	pns->setRunMode(1);  //安静模式
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

