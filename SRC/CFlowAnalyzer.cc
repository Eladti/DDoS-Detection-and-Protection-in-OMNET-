#include "CFlowAnalyzer.h"
#include "FlowRecord.h"
#include <omnetpp.h>
#include <sstream>
#include <algorithm>
#include <vector>

using namespace omnetpp;
using namespace std;

Define_Module(CFlowAnalyzer);

void CFlowAnalyzer::initialize() {
    portPacketThreshold = par("portPacketThreshold").intValue();
    attackFlowSignal = registerSignal("attackFlow");
}

void CFlowAnalyzer::handleMessage(cMessage *msg) {
    EV_INFO << "[CFlowAnalyzer] handleMessage @ t=" << simTime()
            << ", msg=" << msg->getName()
            << ", contextPtr=" << msg->getContextPointer() << "\n";

    auto *recordsPtr = static_cast<std::vector<FlowRecord>*>(msg->getContextPointer());
    if (!recordsPtr) {
        EV_WARN << "[CFlowAnalyzer] bad records pointer, deleting msg\n";
        delete msg;
        return;
    }
    analyze(*recordsPtr);
    delete recordsPtr;
    delete msg;
}

void CFlowAnalyzer::analyze(const std::vector<FlowRecord> &records) {
    static int goodClientPort = -1;
    EV_INFO << "[CFlowAnalyzer] [analyze] Analyzing " << records.size() << " flows for window\n";

    if (!baselineEstablished && !records.empty()) {
        int maxPort = -1, maxCount = -1;
        for (const auto& fr : records) {
            if (fr.packetCount > maxCount) {
                maxCount = fr.packetCount;
                maxPort = fr.srcPort;
            }
        }
        if (maxPort != -1) {
            goodClientPort = maxPort;
            EV_INFO << "[CFlowAnalyzer] Marked port " << goodClientPort << " as good client (never block)\n";
        }
        baselineEstablished = true;
        return;
    }

    bool attackFound = false;
    for (const auto& fr : records) {
        if (fr.srcPort == goodClientPort) {
            EV_INFO << "[CFlowAnalyzer] Port " << fr.srcPort << " is the good client, never block!\n";
            continue;
        }
        if (fr.packetCount >= portPacketThreshold) {
            EV_INFO << "[CFlowAnalyzer] Port " << fr.srcPort << " flagged as attacker\n";
            int floodingPort = fr.srcPort;
            auto *blockMsg = new cMessage("blockPort");
            blockMsg->addPar("blockPort") = floodingPort;
            send(blockMsg, "outToRouter");

            EV_INFO << "[CFlowAnalyzer] Port " << fr.srcPort << " sent " << fr.packetCount << " packets";
            emit(attackFlowSignal, fr.srcPort);
            attackFound = true;
        }
    }

    if (attackFound) {
        EV_INFO << "[CFlowAnalyzer] Detected attack(s) in this window, sending attackConfirmed\n";
        send(new cMessage("attackConfirmed"), "outToRouter");
    } else {
        EV_INFO << "[CFlowAnalyzer] No port exceeded threshold this window\n";
    }
}
