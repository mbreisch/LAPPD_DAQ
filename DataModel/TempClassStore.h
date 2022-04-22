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

    //For Fake data use
    map<int,vector<unsigned short>> ReadFileData;
    vector<unsigned short> ReadFileACC;
    vector<unsigned int> ReadFileError;

 private:

};

#endif
