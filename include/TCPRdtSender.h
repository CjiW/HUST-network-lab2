#ifndef TCP_RDT_SENDER_H
#define TCP_RDT_SENDER_H
#include "RdtSender.h"
#define WINDOW_SIZE 4
#define MAX_SEQ_NUM 8
class TCPRdtSender :public RdtSender
{
private:
    // 窗口基序号
    int baseSeqNum;
    // 下一个发送序号
    int nextSeqNum;
    // 等待确认的分组
    Packet pktsWaitingAck[MAX_SEQ_NUM];

    int ackCount;					// 重复确认计数

    bool waitingState;				// 是否处于满状态

public:

    bool getWaitingState();
    bool send(const Message &message);						//发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
    void receive(const Packet &ackPkt);						//接受确认Ack，将被NetworkServiceSimulator调用	
    void timeoutHandler(int seqNum);					//Timeout handler，将被NetworkServiceSimulator调用
    // 重传分组
    void resend();
public:
    TCPRdtSender();
    virtual ~TCPRdtSender();
};

#endif

