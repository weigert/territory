//Forward Declarations
#pragma once
#include "../forward/logger.fwd.h"
//Depdendencies

class Logger{
  public:
    //Output Messages to Console
    void write(std::string _msg); //Simply write something to console
    void error(std::string _err); //This is specifically an error, and will print as such
    //Special Types of Outputs
    void debug(std::string _deb);
    void progress(std::string _msg, int cur, int max);  //Progress Bar
    bool _debug = false;
};
