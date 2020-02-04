/*

Logging Namespace

*/

namespace logger{

bool _debug = false;

void write(std::string _msg){
  //Write the Message to Console
  std::cout<<"[Territory] "<<_msg<<std::endl;
}

void error(std::string _err){
  //Write the Error to Console
  std::cout<<"[Error] "<<_err<<std::endl;
}

void debug(std::string _deb){
  if(!_debug) return;

  //Write the Debug Message to Console
  std::cout<<"[Debug] "<<_deb<<std::endl;
}

void progress(std::string _msg, int cur, int max){

}

};
