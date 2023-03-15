#include <iostream>
#include <string>
#include <zmq.hpp>
#include <SlowControlMonitor.h>
#include <map>
#include <fstream>


int main(){

    zmq::context_t context;

    zmq::socket_t sock(context, ZMQ_DEALER);
    sock.connect("tcp://127.0.0.1:4444");

    SlowControlMonitor data;

    std::cout<<"NOTICE!!!!"<<std::endl;
    std::cout<<"Recent changes: "<<std::endl;
    std::cout<<"- Saltbridge emergency limit is now 100000 in all presets!"<<std::endl;
    std::cout<<"- LAPPD ID is now set in Tool via configfile"<<std::endl;
    //std::cin>>data.LAPPD_ID;  

    int default_load = 0;
    std::cout<<"Do you want to load default settings (0: no |1: yes): ";
    std::cin>>default_load;

    if(default_load==1)
    {   
        //Emergency limits 
        float LIMIT_temperature_low = 50;
        float LIMIT_temperature_high = 58;
        float LIMIT_humidity_low = 15;
        float LIMIT_humidity_high = 25;
        float LIMIT_Thermistor_temperature_low = 7000;
        float LIMIT_Thermistor_temperature_high = 5800;
        float LIMIT_saltbridge_low = 400000;
        float LIMIT_saltbridge_high = 100000;

        //Triggerboard settings
        float Trig0_threshold = 1.223;
        float Trig1_threshold = 1.23;
        float TrigVref = 2.981;


        int mode = -1;
        while(true)
        {
            std::cout<<"What do you want to load: "<<std::endl;
            std::cout<<"(0) All off | No active senors, LV (ACDC) and HV (LAPPD)"<<std::endl;
            std::cout<<"(1) Relays only | No LV (ACDC) and HV (LAPPD)"<<std::endl;
            std::cout<<"(2) Relays + LV | No HV (LAPPD)"<<std::endl;
            std::cout<<"(3) Full on | Test voltage: 500V"<<std::endl;
            std::cout<<"(4) Operation | All on at 2350V"<<std::endl;
            std::cout<<"(5) Check Emergency local and file Limits/Trigger settings"<<std::endl;
            std::cout<<"(6) Save new Emergency Limits/Trigger settings"<<std::endl;
            std::cout<<"(7) Load Emergency Limits/Trigger settings"<<std::endl;
            std::cout<<"Enter: ";
            std::cin>>mode;

            //Emergency limits 
            data.LIMIT_temperature_low = LIMIT_temperature_low;
            data.LIMIT_temperature_high = LIMIT_temperature_high;
            data.LIMIT_humidity_low = LIMIT_humidity_low;
            data.LIMIT_humidity_high = LIMIT_humidity_high;
            data.LIMIT_Thermistor_temperature_low = LIMIT_Thermistor_temperature_low;
            data.LIMIT_Thermistor_temperature_high = LIMIT_Thermistor_temperature_high;
            data.LIMIT_saltbridge_low = LIMIT_saltbridge_low;
            data.LIMIT_saltbridge_high = LIMIT_saltbridge_high;

            //Triggerboard settings
            data.Trig0_threshold = 1.223;
            data.Trig1_threshold = 1.23;
            data.TrigVref = 2.981;

            if(mode==0)
            {
                //HV settings
                data.HV_state_set = 0;
                data.HV_volts = 0;
                //LV settings
                data.LV_state_set = 0;
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
                //Relaysettings
                data.relayCh1 = 1;
                data.relayCh2 = 1;
                data.relayCh3 = 1;
                break;
            }else if(mode==5)
            {
                //Emergency limits 
                std::cout<<"---------Local Limits-----------"<<std::endl;
                std::cout<<"LIMIT_temperature_low: "<<data.LIMIT_temperature_low<<std::endl;
                std::cout<<"LIMIT_temperature_high: "<<data.LIMIT_temperature_high<<std::endl;
                std::cout<<"LIMIT_humidity_low: "<<data.LIMIT_humidity_low<<std::endl;
                std::cout<<"LIMIT_humidity_high: "<<data.LIMIT_humidity_high<<std::endl;
                std::cout<<endl;
                std::cout<<"LIMIT_Thermistor_temperature_low: "<<data.LIMIT_Thermistor_temperature_low<<std::endl;
                std::cout<<"LIMIT_Thermistor_temperature_high: "<<data.LIMIT_Thermistor_temperature_high<<std::endl;
                std::cout<<endl;
                std::cout<<"LIMIT_saltbridge_low: "<<data.LIMIT_saltbridge_low<<std::endl;
                std::cout<<"LIMIT_saltbridge_high: "<<data.LIMIT_saltbridge_high<<std::endl;
                std::cout<<endl;
                //Trigger
                std::cout<<"Trig0_threshold: "<<data.Trig0_threshold<<std::endl;
                std::cout<<"Trig1_threshold: "<<data.Trig1_threshold<<std::endl;
                std::cout<<"TrigVref: "<<data.TrigVref<<std::endl;
                std::cout<<endl;
                //File limits
                map<int,std::string> LoadMap; 
                std::string line;
                std::fstream infile("./configfiles/SlowControl/LocalSettings", std::ios_base::in);
                if(!infile.is_open())
                {
                    std::cout<<"No local file yet"<<std::endl;
                    continue;
                }
                int lineNumber = 0;
                while(getline(infile, line))
                {
                    LoadMap[lineNumber] = line;
                    ++lineNumber;
                }
                infile.close();
                std::cout<<"---------File Limits-----------"<<std::endl;
                std::cout<<"LIMIT_temperature_low: "<<LoadMap[0]<<std::endl;
                std::cout<<"LIMIT_temperature_high: "<<LoadMap[1]<<std::endl;
                std::cout<<"LIMIT_humidity_low: "<<LoadMap[2]<<std::endl;
                std::cout<<"LIMIT_humidity_high: "<<LoadMap[3]<<std::endl;
                std::cout<<endl;
                std::cout<<"LIMIT_Thermistor_temperature_low: "<<LoadMap[4]<<std::endl;
                std::cout<<"LIMIT_Thermistor_temperature_high: "<<LoadMap[5]<<std::endl;
                std::cout<<endl;
                std::cout<<"LIMIT_saltbridge_low: "<<LoadMap[6]<<std::endl;
                std::cout<<"LIMIT_saltbridge_high: "<<LoadMap[7]<<std::endl;
                std::cout<<endl;
                //Trigger
                std::cout<<"Trig0_threshold: "<<LoadMap[8]<<std::endl;
                std::cout<<"Trig1_threshold: "<<LoadMap[9]<<std::endl;
                std::cout<<"TrigVref: "<<LoadMap[10]<<std::endl;
                std::cout<<endl;
            }else if(mode==6)
            {
                int mode6=-1;
                map<int,std::string> LoadMap; 
                map<int,std::string> SaveMap;
                std::string line;
                std::fstream infile("./configfiles/SlowControl/LocalSettings", std::ios_base::in);
                if(infile.is_open())
                {
                    int lineNumber = 0;
                    while(getline(infile, line))
                    {
                        LoadMap[lineNumber] = line;
                        ++lineNumber;
                    }
                    infile.close();
                    SaveMap = LoadMap; 
                }
                while(true)
                {
                    std::cout<<"Which one do you want to change"<<std::endl;
                    std::cout<<"(0) LIMIT_temperature_low, currently: "<<data.LIMIT_temperature_low<<std::endl;
                    std::cout<<"(1) LIMIT_temperature_high, currently: "<<data.LIMIT_temperature_high<<std::endl;
                    std::cout<<"(2) LIMIT_humidity_low, currently: "<<data.LIMIT_humidity_low<<std::endl;
                    std::cout<<"(3) LIMIT_humidity_high, currently: "<<data.LIMIT_humidity_high<<std::endl;
                    std::cout<<"(4) LIMIT_Thermistor_temperature_low, currently: "<<data.LIMIT_Thermistor_temperature_low<<std::endl;
                    std::cout<<"(5) LIMIT_Thermistor_temperature_high, currently: "<<data.LIMIT_Thermistor_temperature_high<<std::endl;
                    std::cout<<"(6) LIMIT_saltbridge_low, currently: "<<data.LIMIT_saltbridge_low<<std::endl;
                    std::cout<<"(7) LIMIT_saltbridge_high, currently: "<<data.LIMIT_saltbridge_high<<std::endl;
                    std::cout<<"(8) Trig0_threshold, currently: "<<data.Trig0_threshold<<std::endl;
                    std::cout<<"(9) Trig1_threshold, currently: "<<data.Trig1_threshold<<std::endl;
                    std::cout<<"(10) TrigVref, currently: "<<data.TrigVref<<std::endl;
                    std::cout<<"(11) SAVE ALL"<<std::endl;
                    std::cout<<"(12) EXIT"<<std::endl;
                    std::cout<<"Enter: ";
                    std::cin>>mode6;

                    if(mode6!=11 && mode6!=12)
                    {
                        int value;
                        std::cout<<"Enter value: ";
                        std::cin>>value;
                        SaveMap[mode6] = std::to_string(value);
                    }else if(mode6==11)
                    {
                        std::fstream outfile("./configfiles/SlowControl/LocalSettings", std::ios_base::out | std::ios_base::trunc);  
                        for(map<int,std::string>::iterator it = SaveMap.begin(); it != SaveMap.end(); ++it)
                        {
                            outfile << it->second << std::endl;
                        }
                        outfile.close();
                        break;
                    }else if(mode6==12)
                    {
                        break;
                    }
                }
                std::cout<<"ATTENTION! Even after saving new settings new ones will have to be loaded again!"<<std::endl;
            }else if(mode==7)
            {
                map<int,std::string> LoadMap; 
                std::string line;
                std::fstream infile("./configfiles/SlowControl/LocalSettings", std::ios_base::in);
                if(!infile.is_open())
                {
                    std::cout<<"No local file yet"<<std::endl;
                    continue;
                }
                int lineNumber = 0;
                while(getline(infile, line))
                {
                    LoadMap[lineNumber] = line;
                    ++lineNumber;
                }
                infile.close();

                //Emergency limits 
                data.LIMIT_temperature_low =  std::stof(LoadMap[0]);
                data.LIMIT_temperature_high =  std::stof(LoadMap[1]);
                data.LIMIT_humidity_low =  std::stof(LoadMap[2]);
                data.LIMIT_humidity_high =  std::stof(LoadMap[3]);
                data.LIMIT_Thermistor_temperature_low =  std::stof(LoadMap[4]);
                data.LIMIT_Thermistor_temperature_high =  std::stof(LoadMap[5]);
                data.LIMIT_saltbridge_low =  std::stof(LoadMap[6]);
                data.LIMIT_saltbridge_high =  std::stof(LoadMap[7]);      

                //Triggerboard settings
                data.Trig0_threshold = std::stof(LoadMap[8]);
                data.Trig1_threshold = std::stof(LoadMap[9]);
                data.TrigVref = std::stof(LoadMap[10]);       
            }else
            {
                std::cout<<"No valid preset choosen! Try again or ctrl+c"<<std::endl;
            }
            std::cout<<std::endl;
            std::cout<<"---------------------"<<std::endl;
            std::cout<<std::endl;
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
        std::cout<<endl<<"LIMIT_humidity_high (25): ";
        std::cin>>data.LIMIT_humidity_high;


        std::cout<<endl<<"LIMIT_Thermistor_temperature_low (7000): ";
        std::cin>>data.LIMIT_Thermistor_temperature_low;
        std::cout<<endl<<"LIMIT_Thermistor_temperature_high (5800): ";
        std::cin>>data.LIMIT_Thermistor_temperature_high;

        std::cout<<endl<<"LIMIT_saltbridge_low (500000): ";
        std::cin>>data.LIMIT_saltbridge_low;
        std::cout<<endl<<"LIMIT_saltbridge_high (200000/400000): ";
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
    std::cout<<"LIMIT_saltbridge_high (200000/400000): "<<data.LIMIT_saltbridge_high<<std::endl;
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
