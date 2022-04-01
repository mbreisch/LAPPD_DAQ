#include "SC_Poll.h"

SC_Poll::SC_Poll():Tool(){}


bool SC_Poll::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;
  m_log= m_data->Log;

  if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;

  Tcount=0;
  Scount=0;

  return true;
}


bool SC_Poll::Execute(){
    //if(m_data->SCMonitor.recieveFlag==0){return true;}
	
    if(m_data->SCMonitor.SumRelays == true)
    {
        //LV
        m_data->SCMonitor.LV_mon = m_data->CB->GetLV_ONOFF(); 

        //LV vlaues
        std::vector<float> LVvoltage = m_data->CB->GetLV_voltage();  
        m_data->SCMonitor.v33 = LVvoltage[0];
        m_data->SCMonitor.v25 = LVvoltage[1];
        m_data->SCMonitor.v12 = LVvoltage[2];

        //HV state
        m_data->SCMonitor.HV_mon = m_data->CB->GetHV_ONOFF();  

        //HV value
        m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue;  

        //Hum Temp
        vector<float> RHT = m_data->CB->GetTemp(); 
        m_data->SCMonitor.temperature_mon = RHT[0];
        m_data->SCMonitor.humidity_mon = RHT[1];

        //Triggerboard
        m_data->SCMonitor.Trig0_mon = m_data->CB->GetTriggerDac0(m_data->SCMonitor.TrigVref); 
        m_data->SCMonitor.Trig1_mon = m_data->CB->GetTriggerDac1(m_data->SCMonitor.TrigVref);  
            
        //Photodiode
        m_data->SCMonitor.light = m_data->CB->GetPhotodiode();
    }

    //Relay
    m_data->SCMonitor.relayCh1_mon = m_data->CB->GetRelayState(1); 
    m_data->SCMonitor.relayCh2_mon = m_data->CB->GetRelayState(2); 
    m_data->SCMonitor.relayCh3_mon = m_data->CB->GetRelayState(3);

 
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

    //Saltbridge
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
 
    //Errors
    vector<unsigned int> tmpERR = m_data->CB->returnErrors(); 
    m_data->SCMonitor.errorcodes.insert(std::end(m_data->SCMonitor.errorcodes), std::begin(tmpERR), std::end(tmpERR));
    tmpERR.clear();
    m_data->CB->clearErrors();

    //Timestamp
    unsigned long long timeSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    m_data->SCMonitor.timeSinceEpochMilliseconds = to_string(timeSinceEpoch); 
 
    return true;
}


bool SC_Poll::Finalise(){
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
