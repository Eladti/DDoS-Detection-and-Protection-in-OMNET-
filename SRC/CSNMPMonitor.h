#ifndef CSNMPMONITOR_H_
#define CSNMPMONITOR_H_

#include <omnetpp.h>
#include <deque>

using namespace omnetpp;

class CSNMPMonitor : public cSimpleModule {
  private:
    long currentCount;
    int historySize;
    double alpha;
    double threshold;
    double avgPacketRate;
    double absoluteVolumeThreshold;
    double floorFactor;
    double dynamicFloor;
    double firstTriggerTime;
    bool triggered = false;
    std::deque<long> history;
    cMessage *collectEvent;
    simsignal_t snmpTriggerSignal;
    simsignal_t packetReceivedSignal;
    simsignal_t sampleRateSignal;
    simtime_t samplePeriod;
    simtime_t trainingPeriod;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void collectCounters();
    void detectAnomaly(long currentCount);

  public:
    CSNMPMonitor();
    virtual ~CSNMPMonitor();
};

#endif
