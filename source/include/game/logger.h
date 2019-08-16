//Forward Declarations
#pragma once
#include "../forward/logger.fwd.h"
//Depdendencies

class Logger{
  public:
    //Output Messages to Console
    void write(std::string _msg); //Simply write something to console
    void error(std::string _err); //This is specifically an error, and will print as such
    void debug(std::string _deb); //This is a debug message and will appear if debug is true

    //Special Types of Outputs
    void progress(std::string _msg, int cur, int max);  //Progress Bar

  private:
    bool _debug = false;
};
