#include "SC_Poll.h"

SC_Poll::SC_Poll():Tool(){}


bool SC_Poll::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;
  m_log= m_data->Log;

  if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;

  return true;
}


bool SC_Poll::Execute(){
  //if(m_data->SCMonitor.recieveFlag==0){return true;}

  int idf = 0;
  try
  {
    //LV
    m_data->SCMonitor.LV_mon = m_data->CB->GetLV_ONOFF();
    idf++;

    std::vector<float> LVvoltage = m_data->CB->GetLV_voltage();
    idf++;
    m_data->SCMonitor.v33 = LVvoltage[0];
    m_data->SCMonitor.v25 = LVvoltage[1];
    m_data->SCMonitor.v12 = LVvoltage[2];

    //HV
    m_data->SCMonitor.HV_mon = m_data->CB->GetHV_ONOFF();
    idf++;
    m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue;
    idf++;

    //RHT
    vector<float> RHT = m_data->CB->GetTemp();
    idf++;
    m_data->SCMonitor.temperature_mon = RHT[0];
    m_data->SCMonitor.humidity_mon = RHT[1];
    m_data->SCMonitor.temperature_thermistor = m_data->CB->GetThermistor();
    idf++;
    
    //DAC0
    m_data->SCMonitor.Trig0_mon = m_data->CB->GetTriggerDac0(m_data->SCMonitor.TrigVref);
    idf++;
    //DAC1
    m_data->SCMonitor.Trig1_mon = m_data->CB->GetTriggerDac1(m_data->SCMonitor.TrigVref);
    idf++;
    
    //Relay
    m_data->SCMonitor.relayCh1_mon = m_data->CB->GetRelayState(1); idf++;
    m_data->SCMonitor.relayCh2_mon = m_data->CB->GetRelayState(2); idf++;
    m_data->SCMonitor.relayCh3_mon = m_data->CB->GetRelayState(3); idf++;

    //Photodiode
    m_data->SCMonitor.light = m_data->CB->GetPhotodiode(); idf++;

    //Saltbridge
    m_data->SCMonitor.saltbridge = m_data->CB->GetSaltbridge(); idf++;
 
    //Errors
    m_data->SCMonitor.errorcodes = m_data->CB->returnErrors(); idf++;
    m_data->CB->clearErrors();

    //Timestamp
    unsigned long timeSinceEpochMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    m_data->SCMonitor.timeSinceEpochMilliseconds = timeSinceEpochMilliseconds; idf++;
  }catch(...)
  {
    m_data->SCMonitor.errorcodes.push_back((0xCD01EE00 | idf));
  }

  return true;
}


bool SC_Poll::Finalise(){
  int retval=-2;
	int counter;
  if(m_data->SCMonitor.HV_return_mon>5 || m_data->SCMonitor.HV_mon==1)
  {
	  m_data->CB->SetHV_voltage(0,m_data->SCMonitor.HV_return_mon,0);
	  usleep(10000);
	  int retstate = m_data->CB->GetHV_ONOFF(); 
	  float tempHV = m_data->CB->ReturnedHvValue; m_data->SCMonitor.HV_volts=tempHV;
	  counter=0;
		while(fabs(m_data->SCMonitor.HV_return_mon-m_data->SCMonitor.HV_volts)>50)
		{
			usleep(10000000);
			m_data->SCMonitor.HV_mon = m_data->CB->GetHV_ONOFF();
			m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue;	
			if(counter>=30){break;}
			counter++;
		}
	  if(tempHV>5)
	  {
	    m_data->CB->SetHV_voltage(0,m_data->SCMonitor.HV_return_mon,0);
	    usleep(10000);
	    retstate = m_data->CB->GetHV_ONOFF(); m_data->SCMonitor.HV_mon=retstate;
	    tempHV = m_data->CB->ReturnedHvValue; m_data->SCMonitor.HV_volts=tempHV;
		  counter=0;
		while(fabs(m_data->SCMonitor.HV_return_mon-m_data->SCMonitor.HV_volts)>50)
		{
			usleep(10000000);
			m_data->SCMonitor.HV_mon = m_data->CB->GetHV_ONOFF();
			m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue;	
			if(counter>=30){break;}
			counter++;
		}
	  }else
	  {
	    retval = m_data->CB->SetHV_ONOFF(false);
			if(retval!=0 && retval!=1)
			{
				//std::cout << " There was an error (Set HV) with retval: " << retval << std::endl;
				m_data->SCMonitor.errorcodes.push_back(0xCD02EE01);
			}
	  }

	    m_data->CB->get_HV_volts = tempHV;
	    std::fstream outfile("./configfiles/SlowControl/LastHV.txt", std::ios_base::out | std::ios_base::trunc);
	    outfile << m_data->CB->get_HV_volts;
	    outfile.close();
	  m_data->SCMonitor.HV_return_mon = tempHV;
  }
	
  while(retval!=0){retval = m_data->CB->SetRelay(0,false);} retval=-2;
  while(retval!=0){retval = m_data->CB->SetRelay(0,false);} retval=-2;
  while(retval!=0){retval = m_data->CB->SetRelay(0,false);} retval=-2;

  m_data->CB->Disconnect();
  delete m_data->CB;
  m_data->CB=0;
  
  if(m_data->SCMonitor.HV_return_mon>5){return false;}

  return true;
}
