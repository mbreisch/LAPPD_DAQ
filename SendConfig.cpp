#include <iostream>
#include <string>
#include <zmq.hpp>
#include <SlowControlMonitor.h>


int main(){

    zmq::context_t context;

    zmq::socket_t sock(context, ZMQ_DEALER);
    sock.connect("tcp://127.0.0.1:4444");

    SlowControlMonitor data;

    std::cout<<"Please enter the LAPPD ID number: ";
    std::cin>>data.LAPPD_ID;  

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
                //HV settings
                data.HV_state_set = 0;
                data.HV_volts = 0;
                //LV settings
                data.LV_state_set = 0;
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
                data.relayCh1 = 0;
                data.relayCh2 = 0;
                data.relayCh3 = 0;
                break;
            }else if(mode==1)
            {
                //HV settings
                data.HV_state_set = 0;
                data.HV_volts = 0;
                //LV settings
                data.LV_state_set = 0;
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
                data.relayCh1 = 1;
                data.relayCh2 = 1;
                data.relayCh3 = 1;
                break;
            }else if(mode==2)
            {
                //HV settings
                data.HV_state_set = 0;
                data.HV_volts = 0;
                //LV settings
                data.LV_state_set = 1;
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
                data.relayCh1 = 1;
                data.relayCh2 = 1;
                data.relayCh3 = 1;
                break;
            }else if(mode==3)
            {
                //HV settings
                data.HV_state_set = 1;
                data.HV_volts = 500;
                //LV settings
                data.LV_state_set = 1;
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
                data.relayCh1 = 1;
                data.relayCh2 = 1;
                data.relayCh3 = 1;
                break;
            }else if(mode==4)
            {
                //HV settings
                data.HV_state_set = 1;
                data.HV_volts = 2350;
                //LV settings
                data.LV_state_set = 1;
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
                data.relayCh1 = 1;
                data.relayCh2 = 1;
                data.relayCh3 = 1;
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

    std::cout<<"------------------------------"<<std::endl;
    std::cout<<"Settings for LAPPD with ID: "<<data.LAPPD_ID<<std::endl;
    std::cout<<endl;
    //HV settings
    std::cout<<"HV_state_set: "<<data.HV_state_set<<std::endl;
    std::cout<<"HV_volts: "<<data.HV_volts<<std::endl;
    std::cout<<endl;
    //LV settings
    std::cout<<"LV_state_set: "<<data.LV_state_set<<std::endl;
    std::cout<<endl;
    //Emergency limits 
    std::cout<<"LIMIT_temperature_low (50): "<<data.LIMIT_temperature_low<<std::endl;
    std::cout<<"LIMIT_temperature_high (58): "<<data.LIMIT_temperature_high<<std::endl;
    std::cout<<"LIMIT_humidity_low (15): "<<data.LIMIT_humidity_low<<std::endl;
    std::cout<<"LIMIT_humidity_high (20): "<<data.LIMIT_humidity_high<<std::endl;
    std::cout<<endl;
    std::cout<<"LIMIT_Thermistor_temperature_low (7000): "<<data.LIMIT_Thermistor_temperature_low<<std::endl;
    std::cout<<"LIMIT_Thermistor_temperature_high (5800): "<<data.LIMIT_Thermistor_temperature_high<<std::endl;
    std::cout<<endl;
    std::cout<<"LIMIT_saltbridge_low (500000): "<<data.LIMIT_saltbridge_low<<std::endl;
    std::cout<<"LIMIT_saltbridge_high (400000): "<<data.LIMIT_saltbridge_high<<std::endl;
    std::cout<<endl;
    //Trigger
    std::cout<<"Trig0_threshold: "<<data.Trig0_threshold<<std::endl;
    std::cout<<"Trig1_threshold: "<<data.Trig1_threshold<<std::endl;
    std::cout<<"TrigVref: "<<data.TrigVref<<std::endl;
    std::cout<<endl;
    //Relays
    std::cout<<"relayCh1: "<<data.relayCh1<<std::endl;
    std::cout<<"relayCh2: "<<data.relayCh2<<std::endl;
    std::cout<<"relayCh3: "<<data.relayCh3<<std::endl;

    std::cout<<endl<<endl<<"Do you want to send the above settings? ";
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
