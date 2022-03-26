#include "SC_SetConfig.h"

SC_SetConfig::SC_SetConfig():Tool(){}


bool SC_SetConfig::Initialise(std::string configfile, DataModel &data){

	if(configfile!="")  m_variables.Initialise(configfile);
	//m_variables.Print();

	m_data= &data;
	m_log= m_data->Log;

	m_data->CB= new Canbus();
	//m_data->CB->Connect(); 

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
  	//if(m_verbose>2){std::cout<<"in set config"<<std::endl;}
  	//if(m_data->SCMonitor.recieveFlag==0){return true;} //EndRun catch
	if(m_verbose>2){std::cout<<"d1"<<std::endl;}
  	if(m_data->SCMonitor.recieveFlag==1){Setup();} //Normal Setup condition
	if(m_verbose>2){std::cout<<"d2"<<std::endl;}
	if(m_data->SCMonitor.recieveFlag==2){return true;} //After setup continous run mode
	if(m_verbose>2){std::cout<<"d3"<<std::endl;}
	if(m_data->SCMonitor.recieveFlag==3){Update();} //Used to update HV volts set, triggerboards threshold
	if(m_verbose>2){std::cout<<"d4"<<std::endl;}
	if(m_data->SCMonitor.recieveFlag==4) //Used as skip for setting new emergency thresholds
	{
		m_data->SCMonitor.recieveFlag=2;
		return true;
	} 
	if(m_verbose>2){std::cout<<"d5"<<std::endl;}
	return true;

}


bool SC_SetConfig::Finalise(){

  m_data->CB->get_HV_volts = 0;
  return true;
}


bool SC_SetConfig::Setup(){

	//Pre get the relay states
	m_data->SCMonitor.relayCh1_mon = m_data->CB->GetRelayState(1);
	m_data->SCMonitor.relayCh2_mon = m_data->CB->GetRelayState(2);
	m_data->SCMonitor.relayCh3_mon = m_data->CB->GetRelayState(3);
  
	
   	//------------------------------------Relay Control
	if(m_verbose>1){std::cout<<"Relay Control"<<std::endl;}
	if(m_data->SCMonitor.relayCh1!=m_data->SCMonitor.relayCh1_mon)
	{
		//std::cout << "Relay 1 is " << std::boolalpha << m_data->SCMonitor.relayCh1_mon << " and will be " << std::boolalpha << m_data->SCMonitor.relayCh1  << std::endl;
		retval = m_data->CB->SetRelay(1,m_data->SCMonitor.relayCh1);
		if(retval!=0 && retval!=1)
		{
			//std::cout << " There was an error (Relay 1) with retval: " << retval << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB01EE01);
		}
	}

	m_data->SCMonitor.relayCh2_mon = m_data->CB->GetRelayState(2);
	if(m_data->SCMonitor.relayCh2!=m_data->SCMonitor.relayCh2_mon)
	{
		//std::cout << "Relay 2 is " << std::boolalpha << m_data->SCMonitor.relayCh2_mon << " and will be " << std::boolalpha << m_data->SCMonitor.relayCh2  << std::endl;
		retval = m_data->CB->SetRelay(2,m_data->SCMonitor.relayCh2);
		if(retval!=0 && retval!=1)
		{
			//std::cout << " There was an error (Relay 2) with retval: " << retval << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB01EE02);
		}
	}

	m_data->SCMonitor.relayCh3_mon = m_data->CB->GetRelayState(3);
	if(m_data->SCMonitor.relayCh3!=m_data->SCMonitor.relayCh3_mon)
	{
		//std::cout << "Relay 3 is " << std::boolalpha << m_data->SCMonitor.relayCh3_mon << " and will be " << std::boolalpha << m_data->SCMonitor.relayCh3  << std::endl;
		retval = m_data->CB->SetRelay(3,m_data->SCMonitor.relayCh3);
		if(retval!=0 && retval!=1)
		{
			//std::cout << " There was an error (Relay 3) with retval: " << retval << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB01EE03);
		}
	}  
	
  	
	//------------------------------------HV Prep
	if(m_verbose>1){std::cout<<"HV Prep"<<std::endl;}
	retval = m_data->CB->SetLV(false);
	if(retval!=0 && retval!=1)
  	{
	    //std::cout << " There was an error (Set LV) with retval: " << retval << std::endl;
	    m_data->SCMonitor.errorcodes.push_back(0xCB02EE01);
  	}
	
	
	//------------------------------------HV Control
	if(m_verbose>1){std::cout<<"HV control"<<std::endl;}
	int temp_HVstate = m_data->CB->GetHV_ONOFF();
	if(temp_HVstate==0 || temp_HVstate==1)
	{
		m_data->SCMonitor.HV_mon = temp_HVstate;
	}else
	{
		m_data->SCMonitor.errorcodes.push_back(0xCB03EE00);
	}
	
	if(m_data->SCMonitor.HV_state_set!=m_data->SCMonitor.HV_mon)
	{
		retval = m_data->CB->SetHV_ONOFF(m_data->SCMonitor.HV_state_set);
		if(retval!=0 && retval!=1)
		{
			//std::cout << " There was an error (Set HV) with retval: " << retval << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB03EE01);
		}
	}

	m_data->SCMonitor.HV_mon = m_data->CB->GetHV_ONOFF();
	m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue;
	if(fabs(m_data->CB->get_HV_volts-m_data->SCMonitor.HV_return_mon)>1)
	{
		//std::cout << "ERROR! " << "File gave " << m_data->CB->get_HV_volts << " Readback gave " << m_data->SCMonitor.HV_return_mon << std::endl;
		//std::cout << "Setting them as the read back value" << std::endl;
		m_data->SCMonitor.errorcodes.push_back(0xCB03EE02);
		m_data->CB->get_HV_volts = m_data->SCMonitor.HV_return_mon;
	}

	if(m_data->SCMonitor.HV_volts!=m_data->SCMonitor.HV_return_mon && m_data->SCMonitor.HV_mon==1)
	{
		retval = m_data->CB->SetHV_voltage(m_data->SCMonitor.HV_volts,m_data->SCMonitor.HV_return_mon,m_verbose);
		if(retval==0)
		{	
			m_data->SCMonitor.HV_mon = m_data->CB->GetHV_ONOFF();
			m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue;	
			if(fabs(m_data->SCMonitor.HV_return_mon-m_data->SCMonitor.HV_volts)>1)
			{
				m_data->SCMonitor.errorcodes.push_back(0xCB03EE04);
			}else
			{
				m_data->CB->get_HV_volts = m_data->SCMonitor.HV_volts;
				std::fstream outfile("./configfiles/SlowControl/LastHV.txt", std::ios_base::out | std::ios_base::trunc);
				outfile << m_data->CB->get_HV_volts;
				outfile.close();
			}

		}else
		{
			//std::cout << " There was an error (HV V set) with retval: " << retval << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB03EE03);
		}
	}


	//------------------------------------LV Control
	if(m_verbose>1){std::cout<<"LV control"<<std::endl;}
	int temp_LVstate = m_data->CB->GetLV_ONOFF();
	if(temp_LVstate==0 || temp_LVstate==1)
	{
		m_data->SCMonitor.LV_mon = temp_LVstate;
	}else
	{
		m_data->SCMonitor.errorcodes.push_back(0xCB04EE00);
	}

	if(m_data->SCMonitor.LV_state_set!=m_data->SCMonitor.LV_mon)
	{
		retval = m_data->CB->SetLV(m_data->SCMonitor.LV_state_set);
		if(retval!=0 && retval!=1)
		{
			//std::cout << " There was an error (Set LV) with retval: " << retval << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB04EE01);
		}
	}


	//------------------------------------Triggerboard Control
	if(m_verbose>1){std::cout<<"Triggerboard Control"<<std::endl;}
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

	
	m_data->SCMonitor.recieveFlag=2;
	
	return true;	
}

bool SC_SetConfig::Update(){
	//------------------------------------HV Control
	if(m_verbose>1){std::cout<<"HV re-set"<<std::endl;}
	int temp_HVstate = m_data->CB->GetHV_ONOFF();
	if(temp_HVstate==0 || temp_HVstate==1)
	{
		m_data->SCMonitor.HV_mon = temp_HVstate;
	}else
	{
		m_data->SCMonitor.errorcodes.push_back(0xCB06EE00);
	}
	
	if(m_data->SCMonitor.HV_state_set!=m_data->SCMonitor.HV_mon)
	{
		retval = m_data->CB->SetHV_ONOFF(m_data->SCMonitor.HV_state_set);
		if(retval!=0 && retval!=1)
		{
			//std::cout << " There was an error (Set HV) with retval: " << retval << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB06EE01);
		}
	}

	m_data->SCMonitor.HV_mon = m_data->CB->GetHV_ONOFF();
	m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue;
	if(fabs(m_data->CB->get_HV_volts-m_data->SCMonitor.HV_return_mon)>1)
	{
		//std::cout << "ERROR! " << "File gave " << m_data->CB->get_HV_volts << " Readback gave " << m_data->SCMonitor.HV_return_mon << std::endl;
		//std::cout << "Setting them as the read back value" << std::endl;
		m_data->SCMonitor.errorcodes.push_back(0xCB06EE02);
		m_data->CB->get_HV_volts = m_data->SCMonitor.HV_return_mon;
	}

	if(m_data->SCMonitor.HV_volts!=m_data->SCMonitor.HV_return_mon && m_data->SCMonitor.HV_mon==1)
	{
		retval = m_data->CB->SetHV_voltage(m_data->SCMonitor.HV_volts,m_data->SCMonitor.HV_return_mon,m_verbose);
		if(retval==0)
		{	
			m_data->SCMonitor.HV_mon = m_data->CB->GetHV_ONOFF();
			m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue;	
			if(fabs(m_data->SCMonitor.HV_return_mon-m_data->SCMonitor.HV_volts)>1)
			{
				m_data->SCMonitor.errorcodes.push_back(0xCB06EE04);
			}else
			{
				m_data->CB->get_HV_volts = m_data->SCMonitor.HV_volts;
				std::fstream outfile("./configfiles/SlowControl/LastHV.txt", std::ios_base::out | std::ios_base::trunc);
				outfile << m_data->CB->get_HV_volts;
				outfile.close();
			}

		}else
		{
			//std::cout << " There was an error (HV V set) with retval: " << retval << std::endl;
			m_data->SCMonitor.errorcodes.push_back(0xCB06EE03);
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
