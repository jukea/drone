// VideoOutputMaker.cpp: implementation of the VideoOutputMaker class.
//
//////////////////////////////////////////////////////////////////////

#include "VideoOutputMaker.h"
#include <assert.h>
#include <iostream>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

std::map<std::string, VideoOutputMaker*> *VideoOutputMaker::_Registry=NULL;

VideoOutputMaker::VideoOutputMaker(std::string type)
{
  static bool registry_instanciated=false;

  if (!registry_instanciated)
  {
    _Registry = new std::map<std::string, VideoOutputMaker*>();
    registry_instanciated=true;
  }

  _Registry->insert(std::make_pair(type, this));
}

VideoOutput* VideoOutputMaker::makeVideoOutput(std::string type)
{    
  if (_Registry==NULL)
    return NULL;
  
  VideoOutputMaker *VideoOutputmaker= _Registry->find(type)->second;

  if (VideoOutputmaker == NULL)
    return NULL;


  return VideoOutputmaker->internalMakeVideoOutput();
}

void VideoOutputMaker::getAllVideoOutputsType(std::vector<std::string> &videoOutputsType)
{
  if (_Registry==NULL)
    return;
  
  for (std::map<std::string, VideoOutputMaker*>::iterator it=_Registry->begin(); it != _Registry->end(); ++it)
  {
    videoOutputsType.push_back(it->first);
  }
}
