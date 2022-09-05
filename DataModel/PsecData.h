#ifndef PSECDATA_H
#define PSECDATA_H

#include "zmq.hpp"
#include <SerialisableObject.h>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <sstream>

using namespace std;

class PsecData{

    friend class boost::serialization::access;

 public:

    PsecData();
    PsecData(int id);
    ~PsecData();

    bool Send(zmq::socket_t* sock);
    bool Receive(zmq::socket_t* sock);

    //General data
    unsigned int VersionNumber;
    unsigned int ACC_ID;
    vector<int> LAPPDtoBoard1;
    vector<int> LAPPDtoBoard2;
    string Timestamp;

    //Received data from the ACC class
    vector<unsigned short> ReceiveData;

    //To send data form ACC
    vector<int> BoardIndex;
    vector<unsigned short> AccInfoFrame;
    vector<unsigned short> RawWaveform;
    vector<unsigned int> errorcodes;
    int FailedReadCounter;

    //Run control for ACC
    int readRetval;

    bool Print();
    bool SetDefaults();

 private:

 template <class Archive> void serialize(Archive& ar, const unsigned int version){

    ar & VersionNumber;
    ar & ACC_ID;
    ar & LAPPDtoBoard1;
    ar & LAPPDtoBoard2;
    ar & Timestamp;
    ar & BoardIndex;
    ar & AccInfoFrame;
    ar & RawWaveform;
    ar & errorcodes;
    ar & FailedReadCounter;

 }

 
};

#endif
