/*

Logging Namespace

*/
#include <cstdarg>

namespace logger{

bool _debug = true;

void raw(){
  std::cout<<std::endl;
}

template <typename T>
void raw(T m){
  std::cout<<m<<std::endl;
}

template <typename T, typename... Types>
void raw(T m, Types... r){
  std::cout<<m;
  raw(r...);
}

/*

WRAPPED

*/

template <typename T>
void write(T m){
  std::cout<<"[Territory] ";
  raw(m);
}

template <typename T, typename... Types>
void write(T m, Types... r){
  std::cout<<"[Territory] ";
  raw(m, r...);
}

void error(std::string _err){
  //Write the Error to Console
  std::cout<<"[Error] "<<_err<<std::endl;
}

template <typename... Types>
void debug(Types... r){
  if(!_debug) return;
  std::cout<<"[Debug] ";
  raw(r...);
}

template <typename... Types>
void debug(bool f, Types... r){
  if(!f) return;  //Failed Debug Condition
  std::cout<<"[Debug] ";
  raw(r...);
}

template <typename T>
void progress(T d, T D){
  if(d != 0) std::cout<<"\r";
  std::cout<<"["<<d<<"/"<<D<<"]";

  for(int i = 0; i < 25; i++){
    if(i < 25*((float)d/(float)D)) std::cout<<"#";
    else std::cout<<"-";
  }
  std::cout<<"]";
  if(d == D-1) printf("\n");
}

};
