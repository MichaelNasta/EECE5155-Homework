/*
 * SourceSink.cc
 *
 *  Created on: Apr 7, 2021
 *      Author: michael
 */

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <omnetpp.h>
#include "SensorNode2BD.h"
#include "MobileSinkNode2BD.h"
#include "WirelessChannel.h"

using namespace omnetpp;

Define_Module(SensorNode2BD);
Define_Module(WirelessChannel);
Define_Module(MobileSinkNode2BD);

/*
 * Configure Mobile Sink
 */

MobileSinkNode2BD::MobileSinkNode2BD() {
    update_pos=nullptr;
    send_srb=nullptr;
    send_lrb=nullptr;
}

MobileSinkNode2BD::~MobileSinkNode2BD() {
    if(update_pos!=nullptr)
        cancelAndDelete(update_pos);
    if(send_srb!=nullptr)
        cancelAndDelete(send_srb);
    if(send_lrb!=nullptr)
        cancelAndDelete(send_lrb);
}

void MobileSinkNode2BD::initialize() {
    cModule *c=getModuleByPath("SourceSink");

    double dr=(double)c->par("discovery_range");
    double y=par("y");

    Delta=par("Delta");
    Tbi=(double)c->par("Tbi");
    speed=par("speed");
    theta=0;
    //dist_travelled=0;
    num_passes=0;
    x_start=-sqrt(pow(dr,2)+pow(y,2));
    y_start=par("y");
    x_pos=x_start;
    y_pos=y_start;
    x_end=sqrt(pow(dr,2)+pow(y,2));
    y_end=par("y");

    update_pos=new cMessage("update_pos");
    scheduleAt(simTime()+Delta, update_pos);
}

void MobileSinkNode2BD::update_position() {
    double displ=speed*Delta;
    //dist_travelled+=displ;
    if(x_pos>x_end){
        x_pos=x_start;
        num_passes++;
    } else {
        x_pos+=displ;
    }
    //NEED TO KEEP TRACK OF NUMBER OF PASSES TO FINISH AFTER 1000
}

double MobileSinkNode2BD::compute_dist() {
    cModule *c=getModuleByPath("SourceSink");

    double dist=sqrt(pow(x_pos,2)+pow(y_pos,2));
    par("loss_prob")=dist/(4*(double)c->par("discovery_range"));
    return dist;
}

void MobileSinkNode2BD::handleMessage(cMessage *msg) {
    cModule *c=getModuleByPath("SourceSink");

    if(msg==update_pos) {
        if(compute_dist()<=(double)c->par("discovery_range") && compute_dist()>(double)c->par("communication_range")) {
            EV << "Distance=" << compute_dist(); //for debugging
            send_lrb=new cMessage("send_lrb");
            scheduleAt(simTime()+(double)c->par("TD")+Tbi, send_lrb);
        } if(compute_dist()<=(double)c->par("communication_range")) {
            EV << "Distance=" << compute_dist(); //for debugging
            send_srb=new cMessage("send_srb");
            scheduleAt(simTime()+(double)c->par("TD")+Tbi, send_srb);
        }
        update_position();
        scheduleAt(simTime()+Delta, msg);
    } if(msg==send_lrb) {
        cMessage *lrb=new cMessage("lrb");
        send(lrb, "out");
        EV << "LRB Sent from Sink"; //for debugging
    } if(msg==send_srb) {
        cMessage *srb=new cMessage("srb");
        send(srb, "out");
        EV << "SRB Sent from Sink"; //for debugging
    } if(msg->getName()=="data_pkt") {
        cancelAndDelete(msg);
        cMessage *ack_pkt=new cMessage("ack_pkt");
        send(ack_pkt, "out");
    }
}

/*
 * Configure Wireless Channel
 */

void WirelessChannel::initialize() {
    //cModule *c=getModuleByPath("MobileSinkNode2BD");

    enable_loss_prob=par("enable_loss_prob");
    prob=0;
    //loss_prob=(double)c->par("loss_prob");
}

double WirelessChannel::compute_prob() {
    prob=uniform(0,1);
    return prob;
}

void WirelessChannel::handleMessage(cMessage *msg) { //find a way to implement propagation delay
    cModule *c=getModuleByPath("MobileSinkNode2BD");

    if(msg->getName()=="lrb" || "srb" || "ack_pkt") {
        EV << "Message received by channel";
        if(enable_loss_prob==true) {
            if(compute_prob()>(double)c->par("loss_prob")) {
                send(msg,"out_snk");
            } else {
                cancelAndDelete(msg);
                //increment dropped pkts
            }
        } else {
            send(msg,"out_snk");
        }
    } else {
        if(enable_loss_prob==true) {
            if(compute_prob()>(double)c->par("loss_prob")) {
                send(msg,"out_snk");
            } else {
                cancelAndDelete(msg);
                //increment dropped pkts
            }
        } else {
            send(msg,"out_src");
        }
    }
}

/*
 * Configure Sensor Node
 */

SensorNode2BD::SensorNode2BD() {
    turn_radio_on=nullptr;
    turn_radio_off=nullptr;
    send_data=nullptr;
    tx_timeout_expired=nullptr;
    timeout_to_handle_hdc_ldc=nullptr;
}

SensorNode2BD::~SensorNode2BD() {
    if(turn_radio_on!=nullptr)
        cancelAndDelete(turn_radio_on);
    if(turn_radio_off!=nullptr)
        cancelAndDelete(turn_radio_off);
    if(send_data!=nullptr)
        cancelAndDelete(send_data);
    if(tx_timeout_expired!=nullptr)
        cancelAndDelete(tx_timeout_expired);
    if(timeout_to_handle_hdc_ldc!=nullptr)
        cancelAndDelete(timeout_to_handle_hdc_ldc);
}

void SensorNode2BD::initialize() {
    cModule *c=getModuleByPath("SourceSink");

    delta_l=par("delta_l_perc");
    delta_h=par("delta_h_perc");
    T_on=2*(double)c->par("Tbi");
    T_off_l=T_on*(1-delta_l)/delta_l;
    T_off_h=T_on*(1-delta_h)/delta_h;
    p_tx=par("p_tx");
    p_rx=par("p_rx");
}

void SensorNode2BD::handleMessage(cMessage *msg) {
    cModule *c=getModuleByPath("SourceSink");

    if(msg->getName()=="lrb") {
        EV << "LRB Received"; //for debugging
        cancelAndDelete(msg);
        c->par("lrbs_discovered")=(int)c->par("lrbs_discovered")+1;
        //switch to high duty cycle
    } if(msg->getName()=="srb") {
        EV << "SRB Received"; //for debugging
        cancelAndDelete(msg);
        send_data=new cMessage("send_data");
        scheduleAt(simTime()+(double)c->par("TD")+T_on, send_data);
    } if(msg==send_data) {
        cMessage *data_pkt=new cMessage("data_pkt");
        send(data_pkt,"out");
    } if(msg->getName()=="ack_pkt") {
        cancelAndDelete(msg);
        c->par("ack_packets")=(int)c->par("ack_packets")+1;
    }
}

