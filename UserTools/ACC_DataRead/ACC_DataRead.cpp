#include "ACC_DataRead.h"

ACC_DataRead::ACC_DataRead():Tool(){}


bool ACC_DataRead::Initialise(std::string configfile, DataModel &data){

	if(configfile!="")  m_variables.Initialise(configfile);
	//m_variables.Print();

	m_data= &data;
	m_log= m_data->Log;

	if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;

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
		m_data->psec.FailedReadCounter = m_data->psec.FailedReadCounter + 1;
		m_data->psec.ReceiveData.clear();
	}else{
		m_data->psec.AccInfoFrame = m_data->acc->returnACCIF();
		m_data->psec.map_acdcIF = m_data->acc->returnACDCIF();
		m_data->psec.ReceiveData = m_data->acc->returnRaw();
	}
	m_data->psec.errorcodes = m_data->acc->returnErrors();
	m_data->acc->clearErrors();

	return true;
}


bool ACC_DataRead::Finalise(){
	delete m_data->acc;
	return true;
}
