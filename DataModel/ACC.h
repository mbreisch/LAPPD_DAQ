#ifndef _ACC_H_INCLUDED
#define _ACC_H_INCLUDED

#include "ACDC.h" // load the ACDC class
#include "stdUSB.h" //load the usb class 
#include <algorithm>
#include <vector>
#include <map>

using namespace std;

#define SAFE_BUFFERSIZE 100000 //used in setup procedures to guarantee a proper readout 
#define NUM_CH 30 //maximum number of channels for one ACDC board
#define MAX_NUM_BOARDS 8 // maxiumum number of ACDC boards connectable to one ACC 


class ACC
{
public:
	/*------------------------------------------------------------------------------------*/
	/*--------------------------------Constructor/Deconstructor---------------------------*/
	/*ID 5: Constructor*/
	ACC();
	/*ID 6: Constructor*/
	~ACC();

	/*------------------------------------------------------------------------------------*/
	/*--------------------------------Local return functions------------------------------*/
	/*ID Nan: Returns vector of aligned ACDC indices*/
	vector<int> getAlignedIndices(){return alignedAcdcIndices;}
	/*ID Nan: Returns set triggermode */
	int getTriggermode(){return trigMode;} 
	/*ID Nan: Returns the raw data map*/
	map<int, vector<unsigned short>> returnRaw(){return map_raw;}
	void clearErrors(){errorcode.clear();}
	vector<unsigned int> returnErrors()
	{
		if(errorcode.size()==0)
		{
			errorcode.push_back(returncode);
		}
		return errorcode;
	}
	vector<unsigned short> readAccBuffer();

	/*------------------------------------------------------------------------------------*/
	/*-------------------------Local set functions for board setup------------------------*/
	/*-------------------Sets global variables, see below for description-----------------*/
	void setNumChCoin(unsigned int in){SELF_number_channel_coincidence = in;} 
	void setEnableCoin(int in){SELF_coincidence_onoff = in;} 
	void setThreshold(unsigned int in){SELF_threshold = in;} 
	void setPsecChipMask(vector<unsigned int> in){SELF_psec_channel_mask = in;} 
	void setPsecChannelMask(vector<unsigned int> in){SELF_psec_chip_mask = in;} 
	void setValidationStart(unsigned int in){validation_start=in;}
	void setValidationWindow(unsigned int in){validation_window=in;} 
	void setTriggermode(int in){trigMode = in;} 
	void setDetectionMode(int in, int source) 
	{
		if(source==2){ACC_detection_mode = in;}
		else if(source==3){ACDC_detection_mode = in;}
		else if(source==4){SELF_detection_mode = in;}
	}	
	void setSign(int in, int source) 
	{
		if(source==0){ACC_sign = in;}
		else if(source==1){ACDC_sign = in;}
		else if(source==2){SELF_sign = in;}
	}

	/*------------------------------------------------------------------------------------*/
	/*-------------------------Local set functions for board setup------------------------*/
	/*ID 7: Function to completly empty the USB line until the correct response is received*/
	bool emptyUsbLine(); 
	/*ID 8: USB return*/
	stdUSB* getUsbStream(); 
	/*ID:9 Create ACDC class instances for each connected ACDC board*/
	int createAcdcs(); 
	/*ID 10: Clear all ACDC class instances*/
	void clearAcdcs(); 
	/*ID:11 Queries the ACC for information about connected ACDC boards*/
	vector<int> whichAcdcsConnected(); 
	/*ID 12: Set up the software trigger*/
	void setSoftwareTrigger(unsigned int boardMask); 
	/*ID 13: Fires the software trigger*/
	void softwareTrigger(); 
	/*ID 14: Software read function*/
	int readAcdcBuffers(); 
	/*ID 15: Main listen fuction for data readout. Runs for 5s before retuning a negative*/
	int listenForAcdcData(int trigMode); 
	/*ID 16: Used to dis/enable transfer data from the PSEC chips to the buffers*/
	void enableTransfer(int onoff=0); 
	/*ID 17: Main init function that controls generalk setup as well as trigger settings*/
	int initializeForDataReadout(int trigMode = 0,unsigned int boardMask = 0xFF, int calibMode = 0); 
	/*ID 18: Tells ACDCs to clear their ram.*/ 	
	void dumpData(unsigned int boardMask); 
	/*ID 19: Pedestal setting procedure.*/
	bool setPedestals(unsigned int boardmask, unsigned int chipmask, unsigned int adc); 
	/*ID 20: Switch for the calibration input on the ACC*/
	void toggleCal(int onoff, unsigned int channelmask = 0x7FFF,  unsigned int boardMask=0xFF); 
	/*ID 21: Set up the hardware trigger*/
	void setHardwareTrigSrc(int src, unsigned int boardMask = 0xFF); 
	/*ID 22: Special function to check the ports for connections to ACDCs*/
	void connectedBoards(); 
	/*ID 23: Wakes up the USB by requesting an ACC info frame*/
	void usbWakeup(); 
	/*ID 24: Special function to check connected ACDCs for their firmware version*/ 
	void versionCheck();

	/*------------------------------------------------------------------------------------*/
	/*--------------------------------Public gloabl variables-----------------------------*/
	vector<unsigned int> errorcode;
	unsigned int returncode = 0x00000000;

private:
	/*------------------------------------------------------------------------------------*/
	/*---------------------------------Load neccessary classes----------------------------*/
	stdUSB* usb; //calls the usb class for communication
	vector<ACDC*> acdcs; //a vector of active acdc boards. 

	/*------------------------------------------------------------------------------------*/
	/*-----------------------------Neccessary global variables----------------------------*/
	int ACC_detection_mode; //var: ACC detection mode (level or edge)
	int ACC_sign; //var: ACC sign (normal or inverted)
	int ACDC_detection_mode; //var: ACDC detection mode (level or edge)
	int ACDC_sign; //var: ACDC sign (normal or inverted)
	int SELF_detection_mode; //var: self trigger detection mode (level or edge)
	int SELF_sign; //var: self trigger sign (normal or inverted)
	int SELF_coincidence_onoff; //var: flag to enable self trigger coincidence
	int trigMode; //var: decides the triggermode
	unsigned int SELF_number_channel_coincidence; //var: number of channels required in coincidence for the self trigger
	unsigned int SELF_threshold; //var: threshold for the selftrigger
	unsigned int validation_start; //var: validation window for some triggermodes
	unsigned int validation_window; //var: validation window for some triggermodes
	vector<unsigned short> lastAccBuffer; //most recently received ACC buffer
	vector<int> alignedAcdcIndices; //number relative to ACC index (RJ45 port) corresponds to the connected ACDC boards
	vector<unsigned int> SELF_psec_channel_mask; //var: PSEC channels active for self trigger
	vector<unsigned int> SELF_psec_chip_mask; //var: PSEC chips actove for self trigger
	map<int, vector<unsigned short>> map_raw;
	bool usbcheck;
	static void got_signal(int);
};

#endif