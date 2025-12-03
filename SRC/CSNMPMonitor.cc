#include "CSNMPMonitor.h"
#include <inet/common/packet/Packet.h>
#include <inet/networklayer/ipv4/Ipv4Header_m.h>
#include <inet/common/packet/Packet.h>

using namespace omnetpp;
using namespace inet;

Define_Module(CSNMPMonitor);

CSNMPMonitor::CSNMPMonitor() {
    collectEvent = nullptr;
}

CSNMPMonitor::~CSNMPMonitor() {
    cancelAndDelete(collectEvent);
}

void CSNMPMonitor::initialize() {
    currentCount     = 0;
    firstTriggerTime = -1;
    dynamicFloor     = -1;
    triggered = false;

    historySize      = par("historySize");
    alpha            = par("alpha").doubleValue();
    threshold        = par("threshold").doubleValue();
    avgPacketRate    = par("initialRate").doubleValue();
    samplePeriod     = par("samplePeriod").doubleValue();
    trainingPeriod   = par("trainingPeriod").doubleValue();
    absoluteVolumeThreshold = par("absoluteVolumeThreshold");
    floorFactor      = par("floorFactor").doubleValue();

    packetReceivedSignal = registerSignal("packetReceived");
    snmpTriggerSignal    = registerSignal("snmpTrigger");
    sampleRateSignal     = registerSignal("sampleRate");

    collectEvent = new cMessage("collectEvent");

    scheduleAt(simTime() + samplePeriod, collectEvent);

    EV_INFO << "CSNMPMonitor initialized: historySize=" << historySize
            << ", alpha=" << alpha
            << ", threshold=" << threshold
            << ", initialRate=" << avgPacketRate
            << ", samplePeriod=" << samplePeriod
            << ", trainingPeriod=" << trainingPeriod
            << "\n";
}


void CSNMPMonitor::handleMessage(cMessage *msg) {
    if (msg == collectEvent) {
        EV_INFO << "CSNMPMonitor::handleMessage(): collectEvent @ " << simTime() << "\n";
        collectCounters();
        return;
    }

    if (msg->hasPar("ingressPort")) {
        int ingressPort = msg->par("ingressPort");
        EV << "[MONITOR] Got SrcNotify from gate index: " << ingressPort << "\n";
        currentCount++;
        send(msg, "outToFlowCollector");
        return;
    }

    EV_WARN << "[MONITOR] Received unknown message: " << msg->getName() << "\n";
    delete msg;
}

void CSNMPMonitor::collectCounters() {
    long sample = currentCount;
    currentCount = 0;

    EV_INFO << "[SNMP] sample=" << sample
            << "  prevAvg=" << avgPacketRate << "\n";

    detectAnomaly(sample);

    if (firstTriggerTime < 0 && !triggered) {
        avgPacketRate = alpha * sample + (1 - alpha) * avgPacketRate;
        EV_INFO << "  updated avgPacketRate=" << avgPacketRate << "\n";
    } else {
        EV_INFO << "  skipping avgPacketRate update (attack ongoing)\n";
    }

    history.push_back(sample);
    if ((int)history.size() > historySize)
        history.pop_front();

    EV_INFO << "  updated avgPacketRate=" << avgPacketRate
            << ", historySize=" << history.size() << "\n";

    EV_INFO << "  emitting sampleRateSignal=" << sample << "\n";
    emit(sampleRateSignal, (int)sample);

    scheduleAt(simTime() + samplePeriod, collectEvent);
}

void CSNMPMonitor::detectAnomaly(long sample) {
    if (simTime() < trainingPeriod) {
        EV_INFO << "[SNMP] t=" << simTime()
                << " still in training (until " << trainingPeriod << "), skipping detectAnomaly()\n";
        return;
    }
    if (dynamicFloor < 0) {
        dynamicFloor = sample * floorFactor;
        EV_INFO << "[SNMP] dynamicFloor set to " << dynamicFloor << "\n";
    }
    double relThresh = avgPacketRate * threshold;
    EV_INFO << "[SNMP] detectAnomaly: sample=" << sample
            << ", relThresh=" << relThresh
            << ", dynamicFloor="    << dynamicFloor << "\n";

    if (sample < dynamicFloor) {
        if (triggered) {
            EV_INFO << "[SNMP] sample " << sample
                    << " < dynamicFloor(" << dynamicFloor << ") → resetting trigger state\n";
            triggered = false;
        }
        return;
    }

    if (!triggered && sample > relThresh) {
        EV_INFO << "*** SNMP ANOMALY at t=" << simTime()
                << " sample=" << sample
                << " avg=" << avgPacketRate << "\n";

        if (firstTriggerTime < 0) {
            firstTriggerTime = simTime().dbl();
            recordScalar("firstSNMPtrigger", firstTriggerTime);
            EV_INFO << "  recorded firstSNMPtrigger=" << firstTriggerTime << "\n";
        }

        EV_INFO << "  emitting snmpTriggerSignal at " << simTime() << "\n";
        emit(snmpTriggerSignal, simTime());

        cMessage *trigger = new cMessage("triggerStage2");
        send(trigger, "triggerStage2");
        triggered = true;
    }
    else if (triggered && sample <= relThresh) {
        EV_INFO << "[SNMP] sample=" << sample
                << " ≤ relThresh → clearing trigger state\n";
        triggered = false;
    }
}
