#include "ACC_DataRead.h"

ACC_DataRead::ACC_DataRead():Tool(){}


bool ACC_DataRead::Initialise(std::string configfile, DataModel &data){

	if(configfile!="")  m_variables.Initialise(configfile);
	//m_variables.Print();

	m_data= &data;
	m_log= m_data->Log;

	if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
    if(!m_variables.Get("PrintLinesMax",PrintLinesMax)) PrintLinesMax=15000;

	return true;
}


bool ACC_DataRead::Execute(){

	if(m_data->conf.triggermode==1)
	{
		m_data->acc->softwareTrigger();
	}

	m_data->psec.readRetval = m_data->acc->listenForAcdcData(m_data->conf.triggermode);
	if(m_data->psec.readRetval != 0)
	{
        if(m_data->psec.readRetval != 404){m_data->psec.FailedReadCounter = m_data->psec.FailedReadCounter + 1;}
		m_data->psec.ReceiveData.clear();
		m_data->acc->clearData();
	}else
	{
		m_data->psec.AccInfoFrame = m_data->acc->returnACCIF();
		m_data->psec.ReceiveData = m_data->acc->returnRaw();
		m_data->psec.BoardIndex = m_data->acc->returnBoardIndices();
		m_data->acc->clearData();
	}

    //Get Timestamp
    unsigned long long timeSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    m_data->psec.Timestamp = to_string(timeSinceEpoch-UTCCONV); 

    //Get errors
	vector<unsigned int> tmpERR = m_data->acc->returnErrors();
	m_data->psec.errorcodes.insert(std::end(m_data->psec.errorcodes), std::begin(tmpERR), std::end(tmpERR));
	m_data->acc->clearErrors();
	tmpERR.clear();

    if(m_verbose>3){SaveErrorLog();}
	return true;
}


bool ACC_DataRead::Finalise(){
	delete m_data->acc;
    m_data->acc=0;
	return true;
}

bool ACC_DataRead::SaveErrorLog()
{
    int numLines = 0;
    std::string line;
    std::ifstream file("./Errorlog.txt");    
    while(getline(file, line)){numLines++;}
    file.close();

    if(numLines>PrintLinesMax){return false;}
    if(m_data->psec.errorcodes.size()==1)
    {
        if(m_data->psec.errorcodes.at(0)==0x00000000)
        {
            return false;
        }
    }

    //Create Debug file
    std::fstream outfile("./Errorlog.txt", std::ios_base::out | std::ios_base::app);

    //Print a timestamp
    outfile << "Time: " << m_data->psec.Timestamp << endl;
    for(int k1=0; k1<m_data->psec.errorcodes.size(); k1++)
    {
        outfile << m_data->psec.errorcodes.at(k1) << " ";
    }
    outfile << endl;

    outfile.close();

    return true;
}
