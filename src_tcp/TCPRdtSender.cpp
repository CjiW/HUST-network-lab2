#include "Global.h"
#include "TCPRdtSender.h"

bool TCPRdtSender::getWaitingState()
{
    return waitingState;
}

bool TCPRdtSender::send(const Message &message)
{
    if (waitingState) { return false; }
    Packet *pkt = pktsWaitingAck+nextSeqNum;
    pkt->checksum = 0;
    pkt->acknum = -1;
    memcpy(pkt->payload, message.data, sizeof(message.data));
    pkt->checksum = pUtils->calculateCheckSum(*pkt);
    pUtils->printPacket("发送方发送报文", *pkt);
    pns->sendToNetworkLayer(RECEIVER, *pkt);
    // 确认次数置0
    ackCount = 0;
    // 第一次发送，启动计时器
    if (baseSeqNum == nextSeqNum) {
        pns->startTimer(SENDER, Configuration::TIME_OUT, baseSeqNum);
    }
    // 更新nextSeqNum
    nextSeqNum = (nextSeqNum + 1) % MAX_SEQ_NUM;
    if ((nextSeqNum + MAX_SEQ_NUM - baseSeqNum) % MAX_SEQ_NUM >= WINDOW_SIZE) {
        waitingState = true;
    }
    return true;
}

void TCPRdtSender::receive(const Packet &ackPkt)
{
    // 检查校验和
    int checkSum = pUtils->calculateCheckSum(ackPkt);
    if(checkSum == ackPkt.checksum){
        if(ackPkt.acknum == baseSeqNum) {
            // 确认上一次确认的分组，计数器加1
            pktsWaitingAck[baseSeqNum].acknum++;
            if(ackCount == 4){
                // 重发
                resend();
            }
            return;
        }
        pUtils->printPacket("发送方收到确认，窗口更改", ackPkt);
        // 停止旧计时器
        pns->stopTimer(SENDER, baseSeqNum);
        // 更新base
        baseSeqNum = ackPkt.acknum % MAX_SEQ_NUM;
        // 确认次数置0
        ackCount = 0;
        if(baseSeqNum != nextSeqNum){// 不空
            // 重启新计时器
            pns->startTimer(SENDER, Configuration::TIME_OUT, baseSeqNum);
        }
        // 发送窗口中有空闲，更新waitingState
        waitingState = false;
    }else{
        pUtils->printPacket("发送方没有正确收到确认报文,数据校验错误", ackPkt);
    }
}

void TCPRdtSender::timeoutHandler(int seqNum)
{
    // 重发窗口中所有未确认的分组，从base开始到nextSeqNum - 1
    resend();
}

void TCPRdtSender::resend()
{
    Packet *pkt = pktsWaitingAck+baseSeqNum;
    // 重置计时器
    pns->stopTimer(SENDER, baseSeqNum);
    pns->startTimer(SENDER, Configuration::TIME_OUT, baseSeqNum);
    // 重置确认次数
    ackCount = 0;
    pns->sendToNetworkLayer(RECEIVER, *pkt);
}

TCPRdtSender::TCPRdtSender()
{
    ackCount = 0;
    baseSeqNum = 0;
    nextSeqNum = 0;
    for(int i=0; i<MAX_SEQ_NUM;i++){
        pktsWaitingAck[i].seqnum = i;
    }
    waitingState = false;
}

TCPRdtSender::~TCPRdtSender()
{
}
