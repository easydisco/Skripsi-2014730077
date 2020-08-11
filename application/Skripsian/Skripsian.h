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

#ifndef SKRIPSIAN_H_
#define SKRIPSIAN_H_
#undef Success

#include <eigen-master/Eigen/Eigen>
#include <eigen-master/Eigen/Core>
#include <eigen-master/Eigen/Dense>
#include <math.h>
#include <paper/dlib/all/source.cpp>
#include <paper/dlib/matrix.h>
#include <paper/dlib/optimization.h>
#include <paper/dlib/geometry.h>
#include <paper/dlib/queue.h>
#include <map>
#include <unordered_map>
#include <string>
#include <unistd.h>
#include <random>
#include <iostream>
#include <fstream>
#include "eigenmvn.h"
#include "VirtualApplication.h"

using namespace std;
using namespace dlib;

struct viRecord{
    int idVi;
    matrix<double> vi;
};

struct neighborRecord {
    int id;
    int timesRx;
    int receivedPackets;
};

std::unordered_map<int,bool> flagRec;
enum SkripsianTestTimers {
    SEND_PACKET = 1,
    ALGO = 2,
};


class Skripsian: public VirtualApplication {
 private:
    // parameters and variables
    int priority;
    int packetHeaderOverhead;
   // int constantDataPayload;
    double packetSpacing;
    int packetsPerNode;
    int packetSize;
    bool flag;
    const static int MAX_ITER=300;
    int loop;
    int loopVi;

    std::vector<matrix<double>> tabelVj;
    std::vector<neighborRecord> neighborTable;
    std::vector<viRecord> viTable;
    std::vector<std::vector<viRecord>> tableOfViTable;
    int packetsSent;
    int serialNumber;
    int totalSNnodes;
    double txInterval_perNode;
    bool viComplete;

    const static long dimensiP = 2;//n
    long degree;
    double C;
    double tauWSN;
    //banyaknya node
    const static long Nj = 30;
    const static long p1 = dimensiP+1;//n+1
    const static long eta = 10;
    matrix<double> ident;
    matrix<double> low;
    matrix<double> up;
    //matrix pi
    matrix<double,p1,p1> pi;
    matrix<double,Nj,p1> Sj;
    matrix<double,Nj,p1> Xj;
    matrix<double> Yj;
    //Hessian
    matrix<double,Nj,Nj> Qj;
    //matrix UJ
    matrix<double> Uj;
    //LAmbda
    matrix<double> lambdaj;
    matrix<double> vj;
    matrix<double> alpha;
    matrix<double,p1,1>fj;
    matrix<double>qij;
    matrix<double>rt;

 protected:
    void startup();
    void fromNetworkLayer(ApplicationPacket *, const char *, double, double);
    void timerFiredCallback(int);
    void updateNeighborTable(int nodeID, int theSN);
    void updateViTable(int ,matrix<double>);

    void checkLoopComplete();
    void firstVi();
    void initiateBeforeAlg();
    void calculateUj();
    void calculateVj();
    void generateRandom();
    void calculateAlphaj();
    void calculateFj();
    void calculateQij();
    void calculateLambdaj();
    void calculateHessianMatrix();
    matrix<double> calculateRt();
    void sendV();
    void vjTable();
    double empiricalRisk();
    matrix<double> signMat(matrix<double>);
};

#endif /* SKRIPSIAN_H_ */
