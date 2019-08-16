//Include any Depdendencies

//Include Self
#include "logger.h"

//Member Functions
void Logger::write(std::string _msg){
  //Write the Message to Console
  std::cout<<"[Territory] "<<_msg<<std::endl;

  //Write to message log
}

void Logger::error(std::string _err){
  //Write the Error to Console
  std::cout<<"[Error] "<<_err<<std::endl;

  //Write to error log
}

void Logger::debug(std::string _deb){
  //Write the Debug Message to Console
  if(_debug){
    std::cout<<"[Debug] "<<_deb<<std::endl;
  }
}

void progress(std::string _msg, int cur, int max){


}
