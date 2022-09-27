#include "SC_Poll_RHT.h"

SC_Poll_RHT::SC_Poll_RHT():Tool(){}


bool SC_Poll_RHT::Initialise(std::string configfile, DataModel &data)
{
    if(configfile!="")  m_variables.Initialise(configfile);
    //m_variables.Print();

    m_data= &data;
    m_log= m_data->Log;

    if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
    if(!m_variables.Get("CheckDelay",CheckDelay)) CheckDelay=100;

    HFlag = 0;

    return true;
}


bool SC_Poll_RHT::Execute()
{
    bool retchk;
    if(m_data->SCMonitor.RuntimeFlag>=CheckDelay)
    {
        m_data->SCMonitor.RuntimeFlag = -1;
    }else
    {
        m_data->SCMonitor.RuntimeFlag++;
    }

    if(m_data->SCMonitor.SumRelays == true)
    {
        //Get Hum/Temp sensor data
        vector<float> RHT = m_data->CB->GetTemp(); 
        m_data->SCMonitor.temperature_mon = RHT[0];
        m_data->SCMonitor.humidity_mon = RHT[1];
        RHT.clear();

        //Realise Error check
        retchk = TEMPCHK();
        if(retchk==false)
        {
            //report error behavior 
            m_data->SCMonitor.errorcodes.push_back(0xCC01EE00);
        }
        retchk = HUMIDITYCHK();
        if(retchk==false)
        {
            //report error behavior
            m_data->SCMonitor.errorcodes.push_back(0xCC01EE01);
        } 
    }else
    {
        m_data->SCMonitor.temperature_mon = -1.0;
        m_data->SCMonitor.humidity_mon = -1.0;
    }

    return true;
}


bool SC_Poll_RHT::Finalise()
{
    return true;
}


bool SC_Poll_RHT::TEMPCHK(){
    int retval=-2;
    if(m_data->SCMonitor.temperature_mon < m_data->SCMonitor.LIMIT_temperature_low)
    {
        m_data->SCMonitor.FLAG_temperature = 0;
        return true;
    }else if(m_data->SCMonitor.temperature_mon >= m_data->SCMonitor.LIMIT_temperature_low && m_data->SCMonitor.temperature_mon < m_data->SCMonitor.LIMIT_temperature_high)
    {
        m_data->SCMonitor.FLAG_temperature = 1;
        return true;
    }else if(m_data->SCMonitor.temperature_mon >= m_data->SCMonitor.LIMIT_temperature_high)
    {
        bool ret;
        bool safety=true;

        ret = HardShutdown(1,10);
        if(ret==false){safety=false;}
        ret = HardShutdown(2,11);
        if(ret==false){safety=false;}
        ret = HardShutdown(3,12);
        if(ret==false){safety=false;}

        m_data->SCMonitor.FLAG_temperature = 2;

        if(safety==false){m_data->SCMonitor.FLAG_temperature = 3;}

        return safety; 
    }else
    {
        return false;
    }
}


bool SC_Poll_RHT::HUMIDITYCHK(){
    int retval=-2;

    if(m_data->SCMonitor.humidity_mon < m_data->SCMonitor.LIMIT_humidity_low)
    {
        m_data->SCMonitor.FLAG_humidity = 0;
        return true;
    }else if(m_data->SCMonitor.humidity_mon >= m_data->SCMonitor.LIMIT_humidity_low && m_data->SCMonitor.humidity_mon < m_data->SCMonitor.LIMIT_humidity_high)
    {
        m_data->SCMonitor.FLAG_humidity = 1;
        return true;
    }else if(m_data->SCMonitor.humidity_mon >= m_data->SCMonitor.LIMIT_humidity_high)
    {
        bool ret;
        bool safety=true;

        if(m_data->SCMonitor.RuntimeFlag>=0)
        {
            m_data->SCMonitor.FLAG_humidity = 1;
            return safety;
        }

        ret = HardShutdown(1,13);
        if(ret==false){safety=false;}
        ret = HardShutdown(2,14);
        if(ret==false){safety=false;}
        ret = HardShutdown(3,15);
        if(ret==false){safety=false;}

        m_data->SCMonitor.FLAG_humidity = 2;

        if(safety==false){m_data->SCMonitor.FLAG_humidity = 3;}

        return safety; 
    }else
    {
        return false;
    }
}  


bool SC_Poll_RHT::HardShutdown(int relay, int errortype)
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
      m_data->SCMonitor.errorcodes.push_back((0xCC01EE00 | errortype));
      return false;
    }
  
    m_data->SCMonitor.SumRelays = false;

    return true;
}