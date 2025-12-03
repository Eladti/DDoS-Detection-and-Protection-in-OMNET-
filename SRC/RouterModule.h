#ifndef ROUTERMODULE_H
#define ROUTERMODULE_H

#include <omnetpp.h>
#include <set>

using namespace omnetpp;

class RouterModule : public cSimpleModule {
  protected:
    std::set<int> blockedPorts;

    virtual void handleMessage(cMessage *msg) override {
        if (msg->arrivedOn("inFromAnalyzer")) {
            if (strcmp(msg->getName(), "blockPort") == 0 && msg->hasPar("blockPort")) {
                int blockPort = msg->par("blockPort");
                blockedPorts.insert(blockPort);
                EV_INFO << "[RouterModule] Blocking port " << blockPort << " (added to blocklist)\n";
            } else {
                EV_WARN << "[RouterModule] Got unexpected message from analyzer: " << msg->getName() << "\n";
            }
            delete msg;
            return;
        }

        int arrivedGateIndex = msg->getArrivalGate()->getIndex();

        if (blockedPorts.count(arrivedGateIndex) > 0) {
            EV_INFO << "[RouterModule] Dropping packet from blocked port " << arrivedGateIndex << "\n";
            delete msg;
            return;
        }

        if (arrivedGateIndex == 8) {
            delete msg;
            return;
        }
        if (gate("ethg$o", 7)->isConnected()) {
            send(msg->dup(), "ethg$o", 7);
        }
        if (hasGate("outToMonitor") && gate("outToMonitor")->isConnected()) {
            auto *srcInfo = new cMessage("SrcNotify");
            srcInfo->addPar("ingressPort") = arrivedGateIndex;
            send(srcInfo, "outToMonitor");
        } else {
            delete msg;
        }
    }
};

#endif
