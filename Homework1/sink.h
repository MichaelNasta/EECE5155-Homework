/*
 * sink.h
 *
 *  Created on: Mar 9, 2021
 *      Author: michael
 */

#ifndef SINK_H_
#define SINK_H_

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class SinkNodeCSMACA : public cSimpleModule {
    private:
        int receivedPkts;
        int collided;
        double dr;
        double latency;
        double energy;

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void finish() override;

};


#endif /* SINK_H_ */
