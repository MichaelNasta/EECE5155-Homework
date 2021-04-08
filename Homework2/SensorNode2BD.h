/*
 * SensorNode2BD.h
 *
 *  Created on: Apr 7, 2021
 *      Author: michael
 */

#ifndef SENSORNODE2BD_H_
#define SENSORNODE2BD_H_


#ifndef MOBILESINKNODE2BD_H_
#define MOBILESINKNODE2BD_H_


#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

class SensorNode2BD : public cSimpleModule {
    private:
        double delta_l;
        double delta_h;
        double disc_range;
        double comm_range;
        double T_on;
        double T_off_l;
        double T_off_h;
        double p_tx;
        double p_rx;

    public:
        SensorNode2BD();
        virtual ~SensorNode2BD();

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
};


#endif /* SENSORNODE2BD_H_ */
