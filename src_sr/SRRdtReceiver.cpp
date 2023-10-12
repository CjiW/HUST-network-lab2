#include "Global.h"
#include "SRRdtReceiver.h"

SRRdtReceiver::SRRdtReceiver()
{
    baseSeqNum = 0;
    for (int i = 0; i < MAX_SEQ_NUM; i++) {
        pktsWaitingDeliver[i].acknum = NACKED;
    }
    lastAckPkt.acknum = -1;
    lastAckPkt.checksum = 0;
    lastAckPkt.seqnum = -1;
    for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++) {
        lastAckPkt.payload[i] = '.';
    }
}

SRRdtReceiver::~SRRdtReceiver()
{
}

bool SRRdtReceiver::isInWindow(int seqNum)
{
    return (seqNum + MAX_SEQ_NUM - baseSeqNum) % MAX_SEQ_NUM < WINDOW_SIZE;
}

void SRRdtReceiver::receive(const Packet &packet)
{
    int checkSum = pUtils->calculateCheckSum(packet);
    if (checkSum == packet.checksum ) {
        // pUtils->printPacket("接收方正确收到发送方的报文", packet);
        Packet *pkt = pktsWaitingDeliver + packet.seqnum;
        if (isInWindow(packet.seqnum)&&pkt->acknum!=ACKED){
            *pkt = packet;
            // pUtils->printPacket("pkt", *pkt);
            // pUtils->printPacket("packet", packet);
            pkt->acknum=ACKED;
            if (packet.seqnum == baseSeqNum){
                while(pkt->acknum==ACKED) {
                    pkt->acknum=NACKED;
                    Message msg;
                    memcpy(msg.data, pkt->payload, sizeof(pkt->payload));
                    pns->delivertoAppLayer(RECEIVER, msg);
                    // pUtils->printPacket("接收方上交数据", *pkt);
                    baseSeqNum = (baseSeqNum + 1) % MAX_SEQ_NUM;
                    pkt = pktsWaitingDeliver + baseSeqNum;
                }
            }
        }
        lastAckPkt.checksum = 0;
        lastAckPkt.acknum = packet.seqnum;
        lastAckPkt.checksum = pUtils->calculateCheckSum(lastAckPkt);
        // pUtils->printPacket("接收方发送确认报文", lastAckPkt);
        pns->sendToNetworkLayer(SENDER, lastAckPkt);
    }
}
