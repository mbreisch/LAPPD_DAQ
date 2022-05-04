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

    sock->bind(connection.c_str());

    items[0].socket = *sock;
    items[0].fd = 0;
    items[0].events = ZMQ_POLLOUT;
    items[0].revents =0;

    return true;
}


bool ACC_Stream::Execute(){
	
    int timer=100;
    zmq::poll(&items[0], 1, timer);

    //Poll
    if((items [0].revents & ZMQ_POLLOUT)) 
    {
        m_data->psec.RawWaveform = m_data->psec.ReceiveData;

        //If poll succeeds check if the buffer is empty
        if(!m_data->TCS.Buffer.empty())
        {
            //If it is not empty send a buffer value and verbose print
            m_data->TCS.Buffer.front().Send(sock);
            if(m_verbose>1){m_data->TCS.Buffer.front().Print();}
            //Remove the first entry that was send
            m_data->TCS.Buffer.pop();
            //If the current read is not a timeout add it to the buffer
            if(m_data->psec.readRetval!=404)
            {
                m_data->TCS.Buffer.push(m_data->psec);
            }
        }else
        {
            //If the buffer is empty check if the event was a timeout
            if(m_data->psec.readRetval==404)
            {   
                //If it was clear and end
                m_data->psec.errorcodes.clear();
                m_data->psec.ReceiveData.clear();
                m_data->psec.BoardIndex.clear();
                m_data->psec.AccInfoFrame.clear();
                m_data->psec.RawWaveform.clear();
                return true;
            }
            //If it was not send the data and verbose print
            m_data->psec.Send(sock);
            if(m_verbose>1){m_data->psec.Print();}
        }
    }else
    {
        //If the poll failed and the read was not a timeout fill the buffer
        if(m_data->psec.readRetval!=404)
        {
            m_data->TCS.Buffer.push(m_data->psec);
        }
    }

    //At the end always clear
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
