#include "SC_Poll_HV.h"

SC_Poll_HV::SC_Poll_HV():Tool(){}


bool SC_Poll_HV::Initialise(std::string configfile, DataModel &data)
{
    if(configfile!="")  m_variables.Initialise(configfile);
    //m_variables.Print();

    m_data= &data;
    m_log= m_data->Log;

    if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
    if(!m_variables.Get("PRINTFLAG",PRINTFLAG)) PRINTFLAG=0;

    return true;
}


bool SC_Poll_HV::Execute()
{
    if(m_data->SCMonitor.SumRelays == true)
    {
        //Get HV state
        m_data->SCMonitor.HV_mon = m_data->CB->GetHV_ONOFF();  

        //Get HV value
        m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue;          

        if(m_data->SCMonitor.HV_mon==1)
        {
            retchk = HVCHK();
            if(retchk==false)
            {
                //report error behavior 
                m_data->SCMonitor.errorcodes.push_back(0xCC09EE01);
            }  	  
        }
    }else
    {
        m_data->SCMonitor.HV_mon = -1.0;
        m_data->SCMonitor.HV_return_mon = -1.0;
    }

    //Get Errorscodes
    vector<unsigned int> tmpERR = m_data->CB->returnErrors(); 
    m_data->SCMonitor.errorcodes.insert(std::end(m_data->SCMonitor.errorcodes), std::begin(tmpERR), std::end(tmpERR));
    tmpERR.clear();
    m_data->CB->clearErrors();

    return true;
}


bool SC_Poll_HV::Finalise()
{
    int retval=-2;
    int counter;
    float down_voltage = 0;
    if(m_data->SCMonitor.HV_mon==1)
    {
        m_data->CB->SetHV_voltage(down_voltage,m_data->SCMonitor.HV_return_mon,0);

        m_data->SCMonitor.HV_mon = m_data->CB->GetHV_ONOFF(); 
        m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue; 
        counter=0;
		while(fabs(m_data->SCMonitor.HV_return_mon-down_voltage)>50)
		{
			usleep(10000000);
			m_data->SCMonitor.HV_mon = m_data->CB->GetHV_ONOFF();
			m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue;	
			if(counter>=30){break;}
			counter++;
		}
        if(m_data->SCMonitor.HV_return_mon>50)
        {
            m_data->CB->SetHV_voltage(down_voltage,m_data->SCMonitor.HV_return_mon,0);

            m_data->SCMonitor.HV_mon = m_data->CB->GetHV_ONOFF(); 
            m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue;
            counter=0;
            while(fabs(m_data->SCMonitor.HV_return_mon-down_voltage)>50)
            {
                usleep(10000000);
                m_data->SCMonitor.HV_mon = m_data->CB->GetHV_ONOFF();
                m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue;	
                if(counter>=30){break;}
                counter++;
            }
        }
        retval = m_data->CB->SetHV_ONOFF(false);
        if(retval!=0 && retval!=1)
        {
            //std::cout << " There was an error (Set HV) with retval: " << retval << std::endl;
            m_data->SCMonitor.errorcodes.push_back(0xCD02EE01);
        }
	    m_data->SCMonitor.HV_return_mon = down_voltage;
	    m_data->CB->get_HV_volts = m_data->SCMonitor.HV_return_mon;
	    std::fstream outfile("./configfiles/SlowControl/LastHV.txt", std::ios_base::out | std::ios_base::trunc);
	    outfile << m_data->CB->get_HV_volts;
	    outfile.close();
    }
	
    while(retval!=0){retval = m_data->CB->SetRelay(1,false);} retval=-2;
    while(retval!=0){retval = m_data->CB->SetRelay(2,false);} retval=-2;
    while(retval!=0){retval = m_data->CB->SetRelay(3,false);} retval=-2;

    m_data->CB->Disconnect();
    delete m_data->CB;
    m_data->CB=0;

    return true;
}


bool SC_Poll_HV::HVCHK()
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
                if(i_chk<1000 && PRINTFLAG==1)
                {
                    std::fstream outfile("./configfiles/SlowControl/HV_timer_list.txt", std::ios_base::out | std::ios_base::app);
                    outfile << m_data->SCMonitor.timeSinceEpochMilliseconds  << " : " << m_data->SCMonitor.HV_volts << "V -> after " << timer << "s" << std::endl;
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


bool SC_Poll_HV::HardShutdown(int relay, int errortype)
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