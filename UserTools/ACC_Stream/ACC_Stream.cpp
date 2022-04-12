#include "ACC_Stream.h"

ACC_Stream::ACC_Stream():Tool(){}


bool ACC_Stream::Initialise(std::string configfile, DataModel &data){

    if(configfile!="")  m_variables.Initialise(configfile);
    //m_variables.Print();

    m_data= &data;
    m_log= m_data->Log;

    if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;

    std::string ip="";
    std::string port="0";

    //if(!m_variables.Get("IP",ip)) ip="127.0.0.1";
    if(!m_variables.Get("Port",port)) port="8888";

    std::string connection="tcp://*:"+port;

    sock=new zmq::socket_t(*(m_data->context), ZMQ_DEALER);

    sock->connect(connection.c_str());

    return true;
}


bool ACC_Stream::Execute(){

    m_data->psec.RawWaveform = m_data->psec.ReceiveData;
    m_data->psec.Send(sock);
    if(m_verbose>1){m_data->psec.Print()};

    m_data->psec.errorcodes.clear();
    m_data->psec.ReceiveData.clear();
    m_data->psec.BoardIndex.clear();
    m_data->psec.AccInfoFrame.clear();
    m_data->psec.RawWaveform.clear();

    return true;
}


bool ACC_Stream::Finalise(){
	
    delete sock;
    sock=0; 

    return true;
}


