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


simple SensorNodeCSMACA
{
    parameters:
        int tValue @unit(s) = default(5.0s);
        double tccaValue=default(0.000128); //time to sense the channel
        int macMinBE=default(3);
        int macMaxBE=default(4);
        int macMaxCSMABackoffs=default(2);
        double DbpValue=default(0.00032);
        double DpValue=default(0.004256); //length of packet
        int pktsToSend = default(1000); //number of packets to send
        double pTx=49.5; //transmission power mW
        double pRx=56.4; //receive power mW
		double T=5.0; //period, packet transmitted every 5s

    gates:
        output out;
}

simple SinkNodeCSMACA
{
    gates:
        input in[];
}

network sourceSink
{
    parameters:
        int n = default(5); //number of nodes
        bool channelFree = default(true);
        int concurrentTx = default(0);
        double latency = default(0);
        int droppedPkts = default(0);
        int txPkts = default(0);
        double energy = default(0);

    submodules:
        source[n]: SensorNodeCSMACA {
        }
        sink: SinkNodeCSMACA {
        }

    connections:
        for i=0..n-1 {
            source[i].out --> sink.in++;
        }
}
