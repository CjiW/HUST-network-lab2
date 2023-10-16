#include "Global.h"
#include "TCPRdtReceiver.h"

TCPRdtReceiver::TCPRdtReceiver()
{
    baseSeqNum = 0;
    for (int i = 0; i < MAX_SEQ_NUM; i++) {
        pktsWaitingDeliver[i].acknum = NACKED;
    }
    lastAckPkt.acknum = 0;
    lastAckPkt.checksum = 0;
    lastAckPkt.seqnum = -1;
    for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
        lastAckPkt.payload[i] = '.';
    }
}

TCPRdtReceiver::~TCPRdtReceiver()
{
}

inline bool TCPRdtReceiver::isInWindow(int seqNum)
{
    return (seqNum + MAX_SEQ_NUM - baseSeqNum) % MAX_SEQ_NUM < WINDOW_SIZE;
}

void TCPRdtReceiver::printWindow()
{
    fprintf(RLOG, "| ");
    for (int i = 0; i < WINDOW_SIZE; i++) {
        fprintf(RLOG, "%d ", (baseSeqNum + i) % MAX_SEQ_NUM);
    }
    fprintf(RLOG, "|\n| ");
    char c;
    for (int i = 0; i < WINDOW_SIZE; i++) {
        c = pktsWaitingDeliver[(baseSeqNum + i) % MAX_SEQ_NUM].acknum == ACKED ? '+' : ' ';
        fprintf(RLOG, "%c ", c);
    }
    fprintf(RLOG, "|\n\n");
}

void TCPRdtReceiver::receive(const Packet &packet)
{
    int checkSum = pUtils->calculateCheckSum(packet);
    if (checkSum == packet.checksum ) {
        // pUtils->printPacket("接收方正确收到发送方的报文", packet);
        fprintf(RLOG, "RECV(%d)\n", packet.seqnum);
        Packet *pkt = pktsWaitingDeliver + packet.seqnum;
        if (isInWindow(packet.seqnum)&&pkt->acknum!=ACKED){
            *pkt = packet;
            pkt->acknum=ACKED;
            if (packet.seqnum == baseSeqNum){
                while(pkt->acknum==ACKED) {
                    lastAckPkt.acknum=(lastAckPkt.acknum+1)%MAX_SEQ_NUM;
                    pkt->acknum=NACKED;
                    Message msg;
                    memcpy(msg.data, pkt->payload, sizeof(pkt->payload));
                    pns->delivertoAppLayer(RECEIVER, msg);
                    // pUtils->printPacket("接收方上交数据", *pkt);
                    baseSeqNum = (baseSeqNum + 1) % MAX_SEQ_NUM;
                    pkt = pktsWaitingDeliver + baseSeqNum;
                }
            }
            printWindow();
        }
        lastAckPkt.checksum = 0;
        lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
        // pUtils->printPacket("接收方发送确认报文", lastAckPkt);
        pns->sendToNetworkLayer(SENDER, lastAckPkt);
    }
}
