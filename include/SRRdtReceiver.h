#ifndef SR_RDT_RECEIVER_H
#define SR_RDT_RECEIVER_H
#include "RdtReceiver.h"
#define WINDOW_SIZE 4
#define MAX_SEQ_NUM 8
#define ACKED 0
#define NACKED -1
class SRRdtReceiver :public RdtReceiver
{
private:
    int baseSeqNum; // 接收窗口基序号
    Packet lastAckPkt; //上次发送的确认报文
    Packet pktsWaitingDeliver[MAX_SEQ_NUM];

public:
    SRRdtReceiver();
    virtual ~SRRdtReceiver();
    inline bool isInWindow(int seqNum);
    void printWindow();

public:
    
    void receive(const Packet &packet);	//接收报文，将被NetworkService调用
};

#endif

