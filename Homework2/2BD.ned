//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

simple SensorNode2BD {
	parameters:
	    double delta_l_perc=default(0.3);
	    double delta_h_perc=default(3);
	    double p_tx=52.2;
	    double p_rx=56.4;
	    
	gates:
	    input in;
	    output out;
}

simple MobileSinkNode2BD {
	parameters:
	    double y=default(15);
	    double speed=1;
	    double Delta=0.001;

	gates:
	    input in;
	    output out;	      
}

simple WirelessChannel {
	gates:
	    input in_src; input in_snk;
	    output out_src; output out_snk;
}

network SourceSink {
	parameters:
	    double discovery_range=default(100);    
	    double communication_range=default(50);
	    double Tbi=0.1; //beacon interval/period
	    double TD=0.004; //packet duration for beacons, data, acks
	    int packets=0;
	    //add number of contacts?
	    
	submodules:
		sensor: SensorNode2BD;
		wirelessChannel: WirelessChannel;
		sink: MobileSinkNode2BD;
		
	connections:
	    sensor.out --> wirelessChannel.in_src;
	    wirelessChannel.out_src --> sink.in;
	    sink.out --> wirelessChannel.in_snk;
	    wirelessChannel.out_snk --> sensor.in;
}