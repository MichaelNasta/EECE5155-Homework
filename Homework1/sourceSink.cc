/*
 * sourcesink.cc
 *
 *  Created on: Mar 5, 2021
 *      Author: michael
 */

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <omnetpp.h>
#include "source.h"
#include "sink.h"

using namespace omnetpp;

Define_Module(SensorNodeCSMACA);
Define_Module(SinkNodeCSMACA);


SensorNodeCSMACA::SensorNodeCSMACA() {
    backoffTimerExpired=nullptr;
    setChannelBusy=nullptr;
    setChannelFree=nullptr;
    sendMessage=nullptr;
    decreaseConcurrentTxCounter=nullptr;

}

SensorNodeCSMACA::~SensorNodeCSMACA() { //deconstructor
    if(backoffTimerExpired!=nullptr){
        cancelAndDelete(backoffTimerExpired);
    }
    //perhaps we need to create similar if statements for each cMessage
}

void SensorNodeCSMACA::initialize() { //do we have to declare these variables outside the method?
    Dbp=par("DbpValue");
    //do we have to do this with all the paramters? probably...
    T=par("T");
    nb=0;
    be=par("macMinBE"); //he said something about this being a class variable he didn't write
    totPkts=par("pktsToSend");
    pktsToSend=totPkts;

    if(pktsToSend>0) {
        pkt_creation_time=simTime().dbl(); //should I make this a class variable too?
        backoffTimerExpired=new cMessage("backoffTimerExpired");
        scheduleAt(simTime()+generateBackoffTime(), backoffTimerExpired);
    }
}

double SensorNodeCSMACA::generateBackoffTime() {
    int rvInt = intuniform(0,pow(2,be));
    double time=((double)rvInt)*Dbp;
    return time;
}

void SensorNodeCSMACA::handleMessage(cMessage *msg) {

    cModule *c=getModuleByPath("sourceSink");

    if(msg==backoffTimerExpired) {
        if(performCCA()) {
            if(setChannelBusy!=nullptr) {
                cancelAndDelete(setChannelBusy);
            }
            setChannelBusy=new cMessage("setChannelBusy");
            scheduleAt(simTime()+Dbp-0.000001, setChannelBusy);
        } else {
            nb++;
            be++;
            if(be>(int)par("macMaxBE")) {
                be=(int)par("macMaxBE");
            }
            if(nb<=(int)par("macMaxCSMABackoffs")) {
                //is there supposed to be something here? before the schedule?
                //go to 1:05:00 in Mar 1 for this function
                scheduleAt(simTime()+generateBackoffTime(), backoffTimerExpired); //is this right cMessage?
            } else {
                //cModule *c=getModuleByPath("sourceSink");
                c->par("droppedPkts")=((int)c->par("droppedPkts"))+1;
                decreaseAndRepeat(); //pretty sure this goes in the else statement but...
            }
        }
    } if(msg==setChannelFree) {
        c->par("channelFree")=true;
        cancelAndDelete(setChannelFree);
    } if(msg==setChannelBusy) {
        //c->par("channelFree")=false;
        sendDataPkt();
    }
}

void SensorNodeCSMACA::decreaseAndRepeat() {
    nb=0;
    be=par("macMinBE"); //if we make this a class variable then change from par
    pktsToSend--; //should we make this an actual variable? would have to change in initialize func too PROBABLY CAN JUST USE THE PAR
    if(pktsToSend>0) {
        scheduleAt((totPkts-pktsToSend)*T+generateBackoffTime(), backoffTimerExpired);
        pkt_creation_time=(totPkts-pktsToSend)*T;
    }
}

bool SensorNodeCSMACA::performCCA() {
    cModule *c=getModuleByPath("sourceSink");
    c->par("energy")=((double)c->par("energy")+(double)par("pRx"))*(double)par("tccaValue"); //pRx and tcca are not class variables yet, but in ned file

    return ((bool)c->par("channelFree"));
}

void SensorNodeCSMACA::sendDataPkt() {
    cModule *c=getModuleByPath("sourceSink");
    c->par("channelFree")=false;

    if((int)c->par("concurrentTx")<=1) {
        double tmp_lot=simTime().dbl()-pkt_creation_time;
        c->par("latency")=((double)c->par("latency"))+tmp_lot;
    }

    c->par("energy")=((double)c->par("energy")+(double)par("pTx"))*(double)par("DpValue");
    c->par("concurrentTx")=(int)c->par("concurrentTx")+1;
    c->par("txPkts")=(int)c->par("txPkts")+1;
    cMessage *dataPkt=new cMessage("dataPkt");
    send(dataPkt, "out");
    setChannelFree=new cMessage("setChannelFree");
    scheduleAt(simTime()+par("DpValue"), setChannelFree);
    //now actually set channelFree=true, but may move this into the event in handleMessage

    decreaseConcurrentTxCounter=new cMessage("decreaseConcurrentTxCounter");
    scheduleAt(simTime()+0.000001, decreaseConcurrentTxCounter);
    c->par("concurrentTx")=((int)c->par("concurrentTx"))-1;
}


void SinkNodeCSMACA::initialize() {
    receivedPkts=0;
    collided=0;
    dr=0;
    latency=0;
    energy=0;
}

void SinkNodeCSMACA::handleMessage(cMessage *msg) {
    cModule *c=getModuleByPath("sourceSink");

    if((int)c->par("concurrentTx")>=1) {
        collided++;
    } else {
        receivedPkts++;
    }
    cancelAndDelete(msg);
}

void SinkNodeCSMACA::finish() {
    cModule *c=getModuleByPath("sourceSink");

    double totalP=receivedPkts+collided+(int)c->par("droppedPkts");
    dr=(receivedPkts/totalP)*100;
    latency=((double)c->par("latency")/receivedPkts)*1000;
    energy=((double)c->par("energy")/receivedPkts)*1000;

    EV << "Delivery Ratio: " << dr << "\n";
    EV << "Latency: " << latency << "\n";
    EV << "Energy: " << energy << "\n";
}
