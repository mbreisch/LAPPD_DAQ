#include "SC_SetConfig.h"

SC_SetConfig::SC_SetConfig():Tool(){}


bool SC_SetConfig::Initialise(std::string configfile, DataModel &data){

	if(configfile!="")  m_variables.Initialise(configfile);
	//m_variables.Print();

	m_data= &data;
	m_log= m_data->Log;

	m_data->CB= new Canbus();
	//	m_data->CB->Connect(); 

	std::fstream infile("./configfiles/SlowControl/LastHV.txt", std::ios_base::in);
	if(infile.is_open())
	{
		infile >> m_data->CB->get_HV_volts;
		infile.close();
	}
	
	string ThermistorID;
	m_variables.Get("ThermistorID",ThermistorID);
	m_data->CB->SetThermistorID(ThermistorID);

	if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;

	return true;
}


bool SC_SetConfig::Execute(){
	//check LV/HV state_set 
  std::cout<<"in set config"<<std::endl;
  if(m_data->SCMonitor.recieveFlag==0){return true;} //EndRun catch
   std::cout<<"d1"<<std::endl;
  if(m_data->SCMonitor.recieveFlag==1){Setup();} //Normal Setup condition
	  std::cout<<"d2"<<std::endl;
	if(m_data->SCMonitor.recieveFlag==2){return true;} //After setup continous run mode
	  std::cout<<"d3"<<std::endl;
	if(m_data->SCMonitor.recieveFlag==3){Update();} //Used to update HV volts set, triggerboards threshold
	  std::cout<<"d4"<<std::endl;
	if(m_data->SCMonitor.recieveFlag==4) //Used as skip for setting new emergency thresholds
	{
		m_data->SCMonitor.recieveFlag=2;
		return true;
	} 
	  std::cout<<"d5"<<std::endl;
	return true;

}


bool SC_SetConfig::Finalise(){

  m_data->CB->get_HV_volts = 0;
  return true;
}


bool SC_SetConfig::Setup(){
  std::cout<<"in setup"<<std::endl;
  

m_data->SCMonitor.relayCh1_mon = m_data->CB->GetRelayState(1);
m_data->SCMonitor.relayCh2_mon = m_data->CB->GetRelayState(2);
m_data->SCMonitor.relayCh3_mon = m_data->CB->GetRelayState(3);
   
 std::cout<<"Relay Control"<<std::endl;
   //------------------------------------Relay Control
	if(m_data->SCMonitor.relayCh1!=m_data->SCMonitor.relayCh1_mon)
	{
		retval = m_data->CB->SetRelay(1,m_data->SCMonitor.relayCh1);
		if(retval!=0 && retval!=1)
		{
			//std::cout << " There was an error (Relay 1) with retval: " << retval << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB01EE01);
		}
	}

	if(m_data->SCMonitor.relayCh2!=m_data->SCMonitor.relayCh2_mon)
	{
		retval = m_data->CB->SetRelay(2,m_data->SCMonitor.relayCh2);
		if(retval!=0 && retval!=1)
		{
			//std::cout << " There was an error (Relay 2) with retval: " << retval << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB01EE02);
		}
	}

	if(m_data->SCMonitor.relayCh3!=m_data->SCMonitor.relayCh3_mon)
	{
		retval = m_data->CB->SetRelay(3,m_data->SCMonitor.relayCh3);
		if(retval!=0 && retval!=1)
		{
			//std::cout << " There was an error (Relay 3) with retval: " << retval << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB01EE03);
		}
	}  
 std::cout<<"Relay Control end"<<std::endl;
  std::cout<<"HV Prep"<<std::endl;
	//------------------------------------HV Prep
	retval = m_data->CB->SetLV(false);
	std::cout<<"p1"<<std::endl;
	if(retval!=0 && retval!=1)
	  {
	    std::cout<<"p2"<<std::endl;
	    std::cout << " There was an error (Set LV) with retval: " << retval << std::endl;
	    m_data->SCMonitor.errorcodes.push_back(0xCB02EE01);
	    std::cout<<"p3"<<std::endl;
	  }
	std::cout<<"p4"<<std::endl;
	  std::cout<<"HV Prep end"<<std::endl;
	   std::cout<<"HV control"<<std::endl;
	//------------------------------------HV Control
	bool tempHVmon;
	int tCB_HV = m_data->CB->GetHV_ONOFF();
	if(tCB_HV==0)
	{
		tempHVmon = false;
		m_data->SCMonitor.HV_mon = 0;
	}else if(tCB_HV==1)
	{
	      tempHVmon = true;
	      m_data->SCMonitor.HV_mon = 1;
	}  
	if(m_data->SCMonitor.HV_state_set!=tempHVmon)
	{
		retval = m_data->CB->SetHV_ONOFF(m_data->SCMonitor.HV_state_set);
		if(retval!=0 && retval!=1)
		{
			//std::cout << " There was an error (Set HV) with retval: " << retval << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB03EE01);
		}
	}

	int retstate = m_data->CB->GetHV_ONOFF();
	m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue;
	if(abs(m_data->CB->get_HV_volts-m_data->SCMonitor.HV_return_mon)>0.5)
	{
		//std::cout << "ERROR! " << "File gave " << m_data->CB->get_HV_volts << " Readback gave " << m_data->SCMonitor.HV_return_mon << std::endl;
		//std::cout << "Setting them as the read back value" << std::endl;
		m_data->SCMonitor.errorcodes.push_back(0xCB03EE02);
		m_data->CB->get_HV_volts = m_data->SCMonitor.HV_return_mon;
	}

	if(m_data->SCMonitor.HV_volts!=m_data->CB->get_HV_volts)
	{
		retval = m_data->CB->SetHV_voltage(m_data->SCMonitor.HV_volts);
		if(retval==0)
		{
			m_data->CB->get_HV_volts = m_data->SCMonitor.HV_volts;
			std::fstream outfile("./configfiles/SlowControl/LastHV.txt", std::ios_base::out | std::ios_base::trunc);
			outfile << m_data->CB->get_HV_volts;
			outfile.close();
		}else
		{
			//std::cout << " There was an error (HV V set) with retval: " << retval << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB03EE03);
		}
	}

     std::cout<<"HV control end "<<std::endl;

          std::cout<<"LV control"<<std::endl;
	//------------------------------------LV Control
	bool tempLVmon;
	int tCB_LV = m_data->CB->GetLV_ONOFF();
	if(tCB_LV==0)
	{
		tempLVmon = false;
		m_data->SCMonitor.LV_mon = 0;
	}else if(tCB_LV==1)
	{
		tempLVmon = true;
		m_data->SCMonitor.LV_mon = 1;
	}     
	if(m_data->SCMonitor.LV_state_set!=tempLVmon)
	{
		retval = m_data->CB->SetLV(m_data->SCMonitor.LV_state_set);
		if(retval!=0 && retval!=1)
		{
			//std::cout << " There was an error (Set LV) with retval: " << retval << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB04EE01);
		}
	}

     std::cout<<"LV control end"<<std::endl;

      std::cout<<"Triggerboard Control"<<std::endl;
	//------------------------------------Triggerboard Control
	float tempval;
	if(m_data->SCMonitor.Trig0_threshold!=m_data->CB->GetTriggerDac0(m_data->SCMonitor.TrigVref))
	{
		retval = m_data->CB->SetTriggerDac0(m_data->SCMonitor.Trig0_threshold, m_data->SCMonitor.TrigVref);
		if(retval!=0)
		{
			//std::cout << " There was an error (DAC0) with retval: " << retval << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB05EE01);
		}
		tempval = m_data->CB->GetTriggerDac0(m_data->SCMonitor.TrigVref);
		if(std::abs(tempval - m_data->SCMonitor.Trig0_threshold)<0.001)
		{
			m_data->SCMonitor.Trig0_mon = tempval;
		}else
		{
			//std::cout << " There was an error (DAC0) - 0xC0 hasn't been updated!" << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB05EE02);
		}
	}

	if(m_data->SCMonitor.Trig1_threshold!=m_data->CB->GetTriggerDac0(m_data->SCMonitor.TrigVref))
	{
		retval = m_data->CB->SetTriggerDac1(m_data->SCMonitor.Trig1_threshold, m_data->SCMonitor.TrigVref);
		if(retval!=0)
		{
			//std::cout << " There was an error (DAC1) with retval: " << retval << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB05EE03);
		}
		tempval = m_data->CB->GetTriggerDac1(m_data->SCMonitor.TrigVref);
		if(std::abs(tempval - m_data->SCMonitor.Trig1_threshold)<0.001)
		{
			m_data->SCMonitor.Trig1_mon = tempval;
		}else
		{
			//std::cout << " There was an error (DAC1) - 0xC0 hasn't been updated!" << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB05EE04);
		}
	} 

  std::cout<<"Triggerboard Control end"<<std::endl;
	
	m_data->SCMonitor.recieveFlag=2;
	
	return true;	
}

bool SC_SetConfig::Update(){
	int retstate = m_data->CB->GetHV_ONOFF();
	m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue;
	if(abs(m_data->CB->get_HV_volts-m_data->SCMonitor.HV_return_mon)>0.5)
	{
		//std::cout << "ERROR! " << "File gave " << m_data->CB->get_HV_volts << " Readback gave " << m_data->SCMonitor.HV_return_mon << std::endl;
		//std::cout << "Setting them as the read back value" << std::endl;
		m_data->SCMonitor.errorcodes.push_back(0xCB06EE01);
		m_data->CB->get_HV_volts = m_data->SCMonitor.HV_return_mon;
	}

	if(m_data->SCMonitor.HV_volts!=m_data->CB->get_HV_volts)
	{
		retval = m_data->CB->SetHV_voltage(m_data->SCMonitor.HV_volts);
		if(retval==0)
		{
			m_data->CB->get_HV_volts = m_data->SCMonitor.HV_volts;
			std::fstream outfile("./configfiles/SlowControl/LastHV.txt", std::ios_base::out | std::ios_base::trunc);
			outfile << m_data->CB->get_HV_volts;
			outfile.close();
		}else
		{
			//std::cout << " There was an error (HV V set) with retval: " << retval << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB06EE02);
		}
	}
	
	//------------------------------------Triggerboard Control
	float tempval;
	if(m_data->SCMonitor.Trig0_threshold!=m_data->CB->GetTriggerDac0(m_data->SCMonitor.TrigVref))
	{
		retval = m_data->CB->SetTriggerDac0(m_data->SCMonitor.Trig0_threshold, m_data->SCMonitor.TrigVref);
		if(retval!=0)
		{
			//std::cout << " There was an error (DAC0) with retval: " << retval << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB07EE01);
		}
		tempval = m_data->CB->GetTriggerDac0(m_data->SCMonitor.TrigVref);
		if(std::abs(tempval - m_data->SCMonitor.Trig0_threshold)<0.001)
		{
			m_data->SCMonitor.Trig0_mon = tempval;
		}else
		{
			//std::cout << " There was an error (DAC0) - 0xC0 hasn't been updated!" << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB07EE02);
		}
	}

	if(m_data->SCMonitor.Trig1_threshold!=m_data->CB->GetTriggerDac0(m_data->SCMonitor.TrigVref))
	{
		retval = m_data->CB->SetTriggerDac1(m_data->SCMonitor.Trig1_threshold, m_data->SCMonitor.TrigVref);
		if(retval!=0)
		{
			//std::cout << " There was an error (DAC1) with retval: " << retval << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB07EE03);
		}
		tempval = m_data->CB->GetTriggerDac1(m_data->SCMonitor.TrigVref);
		if(std::abs(tempval - m_data->SCMonitor.Trig1_threshold)<0.001)
		{
			m_data->SCMonitor.Trig1_mon = tempval;
		}else
		{
			//std::cout << " There was an error (DAC1) - 0xC0 hasn't been updated!" << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB07EE04);
		}
	} 
	
	m_data->SCMonitor.recieveFlag=2;
	
	return true;	
	
}
