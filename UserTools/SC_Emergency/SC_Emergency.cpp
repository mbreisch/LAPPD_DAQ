#include "SC_Emergency.h"

SC_Emergency::SC_Emergency():Tool(){}


bool SC_Emergency::Initialise(std::string configfile, DataModel &data)
{
    if(configfile!="")  m_variables.Initialise(configfile);
    //m_variables.Print();

    m_data= &data;
    m_log= m_data->Log;
    
    if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
    if(!m_variables.Get("PRINTFLAG",PRINTFLAG)) PRINTFLAG=0;
    return true;
}


bool SC_Emergency::Execute()
{
    //if(m_data->SCMonitor.recieveFlag==0){return true;}
  
    bool retchk;
    if(m_data->SCMonitor.SumRelays==true)
    {
        if(m_data->SCMonitor.HV_mon==1)
        {
            retchk = HVCHK();
            if(retchk==false)
            {
                //report error behavior 
                m_data->SCMonitor.errorcodes.push_back(0xCC09EE01);
            }  	  
        }
        retchk = TEMPCHK();
        if(retchk==false)
        {
            //report error behavior 
            m_data->SCMonitor.errorcodes.push_back(0xCC01EE01);
        }

        retchk = HUMIDITYCHK();
        if(retchk==false)
        {
            //report error behavior
            m_data->SCMonitor.errorcodes.push_back(0xCC02EE01);
        } 
    }	

    //Always do
    retchk = TEMPCHK_Thermistor();
    if(retchk==false)
    {
        //report error behavior 
        m_data->SCMonitor.errorcodes.push_back(0xCC07EE01);
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

bool SC_Emergency::HVCHK()
{
    int retval=-2; 
    int counter=0;
    float timer=0.0;

    //Verbosity print for first appearance
    if(m_verbose>1)
    {
        std::cout << "HV set value was: " << m_data->SCMonitor.HV_volts << std::endl;	
        std::cout << "Last readback HV value before multi check was: " << m_data->SCMonitor.HV_return_mon << std::endl;	        
    }   

    //Multi check start
    if(fabs(m_data->SCMonitor.HV_return_mon-m_data->SCMonitor.HV_volts)>200)
    {
        timer = 0.0;
        while(timer<timeout) //timeout = 10s
        {
            m_data->SCMonitor.HV_mon = m_data->CB->GetHV_ONOFF();
            m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue;

            if(m_data->SCMonitor.HV_mon!=1){if(m_verbose>1){std::cout<<"HV state: "<<m_data->SCMonitor.HV_mon<<std::endl;}}

            if(m_verbose>1)
            {
                std::cout << "HV value after " << timer << " s was: " <<  m_data->SCMonitor.HV_return_mon << " V" << std::endl;
            }

            if(fabs(m_data->SCMonitor.HV_return_mon-m_data->SCMonitor.HV_volts)<200)
            {              
                if(i_chk<10000 && PRINTFLAG==1)
                {
                    std::fstream outfile("./configfiles/SlowControl/HV_timer_list.txt", std::ios_base::out | std::ios_base::app);
                    outfile << m_data->CB->get_HV_volts << std::endl;
                    outfile.close();
                    i_chk++;
                }
                break;
            }

            usleep(timestep*1000000);
            timer+=timestep;
        }
    }

    if(m_data->SCMonitor.HV_return_mon < (m_data->SCMonitor.HV_volts-200) || m_data->SCMonitor.HV_return_mon > (m_data->SCMonitor.HV_volts+200))
    {
        float down_voltage = 0;
        m_data->SCMonitor.errorcodes.push_back(0xCC10EE01);
        m_data->CB->SetHV_voltage(down_voltage,m_data->SCMonitor.HV_return_mon,2);
        
        m_data->SCMonitor.HV_mon = m_data->CB->GetHV_ONOFF(); 
        m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue; 
        counter=0;
        while(fabs(m_data->SCMonitor.HV_return_mon-down_voltage)>50)
        {
            usleep(10000000);
            m_data->SCMonitor.HV_mon = m_data->CB->GetHV_ONOFF();
            m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue;	
            if(counter>=50){break;}
            counter++;
        }  

        if(m_data->SCMonitor.HV_return_mon>50)
        {
            bool ret;
            bool safety=true;

            ret = HardShutdown(1,5);
            if(ret==false){safety=false;}
            ret = HardShutdown(2,5);
            if(ret==false){safety=false;}
            ret = HardShutdown(3,5);
            if(ret==false){safety=false;}
            m_data->SCMonitor.errorcodes.push_back(0xCC10EE03);
            return safety;
        }else
        {
            retval = m_data->CB->SetHV_ONOFF(false);
                if(retval!=0 && retval!=1)
            {
                //std::cout << " There was an error (Set HV) with retval: " << retval << std::endl;
                m_data->SCMonitor.errorcodes.push_back(0xCC10EE04);
            }
            m_data->SCMonitor.HV_return_mon = down_voltage;
            m_data->CB->get_HV_volts = m_data->SCMonitor.HV_return_mon;
            std::fstream outfile("./configfiles/SlowControl/LastHV.txt", std::ios_base::out | std::ios_base::trunc);
            outfile << m_data->CB->get_HV_volts;
            outfile.close();
            return true;
        }
    }else
    {
        return true;
    }
}


bool SC_Emergency::TEMPCHK(){
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
    if(m_data->SCMonitor.temperature_thermistor<0)
    {
        m_data->SCMonitor.errorcodes.push_back(0xCC07EE02);
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
    int retval=-2;
    if(m_data->SCMonitor.saltbridge<0)
    {
        m_data->SCMonitor.errorcodes.push_back(0xCC08EE02);
        return true;
    }
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
    }
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
  
    m_data->SCMonitor.SumRelays = false;

    return true;
}
    
  
  
  
