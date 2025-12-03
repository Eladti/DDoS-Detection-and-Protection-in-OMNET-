#ifndef CFLOWCOLLECTOR_H
#define CFLOWCOLLECTOR_H

#include <omnetpp.h>

class CFlowCollector : public omnetpp::cSimpleModule
{
  protected:
    // Parameters
    double window;
    int maxFlows;
    bool collecting;
    std::map<int, omnetpp::simsignal_t> portCountSignals;

    omnetpp::cMessage *stopEvent;
    omnetpp::simsignal_t flowsExportedSignal;

    virtual void initialize() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;

  public:
    CFlowCollector();
    virtual ~CFlowCollector();
};

#endif
