#include "Global.h"
#include "SRRdtSender.h"

bool SRRdtSender::getWaitingState()
{
    return waitingState;
}

bool SRRdtSender::send(const Message &message)
{
    if (waitingState) { return false; }
    Packet *pkt = pktsWaitingAck+nextSeqNum;
    pkt->checksum = 0;
    pkt->acknum=NACKED;
    memcpy(pkt->payload, message.data, sizeof(message.data));
    pkt->checksum = pUtils->calculateCheckSum(*pkt);
    // pUtils->printPacket("发送方发送报文", *pkt);
    pns->sendToNetworkLayer(RECEIVER, *pkt);
    // 每次发送都启动计时器
    pns->startTimer(SENDER, Configuration::TIME_OUT, nextSeqNum);
    fprintf(SLOG, "SEND(%d)\n", pkt->seqnum);
    // 更新nextSeqNum
    nextSeqNum = (nextSeqNum + 1) % MAX_SEQ_NUM;
    printWindow();
    // 满状态
    if ((nextSeqNum + MAX_SEQ_NUM - baseSeqNum) % MAX_SEQ_NUM >= WINDOW_SIZE) {
        waitingState = true;
    }
    return true;
}

void SRRdtSender::receive(const Packet &ackPkt)
{
    int checkSum = pUtils->calculateCheckSum(ackPkt);
    // pUtils->printPacket("发送方正确收到确认", ackPkt);
    if (checkSum == ackPkt.checksum && isWaiting(ackPkt.acknum)) {
        fprintf(SLOG, "ACK(%d)\n", ackPkt.acknum);
        Packet *pkt = pktsWaitingAck + ackPkt.acknum;
        pkt->acknum = ACKED;
        pns->stopTimer(SENDER, ackPkt.acknum);
        if (ackPkt.acknum == baseSeqNum) {
            while (pkt->acknum==ACKED) {
                pkt->acknum=NACKED;
                baseSeqNum = (baseSeqNum + 1) % MAX_SEQ_NUM;
                pkt = pktsWaitingAck + baseSeqNum;
            }
            waitingState = false;
        }
        printWindow();
    }
}

void SRRdtSender::timeoutHandler(int seqNum)
{
    // 超时重发一个分组
    Packet *pkt = pktsWaitingAck+seqNum;
    pns->stopTimer(SENDER, seqNum);
    // pUtils->printPacket("发送方超时重发报文", *pkt);
    fprintf(SLOG, "TIMEOUT(%d)\nRESEND(%d)\n", seqNum, seqNum);
    pns->sendToNetworkLayer(RECEIVER, *pkt);
    pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
    fputc('\n', SLOG);
}

inline bool SRRdtSender::isWaiting(int seqNum)
{
    return (seqNum - baseSeqNum + MAX_SEQ_NUM) % MAX_SEQ_NUM < (nextSeqNum - baseSeqNum + MAX_SEQ_NUM) % MAX_SEQ_NUM;
}

void SRRdtSender::printWindow()
{
    // |1 2 3 4|
    // |* +    | 1 已发送 2 已确认，3 4未发送
    fprintf(SLOG, "| ");
    for(int i=0; i<WINDOW_SIZE; i++){
        fprintf(SLOG, "%d ", (baseSeqNum + i) % MAX_SEQ_NUM);
    }
    fprintf(SLOG, "|\n| ");
    char c;
    int seqNum;
    for(int i=0; i<WINDOW_SIZE; i++){
        seqNum = (baseSeqNum + i) % MAX_SEQ_NUM;
        c = (isWaiting(seqNum))? (pktsWaitingAck[seqNum].acknum == ACKED)? '+' : '*' : ' ';
        fprintf(SLOG, "%c ", c);
    }
    fprintf(SLOG, "|\n\n");
}

SRRdtSender::SRRdtSender()
{
    baseSeqNum = 0;
    nextSeqNum = 0;
    waitingState = false;
    for (int i = 0; i < MAX_SEQ_NUM; i++) {
        pktsWaitingAck[i].seqnum = i;
        pktsWaitingAck[i].acknum = NACKED;
    }
}

SRRdtSender::~SRRdtSender()
{
}
