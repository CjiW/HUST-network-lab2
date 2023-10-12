#ifndef SR_RDT_SENDER_H
#define SR_RDT_SENDER_H
#include "RdtSender.h"
#define WINDOW_SIZE 4
// 序号空间为窗口大小 * 2
#define MAX_SEQ_NUM 8
#define NACKED -1
#define ACKED 0

class SRRdtSender :public RdtSender
{
private:
    // 窗口基序号
    int baseSeqNum;
    // 下一个发送序号
    int nextSeqNum;
    // 等待确认的分组
    Packet pktsWaitingAck[MAX_SEQ_NUM];

    bool waitingState;				// 是否处于满状态

public:

    bool getWaitingState();
    bool send(const Message &message);						//发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
    void receive(const Packet &ackPkt);						//接受确认Ack，将被NetworkServiceSimulator调用	
    void timeoutHandler(int seqNum);
    bool isWaiting(int seqNum);
public:
    SRRdtSender();
    virtual ~SRRdtSender();
};

#endif