#include "FAKE_DataRead.h"

FAKE_DataRead::FAKE_DataRead():Tool(){}


bool FAKE_DataRead::Initialise(std::string configfile, DataModel &data){

	if(configfile!="")  m_variables.Initialise(configfile);
	//m_variables.Print();

	m_data= &data;
	m_log= m_data->Log;

	if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;

	//Fake Data
	///Load Raw Board 1
	cout << " Reading Board 1";
	fileB1 = "./configfiles/FakeOutChain/B1.txt";
	LoadFile(fileB1,1,0);
	cout << "... done" << endl; 
	cout << "Got " << m_data->TCS.ReadFileData[0].size() << endl;

	///Load Raw Board 2
	cout << " Reading Board 2";
	fileB2 = "./configfiles/FakeOutChain/B2.txt";
	LoadFile(fileB2,1,1);	
	cout << "... done" << endl; 
	cout << "Got " << m_data->TCS.ReadFileData[1].size() << endl;

	///Load Raw Board 3
	cout << " Reading Board 3";
	fileB3 = "./configfiles/FakeOutChain/B3.txt";
	LoadFile(fileB3,1,2);	
	cout << "... done" << endl; 
	cout << "Got " << m_data->TCS.ReadFileData[2].size() << endl;

	///Load Raw Board 4
	cout << " Reading Board 4";
	fileB4 = "./configfiles/FakeOutChain/B4.txt";
	LoadFile(fileB4,1,3);
	cout << "... done" << endl; 
	cout << "Got " << m_data->TCS.ReadFileData[3].size() << endl;

	//Fake ACC
	cout << " Reading ACC Frame";
	fileACC = "./configfiles/FakeOutChain/ACC.txt";
	LoadFile(fileACC,2,0);
	cout << "... done" << endl; 
	cout << "Got " << m_data->TCS.ReadFileACC.size() << endl;

	//Fake Error 
	cout << " Reading Errors";
	fileError = "./configfiles/FakeOutChain/ERROR.txt";
	LoadFile(fileError,3,0);
	cout << "... done" << endl; 
	cout << "Got " << m_data->TCS.ReadFileError.size() << endl;

	return true;
}


bool FAKE_DataRead::Execute(){

	map<int, vector<unsigned short>> TMPV;
	if(Random01()<0.1)
	{
		if(Random01()<=0.5)
		{
			TMPV[0] = m_data->TCS.ReadFileData[0];
			TMPV[1] = m_data->TCS.ReadFileData[2];
		}else
		{
			TMPV[2] = m_data->TCS.ReadFileData[2];
			TMPV[3] = m_data->TCS.ReadFileData[3];
		}
	}else
	{
		TMPV = m_data->TCS.ReadFileData;
	}

	for(std::map<int, vector<unsigned short>>::iterator it=TMPV.begin(); it!=TMPV.end(); ++it)
  	{
		for(int k=0; k<it->second.size(); k++)
		{
			it->second[k] += -50+100*Random01();
		}
	}
	
	//cout << "Gave " << m_data->TCS.ReadFileData.size() << " Boards with " << m_data->TCS.ReadFileData[0].size() << " each to DATA" << endl;
	for(std::map<int, vector<unsigned short>>::iterator it=TMPV.begin(); it!=TMPV.end(); ++it)
  	{
		m_data->psec.ReceiveData.insert(m_data->psec.ReceiveData.end(),it->second.begin(),it->second.end());
		m_data->psec.BoardIndex.push_back(it->first);
	}
	//cout << "Gave " << m_data->TCS.ReadFileACC.size() << " to ACC" << endl;
	m_data->psec.AccInfoFrame = m_data->TCS.ReadFileACC;

	if(Random01() < 0.01)
	{
		//cout << "Gave " << m_data->TCS.ReadFileError.size() << " to ERROR" << endl;
		m_data->psec.errorcodes = m_data->TCS.ReadFileError;	
		m_data->psec.ReceiveData.clear();
	}else
	{
		m_data->psec.errorcodes = {0x00000000};
	}

	TMPV.clear();

	return true;
}


bool FAKE_DataRead::Finalise(){
	return true;
}

bool FAKE_DataRead::LoadFile(string filename, int formattype, int bi)
{
	ifstream rfile;
	rfile.open(filename);

  string line;
  while(getline(rfile,line))
  {
    if(formattype==1)
    {
    	//cout << "Line " << line << " to " << (unsigned short)stoul(line,0,16) << endl;
    	m_data->TCS.ReadFileData[bi].push_back((unsigned short)stoul(line,0,16));
    }else if(formattype==2)
    {
    	m_data->TCS.ReadFileACC.push_back((unsigned short)stoul(line,0,16));
    }else if(formattype==3)
    {
    	m_data->TCS.ReadFileError.push_back((unsigned int)stoul(line,0,16));
    }
  }
  rfile.close();

  return true;
}

float FAKE_DataRead::Random01(){
	auto dt = std::chrono::duration_cast<std::chrono::nanoseconds>(m_clock.now().time_since_epoch()).count();
	std::mt19937 generator((unsigned)dt);
	std::uniform_real_distribution<float> dis(0.0, 1.0);

	float randomRealBetweenZeroAndOne = dis(generator);

	return randomRealBetweenZeroAndOne;
}
