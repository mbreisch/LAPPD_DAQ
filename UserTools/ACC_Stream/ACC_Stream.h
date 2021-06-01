#ifndef ACC_Stream_H
#define ACC_Stream_H

#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <chrono>

#include "Tool.h"


/**
 * \class Stream
 *
 * This is a balnk template for a Tool used by the script to generate a new custom tool. Please fill out the descripton and author information.
*
* $Author: B.Richards $
* $Date: 2019/05/28 10:44:00 $
* Contact: b.richards@qmul.ac.uk
*/
class ACC_Stream: public Tool {


 public:

  ACC_Stream(); ///< Simple constructor
  bool Initialise(std::string configfile,DataModel &data); ///< Initialise Function for setting up Tool resorces. @param configfile The path and name of the dynamic configuration file to read in. @param data A reference to the transient data class used to pass information between Tools.
  bool Execute(); ///< Executre function used to perform Tool perpose. 
  bool Finalise(); ///< Finalise funciton used to clean up resorces.

  std::chrono::high_resolution_clock m_clock;	
  unsigned long long getTime()
  {
      auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(m_clock.now().time_since_epoch()).count();
      return (unsigned long long)time;
  }
 private:

  zmq::socket_t* sock;
  boost::posix_time::ptime first;
  

};


#endif
