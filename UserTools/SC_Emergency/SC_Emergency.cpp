#include "SC_Emergency.h"

SC_Emergency::SC_Emergency():Tool(){}


bool SC_Emergency::Initialise(std::string configfile, DataModel &data)
{
  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;
  m_log= m_data->Log;

  if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;

  return true;
}


bool SC_Emergency::Execute()
{
  bool retchk;
  retchk = TEMPCHK();
  if(retchk==false)
  {
    //report error behavior 
  }
  retchk = HUMIDITYCHK();
  if(retchk==false)
  {
    //report error behavior 
  }  

  return true;
}


bool SC_Emergency::Finalise(){

  return true;
}

bool SC_Emergency::TEMPCHK(){
  int retval=-2;
  if(m_data->SCMonitor.temperature_mon < m_data->SCMonitor.LIMIT_temperature_low)
  {
    m_data->SCMonitor.FLAG_temperature = 0;
    return true;
  }else if(m_data->SCMonitor.temperature_mon >= m_data->SCMonitor.LIMIT_temperature_low && m_data->SCMonitor.temperature_mon < m_data->SCMonitor.LIMIT_temperature_high)
  {
    //Slow shutdown
    m_data->SCMonitor.FLAG_temperature = 1;
    
    return true;
  }else if(m_data->SCMonitor.temperature_mon >= m_data->SCMonitor.LIMIT_temperature_high)
  {
    int tries = 0
    int max_tries = 50;
    //Instant shutdown
    while(retval!=0 && retval!=1 && tries<max_tries) {retval = m_data->CB->SetRelay(1,false); tries++;}
    while(retval!=0 && retval!=1 && tries<max_tries) {retval = m_data->CB->SetRelay(2,false); tries++;}
    while(retval!=0 && retval!=1 && tries<max_tries) {retval = m_data->CB->SetRelay(3,false); tries++;}
    
    m_data->SCMonitor.FLAG_temperature = 2;
    
    return true; 
  }else
  {
    return false;
  }
}

 
bool SC_Emergency::HUMIDITYCHK(){
  int retval=-2;
  if(m_data->SCMonitor.humidity_mon < m_data->SCMonitor.LIMIT_humidity_low)
  {
    m_data->SCMonitor.FLAG_humidity = 0;
    return true;
  }else if(m_data->SCMonitor.humidity_mon >= m_data->SCMonitor.LIMIT_humidity_low && m_data->SCMonitor.humidity_mon < m_data->SCMonitor.LIMIT_humidity_high)
  {
    //Slow shutdown
    m_data->SCMonitor.FLAG_humidity = 1;
    
    return true;
  }else if(m_data->SCMonitor.humidity_mon >= m_data->SCMonitor.LIMIT_humidity_high)
  {
    int tries = 0
    int max_tries = 50;
    //Instant shutdown
    while(retval!=0 && retval!=1 && tries<max_tries) {retval = m_data->CB->SetRelay(1,false); tries++;}
    while(retval!=0 && retval!=1 && tries<max_tries) {retval = m_data->CB->SetRelay(2,false); tries++;}
    while(retval!=0 && retval!=1 && tries<max_tries) {retval = m_data->CB->SetRelay(3,false); tries++;}
    
    m_data->SCMonitor.FLAG_humidity = 2;
    
    return true; 
  }else
  {
    return false;
  }
}  
