#include "CFlowCollector.h"
#include "FlowRecord.h"
#include <inet/common/packet/Packet.h>
#include <inet/networklayer/common/L3Address.h>
#include <omnetpp.h>
#include <map>
#include <vector>
using namespace omnetpp;
using namespace inet;

Define_Module(CFlowCollector);

CFlowCollector::CFlowCollector() : stopEvent(nullptr), collecting(false) {}

CFlowCollector::~CFlowCollector() {
    cancelAndDelete(stopEvent);
    EV_INFO << "[~CFlowCollector] Cancel and Delete\n";
}

std::map<int, int> portPacketCounts;
std::map<int, omnetpp::simsignal_t> portCountSignals;

void CFlowCollector::initialize() {
    window       = par("collectionWindow");
    maxFlows     = par("maxFlows");
    collecting   = false;
    stopEvent    = new cMessage("stopCollection");
    flowsExportedSignal = registerSignal("flowsExported");

    for (int port = 0; port < 10; ++port) {
        std::string sigName = "port" + std::to_string(port) + "_countVec";
        portCountSignals[port] = registerSignal(sigName.c_str());
    }

    EV_INFO << "[initialize] window=" << window << ", maxFlows=" << maxFlows << "\n";
    EV_INFO << "[initialize] done\n";
}

void CFlowCollector::handleMessage(cMessage *msg) {
    if (msg == stopEvent) {
        EV_INFO << "[CFlowCollector] [stopEvent] Exporting window stats (simTime=" << simTime() << ")\n";
        int totalFlows = 0;
        std::vector<FlowRecord> batchRecords;
        for (const auto& entry : portPacketCounts) {
            int port = entry.first;
            int count = entry.second;
            EV_INFO << "[CFlowCollector]    Port " << port << " sent " << count << " packets in window\n";
            recordScalar(("Port" + std::to_string(port) + "_count").c_str(), count);
            FlowRecord fr;
            fr.srcPort = port;
            fr.packetCount = count;
            batchRecords.push_back(fr);
            totalFlows++;
        }

        if (!batchRecords.empty()) {
            EV_INFO << "[CFlowCollector] Sending flowBatch to analyzer, batch size: " << batchRecords.size() << "\n";
            Packet *batch = new Packet("flowBatch");
            batch->setContextPointer(new std::vector<FlowRecord>(batchRecords));
            send(batch, "packetOut");
        } else {
            EV_INFO << "[CFlowCollector] No batchRecords created this window.\n";
        }

        for (const auto& entry : portPacketCounts) {
            int port = entry.first;
            int count = entry.second;
            emit(portCountSignals[port], count);
            recordScalar(("Port" + std::to_string(port) + "_count").c_str(), count);
        }

        emit(flowsExportedSignal, totalFlows);
        portPacketCounts.clear();
        scheduleAt(simTime() + window, stopEvent);
        EV_INFO << "[CFlowCollector] Window reset, awaiting next flows.\n";
        return;
    }
    else if (msg->arrivedOn("controlIn")) {
        EV_INFO << "[CFlowCollector] [controlIn] Got window trigger @ " << simTime() << "\n";
        collecting = true;
        if (stopEvent->isScheduled())
            cancelEvent(stopEvent);
        scheduleAt(simTime() + window, stopEvent);
        delete msg;
        return;
    }
    else if (msg->arrivedOn("packetIn")) {
        if (msg->hasPar("ingressPort")) {
            int port = msg->par("ingressPort");
            portPacketCounts[port]++;
            EV_INFO << "[CFlowCollector] Counted a packet from port " << port
                    << ", total in window: " << portPacketCounts[port] << "\n";
        } else {
            EV_WARN << "[CFlowCollector] Got message with no ingressPort param\n";
        }
        delete msg;
    }
}
