#ifndef __CFLOWANALYZER_H
#define __CFLOWANALYZER_H

#include <omnetpp.h>
#include <vector>
#include "FlowRecord.h"

using namespace omnetpp;

class CFlowAnalyzer : public cSimpleModule
{
  protected:
    int portPacketThreshold;
    int goodClientPort = -1;
    bool baselineEstablished = false;
    simsignal_t attackFlowSignal;
    std::set<int> trustedPorts;

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void analyze(const std::vector<FlowRecord> &records);
};

#endif
