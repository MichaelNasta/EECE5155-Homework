/*
 * MobileSinkNode2BD.h
 *
 *  Created on: Apr 7, 2021
 *      Author: michael
 */

#ifndef MOBILESINKNODE2BD_H_
#define MOBILESINKNODE2BD_H_


#include <stdio.h>
#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class MobileSinkNode2BD : public cSimpleModule {
    private:
        cMessage *update_pos;
        cMessage *send_srb;
        cMessage *send_lrb;
        double Delta;
        double Tbi;
        double speed;
        double theta;
        //double dist_travelled;
        int num_passes;
        double x_pos, y_pos;
        double x_start, y_start;
        double x_end, y_end;

    public:
        MobileSinkNode2BD();
        virtual ~MobileSinkNode2BD();

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        void update_position();
        double compute_dist();
};


#endif /* MOBILESINKNODE2BD_H_ */
