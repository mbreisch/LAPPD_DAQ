#include <iostream>
#include <string>
#include <zmq.hpp>
#include <SlowControlMonitor.h>

void setValues(hv_set,hv_v,lv_set,r1,r2,r3)
{
    //HV settings
    data.HV_state_set = hv_set;
    data.HV_volts = hv_v;
    //LV settings
    data.LV_state_set = lv_set;
    //Emergency limits 
    data.LIMIT_temperature_low = 50;
    data.LIMIT_temperature_high = 58;
    data.LIMIT_humidity_low = 15;
    data.LIMIT_humidity_high = 20;
    data.LIMIT_Thermistor_temperature_low = 7000;
    data.LIMIT_Thermistor_temperature_high = 5800;
    data.LIMIT_saltbridge_low = 500000;
    data.LIMIT_saltbridge_high = 400000;
    //Triggerboard settings
    data.Trig0_threshold = 1.223;
    data.Trig1_threshold = 1.23;
    data.TrigVref = 2.981;
    //Relaysettings
    data.relayCh1 = r1;
    data.relayCh2 = r2;
    data.relayCh3 = r2;
}

int main(){

    zmq::context_t context;

    zmq::socket_t sock(context, ZMQ_DEALER);
    sock.connect("tcp://127.0.0.1:4444");


    SlowControlMonitor data;

    int default_load = 0;
    std::cout<<"Do you want to load default settings (0: no |1: yes): ";
    std::cin>>default_load;

    if(default_load==1)
    {   
        int mode = -1;
        std::cout<<"Do you want to load: "<<std::endl;
        std::cout<<"(0) All off | No active senors, LV (ACDC) and HV (LAPPD)"<<std::endl;
        std::cout<<"(1) Relays only | No LV (ACDC) and HV (LAPPD)"<<std::endl;
        std::cout<<"(2) Relays + LV | No HV (LAPPD)"<<std::endl;
        std::cout<<"(3) Full on | Test voltage: 500V"<<std::endl;
        std::cout<<"(4) Operation | All on at 2350V"<<std::endl;

        while(true)
        {
            std::cout<<"Enter: ";
            std::cin>>mode;

            if(mode==0)
            {
                setValues(0,0,0,0,0,0);
                break;
            }else if(mode==1)
            {
                setValues(0,0,0,1,1,1);
                break;
            }else if(mode==2)
            {
                setValues(0,0,1,1,1,1);
                break;
            }else if(mode==3)
            {
                setValues(1,500,1,1,1,1);
                break;
            }else if(mode==4)
            {
                setValues(1,2350,1,1,1,1);
                break;
            }else
            {
                std::cout<<"No valid preset choosen! Try again or ctrl+c"<<std::endl;
            }
        }
    }else if(default_load==0)
    {
        //HV settings
        std::cout<<"HV_state_set (0): ";
        std::cin>>data.HV_state_set;
        std::cout<<endl<<"HV_volts (0): ";
        std::cin>>data.HV_volts;

        //LV settings
        std::cout<<endl<<"LV_state_set (0): ";
        std::cin>>data.LV_state_set;

        //Emergency limits 
        std::cout<<endl<<"LIMIT_temperature_low (50): ";
        std::cin>>data.LIMIT_temperature_low;
        std::cout<<endl<<"LIMIT_temperature_high (58): ";
        std::cin>>data.LIMIT_temperature_high;
        std::cout<<endl<<"LIMIT_humidity_low (15): ";
        std::cin>>data.LIMIT_humidity_low;
        std::cout<<endl<<"LIMIT_humidity_high (20): ";
        std::cin>>data.LIMIT_humidity_high;


        std::cout<<endl<<"LIMIT_Thermistor_temperature_low (7000): ";
        std::cin>>data.LIMIT_Thermistor_temperature_low;
        std::cout<<endl<<"LIMIT_Thermistor_temperature_high (5800): ";
        std::cin>>data.LIMIT_Thermistor_temperature_high;

        std::cout<<endl<<"LIMIT_saltbridge_low (500000): ";
        std::cin>>data.LIMIT_saltbridge_low;
        std::cout<<endl<<"LIMIT_saltbridge_high (400000): ";
        std::cin>>data.LIMIT_saltbridge_high;

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
    }else
    {
        std::cout<<"Only enter 0 or 1! Quitting program!"<<std::endl;
        return 0;
    }
  
    std::cout<<endl<<endl<<data.Print()<<endl<<" Do you want to send the above settings? ";
    bool send=false;
    std::cin>>send;
    std::cout<<endl<<endl;

    if(!send)
    {
        std::cout<<"Quitting without sending"<<std::endl;
        return 0;
    }

    std::cout<<"sending"<<std::endl;
    data.Send_Config(&sock);  

    return 0;
}
