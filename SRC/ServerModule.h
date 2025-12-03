#ifndef SERVERMODULE_H
#define SERVERMODULE_H

#include <omnetpp.h>
using namespace omnetpp;

class ServerModule : public cSimpleModule {
  protected:
    virtual void initialize() override {}
    virtual void handleMessage(cMessage *msg) override {
        delete msg;
    }
};

#endif
