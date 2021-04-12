/*
 * WirelessChannel.h
 *
 *  Created on: Apr 11, 2021
 *      Author: michael
 */

#ifndef WIRELESSCHANNEL_H_
#define WIRELESSCHANNEL_H_

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class WirelessChannel : public cSimpleModule {
    private:
        bool enable_loss_prob;
        //double loss_prob;
        double prob;

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        //bool forward_msg(cMessage *msg);
        double compute_prob();
};



#endif /* WIRELESSCHANNEL_H_ */
