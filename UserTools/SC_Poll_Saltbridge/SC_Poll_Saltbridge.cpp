#include "SC_Poll_Saltbridge.h"

SC_Poll_Saltbridge::SC_Poll_Saltbridge():Tool(){}


bool SC_Poll_Saltbridge::Initialise(std::string configfile, DataModel &data)
{
    if(configfile!="")  m_variables.Initialise(configfile);
    //m_variables.Print();

    m_data= &data;
    m_log= m_data->Log;

    if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
    Scount=0;

    return true;
}


bool SC_Poll_Saltbridge::Execute()
{
    //Saltbridge
    bool retchk;
    float tmpSalt = m_data->SCMonitor.saltbridge;
    m_data->SCMonitor.saltbridge = m_data->CB->GetSaltbridge(); 
    if(m_data->SCMonitor.saltbridge<0 && Scount<2)
    {
        m_data->SCMonitor.saltbridge = tmpSalt; 
        Scount++;
    }else
    {
        Scount = 0;
    }

    retchk = SALTBRIDGECHK();
    if(retchk==false)
    {
        //report error behavior
        m_data->SCMonitor.errorcodes.push_back(0xCC02EE00);
    }

    //Get Errorscodes
    vector<unsigned int> tmpERR = m_data->CB->returnErrors(); 
    m_data->SCMonitor.errorcodes.insert(std::end(m_data->SCMonitor.errorcodes), std::begin(tmpERR), std::end(tmpERR));
    tmpERR.clear();
    m_data->CB->clearErrors();

    return true;
}


bool SC_Poll_Saltbridge::Finalise()
{
    return true;
}


bool SC_Poll_Saltbridge::SALTBRIDGECHK(){
    int retval=-2;
    bool safety=true;
    if(m_data->SCMonitor.saltbridge<0)
    {
        m_data->SCMonitor.errorcodes.push_back(0xCC02EE01);
    }
    if(m_data->SCMonitor.saltbridge > m_data->SCMonitor.LIMIT_saltbridge_low)
    {
        m_data->SCMonitor.FLAG_saltbridge  = 0;
    }else if(m_data->SCMonitor.saltbridge <= m_data->SCMonitor.LIMIT_saltbridge_low && m_data->SCMonitor.saltbridge > m_data->SCMonitor.LIMIT_saltbridge_high)
    {
        m_data->SCMonitor.FLAG_saltbridge  = 1;
    }else if(m_data->SCMonitor.saltbridge <= m_data->SCMonitor.LIMIT_saltbridge_high)
    {
        bool ret;

        ret = HardShutdown(1,10);
        if(ret==false){safety=false;}
        ret = HardShutdown(2,11);
        if(ret==false){safety=false;}
        ret = HardShutdown(3,12);
        if(ret==false){safety=false;}

        m_data->SCMonitor.FLAG_saltbridge = 2;

        if(safety==false){m_data->SCMonitor.FLAG_saltbridge = 3;}
    }else
    {
        safety = false;
    }
    
    return safety; 
}


bool SC_Poll_Saltbridge::HardShutdown(int relay, int errortype)
{
    int tries = 0;
    int retval = -2;
    int max_tries = 50;
    //Instant shutdown
    while(retval!=0 && tries<max_tries)
    {
      retval = m_data->CB->SetRelay(relay,false); 
      tries++;
    }

    if(tries>=max_tries && retval!=0)
    {
      m_data->SCMonitor.errorcodes.push_back((0xCC02EE00 | errortype));
      return false;
    }
  
    m_data->SCMonitor.SumRelays = false;

    return true;
}