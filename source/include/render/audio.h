#pragma once
#include "../forward/audio.fwd.h"



//Soundbytes and Music Stores in here, as well as calling functions.
class Audio{
public:
  //Storage for unprocessed soundbytes
  std::vector<SoundByte> unprocessed;

  //Vector containing the guy
  Mix_Chunk *med = NULL;
  Mix_Chunk *hit = NULL;

  bool Init();
  bool cleanup();
  void process();
};
