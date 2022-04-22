#ifndef TempClassStore_H
#define TempClassStore_H

#include <iostream>
#include <vector>
#include <map>

#include<PsecData.h>

using namespace std;

class TempClassStore{


 public:

    TempClassStore();

    vector<PsecData> Buffer;

 private:

    //For Fake data use
    /*
    int help1=0;
    int help2=0;
    int counter=0;

    map<int,vector<unsigned short>> ReadFileData;
    vector<unsigned short> ReadFileACC;
    vector<unsigned int> ReadFileError;
    */
};

#endif
