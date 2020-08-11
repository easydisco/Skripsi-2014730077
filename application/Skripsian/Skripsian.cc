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

#include "Skripsian.h"

Define_Module(Skripsian);


void Skripsian::startup()
{
    packetSpacing = (double)par("packetSpacing") / 10000.0;
    packetsPerNode = par("packetsPerNode");
    packetSize = par("packetSize");
    C = 10;

    tableOfViTable.clear();
    neighborTable.clear();
    packetsSent = 0;
    totalSNnodes = getParentModule()->getParentModule()->par("numNodes");

    txInterval_perNode = packetsPerNode * packetSpacing;

    //Membentuk neighbourhood table
    double startTxTime = txInterval_perNode*self;
    setTimer(SEND_PACKET, startTxTime);

    //Memulai algoritma
    double startInitiateAlgo = totalSNnodes*10;
    setTimer(ALGO, startInitiateAlgo);
}



void Skripsian::timerFiredCallback(int timerIndex)
{
    switch (timerIndex) {

        case SEND_PACKET:{

            if (packetsSent >= packetsPerNode){
                break;
            }

            toNetworkLayer(createGenericDataPacket(0, packetsSent, packetSize), BROADCAST_NETWORK_ADDRESS);
            packetsSent++;
            setTimer(SEND_PACKET, packetSpacing);
            break;
        }

        case ALGO:{
            initiateBeforeAlg();
            loop = 0;
            packetsSent = 0;
            usleep(10);
            for(int i = 0;i<degree;i++){
                flagRec[neighborTable[i].id] = false;
            }
            while(loop<=MAX_ITER){
                calculateFj();
                calculateQij();
                calculateLambdaj();
                calculateVj();
                sendV();
                calculateAlphaj();
                matrix<double> rt1 = calculateRt();
                if(abs((rt1-rt))<tauWSN && flag == false){
                    flag = true;
                    flagRec[self]=flag;
                    for(int i = 0;i < degree;i++){
                        cPacket* bendera = createGenericDataPacket(1, packetsSent, packetSize);
                        const char* benderara = "bendera";
                        bendera->setName(benderara);
                        stringstream strs;
                        strs << neighborTable[i].id;
                        string temp = strs.str();
                        const char* id = (char*) temp.c_str();
                        toNetworkLayer(bendera,id);
                        packetsSent++;
                    }
                }
                rt= rt1;
                bool flagDone = true;
                for(int i = 0;i<neighborTable.size();i++){
                    int idP = neighborTable[i].id;
                    if(flagRec[idP]==false){
                        flagDone=false;
                    }
                }
                double R = empiricalRisk();
                if(flagDone == true){
                    break;
                }
                loop++;
                }
                trace()<<"Empirical risk akhir : "<<empiricalRisk();
        }
        break;
    }


}

void Skripsian::sendV(){
    //memindahkan isi matrix vj ke vi
    std::vector<double> vi(vj.begin(),vj.end());
    int ukuran = vi.size();
    ukuran = ukuran+1;
    double loopPengirim = this->loop;
    vi.push_back(loopPengirim);
    for(int i = 0;i < neighborTable.size();i++){
        stringstream strs;
        strs << neighborTable[i].id;
        string temp = strs.str();
        const char* id = (char*) temp.c_str();
        toNetworkLayer(createMyDataPacket(&vi[0],packetsSent,ukuran),id);
        packetsSent++;
    }


}

void Skripsian::initiateBeforeAlg(){
    tauWSN = 0.000001;
    pi = 1, 0, 0,
         0, 1, 0,
         0, 0, 0;
    degree = (int)neighborTable.size();
    flag = false;
    alpha = randm(p1,1);
    lambdaj = randm(Nj,1);
    vjTable();
    this->vj=tabelVj[self];
    firstVi();
    up = totalSNnodes*C*ones_matrix<double>(Nj,1);
    low =0*ones_matrix<double>(Nj,1);
    ident = identity_matrix<double>(p1);
    trace()<<"Degree/banyaknya tetangga pada node  : "<<degree;
    calculateUj();
    calculateHessianMatrix();
    rt = calculateRt();
    viComplete = false;

}

void Skripsian::fromNetworkLayer(ApplicationPacket * rcvPacket, const char *source, double rssi, double lqi)
{   const char *cases = rcvPacket->getName();
    const char *nama = "ApplicationPacket";
    const char *vje = "myDataPacket";
    const char *flagu = "bendera";
    const char *req = "request";
    this->loopVi= this->loop;
    int idPengirim = atoi(source);
    if(strcmp(rcvPacket->getClassName(),nama)==0){
        updateNeighborTable(atoi(source), rcvPacket->getSequenceNumber());
    }
    else if(strcmp(rcvPacket->getClassName(),vje)==0){

        int numElements = ((myDataPacket*)rcvPacket)->getMyDataArrayArraySize();
        int sequenceNumber = ((myDataPacket*)rcvPacket)->getSequenceNumber();
        string destination = ((myDataPacket*)rcvPacket)->getAppNetInfoExchange().destination.c_str();
        std::vector<double> viDiterima(numElements);
        this->loopVi = viDiterima[3];
        viDiterima.pop_back();
        for (int k=0; k<numElements; k++) {
             viDiterima[k] = ((myDataPacket*)rcvPacket)->getMyDataArray(k);
        }
        matrix<double> viD = dlib::mat(viDiterima);
        updateViTable(idPengirim, viD);
        checkLoopComplete();

    }
    else if(strcmp(rcvPacket->getName(),flagu)==0){
        flagRec[idPengirim]=true;
    }
}

void Skripsian::vjTable(){
    for(int i = 0;i<totalSNnodes;i++){
        matrix<double> vjNode;
        int a = 14-i;
        int b = 11+i;
        int c = i;
        double nodeVj[]={a,b,c};
        std::vector<double>vjvj;
        std::copy(std::begin(nodeVj),std::end(nodeVj),std::back_inserter(vjvj));
        vjNode = mat(vjvj);
        tabelVj.push_back(vjNode);
    }
}

void Skripsian::firstVi(){
    viTable.clear();
    for(int i = 0;i<degree;i++){
        viRecord recFVi;
        int idv = (int) neighborTable[i].id;
        matrix<double> vii =tabelVj[idv];
        recFVi.idVi = idv;
        recFVi.vi = vii;
        viTable.push_back(recFVi);
    }
    tableOfViTable.push_back(viTable);
    viTable.clear();

}


void Skripsian::updateViTable(int nodeID, matrix<double> vi){
    viRecord newReco;
    newReco.idVi = nodeID;
    newReco.vi = vi;
    viTable.push_back(newReco);
}

void Skripsian::checkLoopComplete(){
    if(viTable.size()==neighborTable.size()){
        viComplete = true;
        tableOfViTable.push_back(viTable);
        viTable.clear();
    }
}

double Skripsian::empiricalRisk(){
    matrix<double,120,p1> Xt;
    Xt =   -0.7893427,   2.1399148,   1.0000000,
            0.6949764,   1.7930991,   1.0000000,
            0.2160288,   1.8937590,   1.0000000,
           -0.1784215,   1.2772132,   1.0000000,
            0.7684971,  -0.6257323,   1.0000000,
            0.5361308,   2.1641784,   1.0000000,
            0.0771360,   0.8376636,   1.0000000,
            0.4662462,   2.3464374,   1.0000000,
            0.3215651,   3.0441132,   1.0000000,
            3.6669737,   1.7931754,   1.0000000,
           -0.0012136,   0.4763175,   1.0000000,
            0.9511483,   4.3390817,   1.0000000,
           -0.1436904,  -3.2939611,   1.0000000,
           -0.1756625,   2.5999601,   1.0000000,
            0.1020328,   0.2943968,   1.0000000,
            0.2781883,   1.5307493,   1.0000000,
            0.3664523,   1.0615465,   1.0000000,
            2.3074583,   2.8200634,   1.0000000,
            0.7286970,   1.2077844,   1.0000000,
           -0.3882856,   1.4711353,   1.0000000,
            2.2971206,  -0.0200616,   1.0000000,
            1.5885932,   0.6098098,   1.0000000,
            2.5089626,   0.5601763,   1.0000000,
            0.9375771,   4.2618050,   1.0000000,
           -0.6198204,   3.7467341,   1.0000000,
            2.9966506,   5.1015932,   1.0000000,
            1.7672910,   0.7774299,   1.0000000,
            1.5080329,   3.2849581,   1.0000000,
            1.5854497,  -0.0718313,   1.0000000,
            0.8891776,  -0.4540397,   1.0000000,
           -0.6560540,   0.3315466,   1.0000000,
            1.6322466,   1.3253923,   1.0000000,
           -0.2595671,   1.7201719,   1.0000000,
            0.8512743,  -0.9326391,   1.0000000,
            0.8166998,   2.8772914,   1.0000000,
            1.4948244,  -1.1127293,   1.0000000,
            1.4855414,   2.8980031,   1.0000000,
            1.3809909,   1.9375311,   1.0000000,
            1.5168902,   1.2841289,   1.0000000,
            1.0380411,   0.9645279,   1.0000000,
           -0.6262573,   0.0151102,   1.0000000,
            0.4779073,   3.0782390,   1.0000000,
            0.9967939,   0.4811930,   1.0000000,
            1.6200323,   1.3083394,   1.0000000,
            1.6213093,  -0.6732715,   1.0000000,
            2.2907458,   0.9369546,   1.0000000,
            1.5812691,   0.2836155,   1.0000000,
            1.1317695,  -2.0857066,   1.0000000,
            0.6536213,   1.9608076,   1.0000000,
            0.0038698,  -1.3585381,   1.0000000,
            0.9059964,   0.7763449,   1.0000000,
           -0.4270892,  -0.2676538,   1.0000000,
           -0.7911596,  -0.1822719,   1.0000000,
           -1.7637610,  -0.4622396,   1.0000000,
           -0.5973758,   0.3112530,   1.0000000,
            0.1390035,  -0.5041103,   1.0000000,
            1.3263943,   2.5621806,   1.0000000,
           -0.2137377,   0.3316640,   1.0000000,
            2.3033262,   2.8220557,   1.0000000,
            0.6126851,   1.2319244,   1.0000000,
           -2.1498639,  -2.6030688,  -1.0000000,
           -0.5988329,   1.1530881,  -1.0000000,
           -1.9066593,  -2.7785010,  -1.0000000,
           -2.4841624,  -4.3388136,  -1.0000000,
           -2.4297476,  -0.5497998,  -1.0000000,
           -2.1539736,  -1.0747984,  -1.0000000,
           -0.8009645,  -0.4589970,  -1.0000000,
           -0.8574270,   1.7033809,  -1.0000000,
           -1.4006720,   0.3548255,  -1.0000000,
           -0.4904444,  -1.4802585,  -1.0000000,
            0.6481982,  -1.0308666,  -1.0000000,
           -2.2020952,  -1.9025368,  -1.0000000,
           -1.0437890,  -1.5927373,  -1.0000000,
           -0.7601851,  -0.2158945,  -1.0000000,
           -0.3601840,  -0.1582220,  -1.0000000,
           -0.4809978,   0.1955839,  -1.0000000,
           -1.9572672,  -2.2934495,  -1.0000000,
           -3.0594934,  -0.3625720,  -1.0000000,
           -1.5132641,  -1.7223254,  -1.0000000,
           -2.6975748,  -0.8290708,  -1.0000000,
           -2.3093917,  -1.4052213,  -1.0000000,
           -0.6154391,  -0.6445937,  -1.0000000,
           -0.8866993,  -0.6734514,  -1.0000000,
           -1.8688276,  -1.5685860,  -1.0000000,
           -2.5129466,  -1.6723054,  -1.0000000,
           -1.5993368,  -2.3332362,  -1.0000000,
            0.7907260,  -1.2390473,  -1.0000000,
           -1.5588564,   0.0999447,  -1.0000000,
           -2.6741046,  -2.0374657,  -1.0000000,
           -0.5495281,  -1.7638328,  -1.0000000,
           -0.1400354,  -0.2132642,  -1.0000000,
           -0.0922451,  -1.4993444,  -1.0000000,
           -1.3797174,  -0.5175491,  -1.0000000,
            0.8148101,   0.5477808,  -1.0000000,
           -0.2670709,   1.4782479,  -1.0000000,
           -0.8913860,  -0.3671464,  -1.0000000,
           -1.7553135,   0.0158854,  -1.0000000,
           -1.1251336,  -2.9822284,  -1.0000000,
           -1.0250537,  -1.7203743,  -1.0000000,
           -0.8610594,   1.3043217,  -1.0000000,
            0.2530578,  -1.3712623,  -1.0000000,
            0.2898922,  -2.1210391,  -1.0000000,
           -1.4075272,  -3.2100534,  -1.0000000,
           -0.6485805,  -1.2038494,  -1.0000000,
           -1.2705780,  -0.6220316,  -1.0000000,
           -2.1025771,  -0.8862776,  -1.0000000,
           -0.7556332,  -1.1515724,  -1.0000000,
            1.2493006,  -0.4046597,  -1.0000000,
           -1.5329245,  -0.4557103,  -1.0000000,
           -0.6128026,   0.0403042,  -1.0000000,
           -2.3456213,  -1.2284380,  -1.0000000,
            0.1813437,  -0.4610466,  -1.0000000,
           -3.7018435,  -3.8496464,  -1.0000000,
           -1.8076323,  -1.6913805,  -1.0000000,
           -1.2097099,  -2.1087478,  -1.0000000,
            0.6471162,  -0.6513600,  -1.0000000,
            0.1872028,  -3.1935648,  -1.0000000,
           -2.0631621,  -1.2553460,  -1.0000000,
            0.4538827,  -0.3907228,  -1.0000000,
           -0.8003210,  -1.4153583,  -1.0000000;


    matrix<double> Yt;
    Yt = colm(Xt,2);
    int Nt = Xt.nr();
    double R;
    R = 0;
    matrix<double> Yp;
    matrix<double> temp;
    temp = Xt*vj;
    for(int i=0; i<totalSNnodes;i++){
        Yp = signMat(temp);
        for(int j = 0; j<Nt; j++){
            R=R+(0.5)*(abs(Yt(j,0)-Yp(j,0)));
        }
    }
    R = R/(totalSNnodes*Nt);
    return R;
}

matrix<double> Skripsian:: signMat(matrix<double> matt){
    for(int i = 0; i<matt.nr() ;i++){
        for(int j = 0; j<matt.nc();j++){
            if(matt(i,j)==0){
                matt(i,j)=0;
            }
            else if(matt(i,j)<0){
                matt(i,j)=-1;
            }
            else{
                matt(i,j)=1;
            }
        }
    }
    return matt;
}


void Skripsian::updateNeighborTable(int nodeID, int serialNum)
{
    int i = 0, pos = -1;
    int tblSize = (int)neighborTable.size();

    for (i = 0; i < tblSize; i++)
        if (neighborTable[i].id == nodeID)
            pos = i;

    if (pos == -1) {
        neighborRecord newRec;
        newRec.id = nodeID;
        newRec.timesRx = 1;
        if ((serialNum >= 0) && (serialNum < packetsPerNode))
            newRec.receivedPackets = 1;

        neighborTable.push_back(newRec);
    } else {
        neighborTable[pos].timesRx++;

        if ((serialNum >= 0) && (serialNum < packetsPerNode))
            neighborTable[pos].receivedPackets++;
    }
}

matrix<double> Skripsian:: calculateRt(){
    matrix<double,1,1> hLostFunc;
    matrix<double> rTemp;
    hLostFunc = 0;
    matrix<double,Nj,1> YY = dlib::diag(Yj);
    matrix<double> transVj = trans(vj);
    for(int i = 0;i<Nj;i++){
        if(0>(1-YY(i)*rowm(Xj,0)*vj)){
            hLostFunc = hLostFunc;
        }
        else{
        hLostFunc = hLostFunc+(1-YY(i)*rowm(Xj,0)*vj);
        }
    }
    matrix<double,1,1> half;
    half = 0.5;
    rTemp=pointwise_multiply(half,transVj)*(identity_matrix<double>(p1))*vj+Nj*C*hLostFunc;
    return rTemp;
}


void Skripsian::calculateUj(){
    Uj = 1+2*eta*degree*ident-pi;
}



void Skripsian::generateRandom(){
    int seed = std::rand() % 10000;
    //Jumlah Kolom
    const uint n_cols = 2;
    //JUmlah baris
    const uint n_rows = Nj/2;

    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> total_gaussian, gaussian1,gaussian2;
    gaussian1.setZero(n_rows,n_cols);
    gaussian2.setZero(n_rows,n_cols);
    //2*n_rows karena kita mau menggabungkan dua matrix gaussian.
    total_gaussian.setZero(2*n_rows,n_cols+1);

    Eigen::Vector2d mean;
    Eigen::Matrix2d covar;
    mean << 1,1; // Set mean
    covar <<1,0.2,0.2,1;
    //covar = |1 0.2|
    //        |0.2 1|

    //gaussian pertama
    Eigen::EigenMultivariateNormal<double> normX_solver1(mean,covar,seed);
    gaussian1<< normX_solver1.samples(n_rows).transpose() ;
    gaussian1.conservativeResize(Eigen::NoChange,gaussian1.cols()+1);
    for(int i = 0;i<gaussian1.rows();i++ ){
        gaussian1(i,2)=1;
    }
    //gaussian kedua
    mean << -1,-1; // Set mean
    covar <<1,0.2,0.2,1;
    //covar = |1 0.2|
    //        |0.2 1|

    Eigen::EigenMultivariateNormal<double> normX_solver2(mean,covar,seed);
    gaussian2<<normX_solver2.samples(n_rows).transpose() ;
    gaussian2.conservativeResize(Eigen::NoChange,gaussian2.cols()+1);
    for(int i = 0;i<gaussian2.rows();i++ ){
        gaussian2(i,2)=-1;
    }


    //concate gaussian 1 dan 2,
    total_gaussian<<gaussian1,gaussian2;

    //Mengubah matrix eigen menjadi matrix dlib
    for(int i = 0; i<total_gaussian.rows() ;i++){
        for(int j = 0;j<total_gaussian.cols() ;j++){
            Sj(i,j)=total_gaussian(i,j);
        }
    }

}

//hitung matriks lokal(hessian matrix)
void Skripsian::calculateHessianMatrix(){
   generateRandom();
   Xj = Sj;
   set_colm(Xj,2)=1;
   matrix<double> temp;
   temp = colm(Sj,2);
   Yj = dlib::diagm(temp);
   Qj = Yj*Xj*inv(Uj)*trans(Xj)*Yj;
   Qj = (Qj+trans(Qj))/2;//buat simetri
}


//hitung fj
void Skripsian::calculateFj(){
    std::vector<viRecord> viTableDiterima = tableOfViTable[this->loopVi];
    matrix<double> zigmaFj;
    zigmaFj = 0*ones_matrix<double>(p1,1);
    for(int i =0;i<degree;i++){
        matrix<double> vii = viTableDiterima[i].vi;
        zigmaFj = zigmaFj+(vj+vii);
    }
    fj = 2*alpha-eta*zigmaFj;
    viTable.clear();
    viComplete = false;
}

void Skripsian::calculateAlphaj(){
    std::vector<viRecord> viTableDiterima = tableOfViTable[this->loopVi];
    matrix<double> zigmaAlpha;
    zigmaAlpha = 0*ones_matrix<double>(p1,1);
    for(int i =0;i<degree;i++){
        matrix<double> vii = viTableDiterima[i].vi;
        zigmaAlpha = zigmaAlpha+(vj-vii);
    }
    alpha = alpha+eta/2*zigmaAlpha;
}

void Skripsian::calculateQij(){
    qij = ones_matrix<double>(Nj,1)+Yj*Xj*inv(Uj)*fj;
    qij = -1*qij;
}

void Skripsian::calculateLambdaj(){
    solve_qp_box_constrained(Qj,qij,lambdaj,low,up);
}

void Skripsian::calculateVj(){
    vj = inv(Uj)*(trans(Xj)*Yj*lambdaj-fj);
}





