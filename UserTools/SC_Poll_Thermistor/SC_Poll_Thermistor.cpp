#include "SC_Poll_Thermistor.h"

SC_Poll_Thermistor::SC_Poll_Thermistor():Tool(){}


bool SC_Poll_Thermistor::Initialise(std::string configfile, DataModel &data)
{
    if(configfile!="")  m_variables.Initialise(configfile);
    //m_variables.Print();

    m_data= &data;
    m_log= m_data->Log;

    if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;

    Tcount=0;

    return true;
}


bool SC_Poll_Thermistor::Execute()
{
    bool retchk;
    //Thermistor
    float tmpTherm = m_data->SCMonitor.temperature_thermistor;
    m_data->SCMonitor.temperature_thermistor = m_data->CB->GetThermistor(); 
    if(m_data->SCMonitor.temperature_thermistor<0 && Tcount<2)
    {
        m_data->SCMonitor.temperature_thermistor = tmpTherm; 
        Tcount++;
    }else
    {
        Tcount = 0;
    }

    //Always do
    retchk = THERMISTORCHK();
    if(retchk==false)
    {
        //report error behavior 
        m_data->SCMonitor.errorcodes.push_back(0xCC03EE00);
    }	

    return true;
}


bool SC_Poll_Thermistor::Finalise()
{
    return true;
}


bool SC_Poll_Thermistor::THERMISTORCHK(){
    int retval=-2;
    if(m_data->SCMonitor.temperature_thermistor<0)
    {
        m_data->SCMonitor.errorcodes.push_back(0xCC03EE01);
        return true;
    }
    if(m_data->SCMonitor.temperature_thermistor > m_data->SCMonitor.LIMIT_Thermistor_temperature_low )
    {
        m_data->SCMonitor.FLAG_temperature_Thermistor  = 0;
        return true;
    }else if(m_data->SCMonitor.temperature_thermistor <= m_data->SCMonitor.LIMIT_Thermistor_temperature_low  && m_data->SCMonitor.temperature_thermistor > m_data->SCMonitor.LIMIT_Thermistor_temperature_high )
    {
        m_data->SCMonitor.FLAG_temperature_Thermistor  = 1;   
        return true;
    }else if(m_data->SCMonitor.temperature_thermistor <= m_data->SCMonitor.LIMIT_Thermistor_temperature_high )
    {
        bool ret;
        bool safety=true;

        ret = HardShutdown(1,10);
        if(ret==false){safety=false;}
        ret = HardShutdown(2,11);
        if(ret==false){safety=false;}
        ret = HardShutdown(3,12);
        if(ret==false){safety=false;}

        m_data->SCMonitor.FLAG_temperature_Thermistor = 2;

        if(safety==false){m_data->SCMonitor.FLAG_temperature_Thermistor = 3;}

        return safety; 
    }else
    {
        return false;
    }
}


bool SC_Poll_Thermistor::HardShutdown(int relay, int errortype)
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
      m_data->SCMonitor.errorcodes.push_back((0xCC03EE00 | errortype));
      return false;
    }
  
    m_data->SCMonitor.SumRelays = false;

    return true;
}