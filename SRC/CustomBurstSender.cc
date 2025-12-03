#include <omnetpp.h>
using namespace omnetpp;

class CustomBurstSender : public cSimpleModule {
  protected:
    simtime_t burstDuration, sleepDuration, sendInterval, startTime;
    int burstCount;
    bool active;
    cMessage *tick;
    simtime_t burstEnd;

    virtual void initialize() override {
        // Get parameters from ini file
        startTime = par("startTime");
        burstDuration = par("burstDuration");
        sleepDuration = par("sleepDuration");
        sendInterval = par("sendInterval");
        burstCount = 0;
        active = false;
        tick = new cMessage("tick");
        scheduleAt(startTime, tick);
    }

    virtual void handleMessage(cMessage *msg) override {
        if (msg == tick) {
            if (!active) {
                // Start burst
                active = true;
                burstEnd = simTime() + burstDuration;
            }
            if (active && simTime() < burstEnd) {
                // Send one packet per interval
                send(new cPacket("attackPacket"), "ethg$o");
                burstCount++;
                scheduleAt(simTime() + sendInterval, tick);
            } else {
                // End of burst, sleep
                active = false;
                scheduleAt(simTime() + sleepDuration, tick);
            }
        }
    }

    virtual void finish() override {
        recordScalar("sentPackets", burstCount);
        cancelAndDelete(tick);
    }
};

Define_Module(CustomBurstSender);
