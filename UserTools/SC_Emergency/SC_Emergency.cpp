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
  //if(m_data->SCMonitor.recieveFlag==0){return true;}
  
  bool retchk;
  retchk = TEMPCHK();
  if(retchk==false)
  {
    //report error behavior 
    m_data->SCMonitor.errorcodes.push_back(0xCC01EE01);
  }
  retchk = TEMPCHK_Thermistor();
  if(retchk==false)
  {
    //report error behavior 
    m_data->SCMonitor.errorcodes.push_back(0xCC07EE01);
  }
  retchk = HUMIDITYCHK();
  if(retchk==false)
  {
    //report error behavior
    m_data->SCMonitor.errorcodes.push_back(0xCC02EE01);
  } 
  retchk = SALTBRIDGECHK();
  if(retchk==false)
  {
    //report error behavior
    m_data->SCMonitor.errorcodes.push_back(0xCC08EE01);
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
    bool ret;
    bool safety=true;
    
    ret = HardShutdown(1,1);
    if(ret==false){safety=false;}
    ret = HardShutdown(2,1);
    if(ret==false){safety=false;}
    ret = HardShutdown(3,1);
    if(ret==false){safety=false;}
    
    m_data->SCMonitor.FLAG_temperature = 2;
    
    if(safety==false){m_data->SCMonitor.FLAG_temperature = 3;}
    
    return safety; 
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
    bool ret;
    bool safety=true;
    
    ret = HardShutdown(1,2);
    if(ret==false){safety=false;}
    ret = HardShutdown(2,2);
    if(ret==false){safety=false;}
    ret = HardShutdown(3,2);
    if(ret==false){safety=false;}
    
    m_data->SCMonitor.FLAG_humidity = 2;
    
    if(safety==false){m_data->SCMonitor.FLAG_humidity = 3;}
    
    return safety; 
  }else
  {
    return false;
  }
}  


bool SC_Emergency::TEMPCHK_Thermistor(){
  int retval=-2;
  if(m_data->SCMonitor.temperature_thermistor > m_data->SCMonitor.LIMIT_Thermistor_temperature_low )
  {
    m_data->SCMonitor.FLAG_temperature_Thermistor  = 0;
    return true;
  }else if(m_data->SCMonitor.temperature_thermistor <= m_data->SCMonitor.LIMIT_Thermistor_temperature_low  && m_data->SCMonitor.temperature_thermistor > m_data->SCMonitor.LIMIT_Thermistor_temperature_high )
  {
    //Slow shutdown
    m_data->SCMonitor.FLAG_temperature_Thermistor  = 1;   
    return true;
  }else if(m_data->SCMonitor.temperature_thermistor <= m_data->SCMonitor.LIMIT_Thermistor_temperature_high )
  {
    bool ret;
    bool safety=true;
    
    ret = HardShutdown(1,3);
    if(ret==false){safety=false;}
    ret = HardShutdown(2,3);
    if(ret==false){safety=false;}
    ret = HardShutdown(3,3);
    if(ret==false){safety=false;}
    
    m_data->SCMonitor.FLAG_temperature_Thermistor = 2;
    
    if(safety==false){m_data->SCMonitor.FLAG_temperature_Thermistor = 3;}
    
    return safety; 
  }else
  {
    return false;
  }
}

 
bool SC_Emergency::SALTBRIDGECHK(){
 /* int retval=-2;
  if(m_data->SCMonitor.saltbridge > m_data->SCMonitor.LIMIT_saltbridge_low)
  {
    m_data->SCMonitor.FLAG_saltbridge  = 0;
    return true;     
  }else if(m_data->SCMonitor.saltbridge <= m_data->SCMonitor.LIMIT_saltbridge_low && m_data->SCMonitor.saltbridge > m_data->SCMonitor.LIMIT_saltbridge_high)
  {
    m_data->SCMonitor.FLAG_saltbridge  = 1;
    return true;   
  }else if(m_data->SCMonitor.saltbridge <= m_data->SCMonitor.LIMIT_saltbridge_high)
  {
    bool ret;
    bool safety=true;
    
    ret = HardShutdown(1,4);
    if(ret==false){safety=false;}
    ret = HardShutdown(2,4);
    if(ret==false){safety=false;}
    ret = HardShutdown(3,4);
    if(ret==false){safety=false;}
    
    m_data->SCMonitor.FLAG_saltbridge = 2;
    
    if(safety==false){m_data->SCMonitor.FLAG_saltbridge = 3;}
    
    return safety; 
  }else
  {
    return false;
  }*/
  return true;
}  

bool SC_Emergency::HardShutdown(int relay, int errortype)
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
      m_data->SCMonitor.errorcodes.push_back((0xCC05EE00 | errortype));
      return false;
    }
  
    return true;
}
    
  
  
  
