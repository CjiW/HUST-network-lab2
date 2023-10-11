#include "Global.h"
#include "GBNRdtSender.h"

bool GBNRdtSender::getWaitingState()
{
    return waitingState;
}

bool GBNRdtSender::send(const Message &message)
{
    if (waitingState) { return false; }
    Packet *pkt = pktsWaitingAck+nextSeqNum;
    pkt->checksum = 0;
    memcpy(pkt->payload, message.data, sizeof(message.data));
    pkt->checksum = pUtils->calculateCheckSum(*pkt);
    pUtils->printPacket("发送方发送报文", *pkt);
    pns->sendToNetworkLayer(RECEIVER, *pkt);
    // 第一次发送，启动计时器
    if (baseSeqNum == nextSeqNum) {
        pns->startTimer(SENDER, Configuration::TIME_OUT, baseSeqNum);
    }
    // 更新nextSeqNum
    nextSeqNum = (nextSeqNum + 1) % MAX_SEQ_NUM;
    if (nextSeqNum == (baseSeqNum + WINDOW_SIZE) % MAX_SEQ_NUM) {
        // 发送窗口已满，等待确认
        waitingState = true;
    }
    return true;
}

void GBNRdtSender::receive(const Packet &ackPkt)
{
    // 检查校验和
    int checkSum = pUtils->calculateCheckSum(ackPkt);
    if(checkSum == ackPkt.checksum){
        if(1 == (baseSeqNum + MAX_SEQ_NUM - ackPkt.acknum) % MAX_SEQ_NUM) {
            // 确认上一次确认的分组，不做处理
            pUtils->printPacket("发送方收到重复确认，窗口不变", ackPkt);
            return;
        }
        pUtils->printPacket("发送方收到确认，窗口更改", ackPkt);
        // 停止旧计时器
        pns->stopTimer(SENDER, baseSeqNum);
        // 更新base
        baseSeqNum = (ackPkt.acknum + 1) % MAX_SEQ_NUM;
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

void GBNRdtSender::timeoutHandler(int seqNum)
{
    // 重发窗口中所有未确认的分组，从base开始到nextSeqNum - 1
    resend();
}

void GBNRdtSender::resend()
{
    int seqnum = baseSeqNum;
    pns->stopTimer(SENDER, seqnum);
    pns->startTimer(SENDER, Configuration::TIME_OUT, seqnum);
    while (seqnum != nextSeqNum){
        Packet *pkt = pktsWaitingAck+seqnum;
        pns->sendToNetworkLayer(RECEIVER, *pkt);
        pUtils->printPacket("发送方定时器时间到，重发报文", *pkt);
        seqnum = (seqnum + 1) % MAX_SEQ_NUM;
    }
}

GBNRdtSender::GBNRdtSender()
{
    baseSeqNum = 0;
    nextSeqNum = 0;
    for(int i=0; i<MAX_SEQ_NUM;i++){
        pktsWaitingAck[i].acknum = -1;
        pktsWaitingAck[i].seqnum = i;
    }
    waitingState = false;
}

GBNRdtSender::~GBNRdtSender()
{
}
