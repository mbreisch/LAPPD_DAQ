#include "SC_Poll_Timestamp.h"

SC_Poll_Timestamp::SC_Poll_Timestamp():Tool(){}


bool SC_Poll_Timestamp::Initialise(std::string configfile, DataModel &data)
{
    if(configfile!="")  m_variables.Initialise(configfile);
    //m_variables.Print();

    m_data= &data;
    m_log= m_data->Log;

    if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;

    return true;
}


bool SC_Poll_Timestamp::Execute()
{
    std::time_t tp = std::time(NULL);
    std::tm * ts = std::localtime(&tp); 
    int m = ts->tm_mon;
    int d = ts->tm_mday;
    
    unsigned long long UTCCONV = 0;
    
    if (m<3)
    {
        UTCCONV = UTCCONV_base*6;
    }
    else if (m==3)
    {
        if (d<7)
        {
            UTCCONV = UTCCONV_base*6;
        }
        else if (d>=7)
        {
            UTCCONV = UTCCONV_base*5;
        }
    } 
    else if (m>3 && m<11)
    {
        UTCCONV = UTCCONV_base*5;
    }
    else if (m>=11 && m<=12)
    {
        UTCCONV = UTCCONV_base*6;
    }

    //Get Timestamp
    unsigned long long timeSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    m_data->SCMonitor.timeSinceEpochMilliseconds = to_string(timeSinceEpoch-UTCCONV); 

    return true;
}


bool SC_Poll_Timestamp::Finalise()
{
    return true;
}
