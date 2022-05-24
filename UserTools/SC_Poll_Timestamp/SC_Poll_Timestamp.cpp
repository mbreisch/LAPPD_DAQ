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
    //Get Timestamp
    unsigned long long timeSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    m_data->SCMonitor.timeSinceEpochMilliseconds = to_string(timeSinceEpoch-UTCCONV); 

    return true;
}


bool SC_Poll_Timestamp::Finalise()
{
    return true;
}
