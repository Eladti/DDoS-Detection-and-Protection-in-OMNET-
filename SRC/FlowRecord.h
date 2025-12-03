#ifndef __FLOWRECORD_H
#define __FLOWRECORD_H

#include <inet/networklayer/common/L3Address.h>

struct FlowRecord {
    inet::L3Address srcAddr;
    inet::L3Address destAddr;
    int protocol;
    int srcPort;
    int destPort;
    int packetCount;
    int byteCount;
    double startTime;
    double endTime;
};

#endif
