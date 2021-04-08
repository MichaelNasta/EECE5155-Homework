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

using namespace omnetpp;

Define_Module(SensorNode2BD);
Define_Module(WirelessChannel);
Define_Module(MobileSinkNode2BD);

void MobileSinkNode2BD::initialize() {
    cModule *c=getModuleByPath("SourceSink");

    Delta=par("Delta");
    Tbi=par("Tbi");
    speed=par("speed");
    theta=0;
    x_start=-sqrt(pow((double)c->par("discovery_range"),2)+pow(par("y"),2));
    y_start=par("y");
    x_pos=x_start;
    y_pos=y_start;
    x_end=sqrt(pow((double)c->par("discovery_range"),2)+pow(par("y"),2));
    y_end=par("y");
}

void MobileSinkNode2BD::update_position() {
    double displ=speed*Delta;
    x_pos=x_pos+displ;
}

void MobileSinkNode2BD::handleMessage
