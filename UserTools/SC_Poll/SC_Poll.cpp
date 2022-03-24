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
  if(m_data->SCMonitor.recieveFlag==0){return true;}

  //LV
  m_data->SCMonitor.LV_mon = m_data->CB->GetLV_ONOFF();
  std::vector<float> LVvoltage = m_data->CB->GetLV_voltage();
  m_data->SCMonitor.v33 = LVvoltage[0];
  m_data->SCMonitor.v25 = LVvoltage[1];
  m_data->SCMonitor.v12 = LVvoltage[2];

  //HV
  m_data->SCMonitor.HV_mon = m_data->CB->GetHV_ONOFF();
  m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue;

  //RHT
  vector<float> RHT = m_data->CB->GetTemp();
  m_data->SCMonitor.temperature_mon = RHT[0];
  m_data->SCMonitor.humidity_mon = RHT[1];
  m_data->SCMonitor.temperature_thermistor = m_data->CB->GetThermistor();

  //DAC0
  m_data->SCMonitor.Trig0_mon = m_data->CB->GetTriggerDac0(m_data->SCMonitor.TrigVref);

  //DAC1
  m_data->SCMonitor.Trig1_mon = m_data->CB->GetTriggerDac1(m_data->SCMonitor.TrigVref);

  //Relay
  m_data->SCMonitor.relayCh1_mon = m_data->CB->GetRelayState(1);
  m_data->SCMonitor.relayCh2_mon = m_data->CB->GetRelayState(2);
  m_data->SCMonitor.relayCh3_mon = m_data->CB->GetRelayState(3);

  //Photodiode
  m_data->SCMonitor.light = m_data->CB->GetPhotodiode();

  //Saltbridge
  m_data->SCMonitor.saltbridge = m_data->CB->GetSaltbridge();

  //Errors
  m_data->SCMonitor.errorcodes = m_data->CB->returnErrors();
  m_data->CB->clearErrors();

  return true;
}


bool SC_Poll::Finalise(){
  int retval=-2;
  m_data->CB->SetHV_voltage(0);
  usleep(10000);
  int retstate = m_data->CB->GetHV_ONOFF();
  float tempHV = m_data->CB->ReturnedHvValue;
  if(tempHV>1)
  {
    m_data->CB->SetHV_voltage(0);
    usleep(10000);
    retstate = m_data->CB->GetHV_ONOFF();
    tempHV = m_data->CB->ReturnedHvValue;
  }

  while(retval!=0){retval = m_data->CB->SetRelay(0,false);} retval=-2;
  while(retval!=0){retval = m_data->CB->SetRelay(0,false);} retval=-2;
  while(retval!=0){retval = m_data->CB->SetRelay(0,false);} retval=-2;

  m_data->CB->Disconnect();
  delete m_data->CB;
  m_data->CB=0;
  
  if(tempHV>1){return false;}

  return true;
}
