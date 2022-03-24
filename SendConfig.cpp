#include <iostream>
#include <string>
#include <zmq.hpp>
#include <SlowControlMonitor.h>

int main(){


  zmq::context_t context;

  zmq::socket_t sock(context, ZMQ_DEALER);
  sock.connect("tcp://127.0.0.1:4444");


  SlowControlMonitor data;

  //HV settings
  std::cout<<"HV_state_set (0): ";
  std::cin>>data.HV_state_set;
  std::cout<<endl<<"HV_volts (0): ";
  std::cin>>data.HV_volts;

  //LV settings
  std::cout<<endl<<"LV_state_set (0): ";
  std::cin>>data.LV_state_set;

  //Emergency limits 
  std::cout<<endl<<"LIMIT_temperature_low (10): ";
  std::cin>>data.LIMIT_temperature_low;
  std::cout<<endl<<"LIMIT_temperature_high (60): ";
  std::cin>>data.LIMIT_temperature_high;
  std::cout<<endl<<"LIMIT_humidity_low (00): ";
  std::cin>>data.LIMIT_humidity_low;
  std::cout<<endl<<"LIMIT_humidity_high (60): ";
  std::cin>>data.LIMIT_humidity_high;


  std::cout<<endl<<"LIMIT_Thermistor_temperature_low (20000): ";
  std::cin>>data.LIMIT_Thermistor_temperature_low;
  std::cout<<endl<<"LIMIT_Thermistor_temperature_high (5000): ";
  std::cin>>data.LIMIT_Thermistor_temperature_high;
  /*
  std::cout<<endl<<"LIMIT_saltbridge_low (600000): ";
  std::cin>>data.LIMIT_saltbridge_low;
  std::cout<<endl<<"LIMIT_saltbridge_high (500000): ";
  std::cin>>data.LIMIT_saltbridge_high;
  */
  std::cout<<endl<<"Trig0_threshold (1.223): ";
  std::cin>>data.Trig0_threshold;
  std::cout<<endl<<"Trig1_threshold (1.23): ";
  std::cin>>data.Trig1_threshold;
  std::cout<<endl<<"TrigVref (2.981): ";
  std::cin>>data.TrigVref;

  std::cout<<endl<<"relayCh1 (1): ";
  std::cin>>data.relayCh1;
  std::cout<<endl<<"relayCh2 (1): ";
  std::cin>>data.relayCh2;
  std::cout<<endl<<"relayCh3 (1): ";
  std::cin>>data.relayCh3;

  
  std::cout<<endl<<endl<<data.Print()<<endl<<" Do you want to send the above settings? ";
  bool send=false;
  std::cin>>send;
  std::cout<<endl<<endl;

  if(!send){
    std::cout<<"Quitting without sending"<<std::endl;
    return 0;
  }

  std::cout<<"sending"<<std::endl;
  data.Send_Config(&sock);  

  return 0;

}
