#include "Factory.h"

Tool* Factory(std::string tool) {
Tool* ret=0;

if (tool=="DummyTool") ret=new DummyTool;
if (tool=="SC_Emergency") ret=new SC_Emergency;
if (tool=="SC_Poll") ret=new SC_Poll;
if (tool=="SC_Receive") ret=new SC_Receive;
if (tool=="SC_SetConfig") ret=new SC_SetConfig;
if (tool=="SC_Stream") ret=new SC_Stream;
if (tool=="SC_Poll_HV") ret=new SC_Poll_HV;
if (tool=="SC_Poll_LV") ret=new SC_Poll_LV;
if (tool=="SC_Poll_Photodiode") ret=new SC_Poll_Photodiode;
if (tool=="SC_Poll_Relays") ret=new SC_Poll_Relays;
if (tool=="SC_Poll_RHT") ret=new SC_Poll_RHT;
if (tool=="SC_Poll_Saltbridge") ret=new SC_Poll_Saltbridge;
if (tool=="SC_Poll_Thermistor") ret=new SC_Poll_Thermistor;
if (tool=="SC_Poll_Timestamp") ret=new SC_Poll_Timestamp;
if (tool=="SC_Poll_Trigger") ret=new SC_Poll_Trigger;
if (tool=="SC_LocalLog") ret=new SC_LocalLog;
return ret;
}
