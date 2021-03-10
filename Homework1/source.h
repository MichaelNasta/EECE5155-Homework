/*
 * source.h
 *
 *  Created on: Mar 7, 2021
 *      Author: michael
 */

#ifndef SOURCE_H_
#define SOURCE_H_

#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class SensorNodeCSMACA : public cSimpleModule {
    private:
        cMessage *backoffTimerExpired;
        cMessage *setChannelBusy;
        cMessage *setChannelFree;
        cMessage *sendMessage;
        cMessage *decreaseConcurrentTxCounter;
        double Dbp;
        double T;
        int be;
        int nb;
        int pktsToSend;
        int totPkts;
        double pkt_creation_time;


    public:
        SensorNodeCSMACA();
        virtual ~SensorNodeCSMACA();

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        void decreaseAndRepeat();
        bool performCCA();
        void sendDataPkt();
        double generateBackoffTime();

};

#endif /* SOURCE_H_ */
