#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <linux/types.h>

#include <string>
#include <iostream>

/* #include </usr/src/linux/drivers/media/video/zoran.h> */
/*                                                       */
/* extern "C" {                                          */
/* #include <jpegutils.h>                                */
/*                                                       */
/* }                                                     */


#include "Gear_VideoInput.h"
#include "Engine.h"

#include "GearMaker.h"



Register_Gear(MAKERGear_VideoInput, Gear_VideoInput, "VideoInput");

const std::string Gear_VideoInput::SETTING_DEVICE = "Device";
const std::string Gear_VideoInput::DEFAULT_DEVICE = "/dev/video0";

unsigned char *Gear_VideoInput::_data=NULL;

Gear_VideoInput::Gear_VideoInput(Engine *engine, std::string name) : Gear(engine, "VideoInput", name),
_sizeX(0),
_sizeY(0),
_device(0),
_bufferBGRA(NULL)
{      
  addPlug(_VIDEO_OUT = new PlugOut<VideoTypeRGBA>(this, "Out"));

  _settings.add(Property::STRING, SETTING_DEVICE)->valueStr(DEFAULT_DEVICE);    

  resetInputDevice();
}

Gear_VideoInput::~Gear_VideoInput()
{
}

bool Gear_VideoInput::ready()
{
  return(_VIDEO_OUT->connected());
}

void Gear_VideoInput::init()
{
  initInputDevice();
}

void Gear_VideoInput::onUpdateSettings()
{
  initInputDevice();
}

void Gear_VideoInput::resetInputDevice()
{
    if (_device!=0)
        close(_device);
    
    _sizeX=0;
    _sizeY=0;

    if (_bufferBGRA!=NULL)
    {
        free(_bufferBGRA);
        _bufferBGRA=NULL;
    }

    memset(&_vidCap, 0, sizeof(video_capability));
    memset(&_vidWin, 0, sizeof(video_window));
    memset(&_vidPic, 0, sizeof(video_picture));    
}

void Gear_VideoInput::initInputDevice()
{
    resetInputDevice();

    _device = open(_settings.get(SETTING_DEVICE)->valueStr().c_str(), O_RDWR | O_NONBLOCK);

    if (_device<=0)
    {
        std::cout << "fail to open device " << _settings.get(SETTING_DEVICE)->valueStr().c_str() << std::endl;
        return;
    }

    //get info
    ioctl(_device, VIDIOCGCAP, &_vidCap);
    ioctl(_device, VIDIOCGWIN, &_vidWin);
    ioctl(_device, VIDIOCGPICT, &_vidPic);
    
    std::cout << "palette : " << _vidPic.palette << std::endl;

    _vidPic.palette = VIDEO_PALETTE_RGB32;    
    ioctl(_device, VIDIOCSPICT, &_vidPic);

    
    _vidWin.clips = _vidClips;
    _vidWin.clipcount = 0;
    _sizeX = _vidCap.maxwidth;
    _sizeY = _vidCap.maxheight;

    //show info
    std::cout << "-- device info --" << std::endl;
    std::cout << "name   : " << _vidCap.name << std::endl;
    std::cout << "type   : " << _vidCap.type << std::endl;
    std::cout << "size X : " << _sizeX << std::endl;
    std::cout << "size Y : " << _sizeY << std::endl;
    std::cout << "bpp    : " << _vidPic.depth << std::endl;

    //prepare and allocate mmap
/*     ioctl(_device, VIDIOCGMBUF, &_vidMBuf);                                                       */
/*                                                                                                   */
/*     _bufferRGB24 = (unsigned char*) mmap(NULL, _vidMBuf.size, PROT_READ, MAP_SHARED, _device, 0); */
/*                                                                                                   */
/*     _vidMMap.format = VIDEO_PALETTE_RGB24;                                                        */
/*     _vidMMap.frame  = 0;                                                                          */
/*     _vidMMap.width  = _sizeX;                                                                     */
/*     _vidMMap.height = _sizeY;                                                                     */


    _bufferBGRA = (unsigned char*)malloc(_sizeX * _sizeY * 4);
    _VIDEO_OUT->type()->resize(_sizeX, _sizeY);
}

void Gear_VideoInput::runVideo()
{
    _outData = (unsigned char*) _VIDEO_OUT->type()->data();
                   
    int len=0;
          
    len = read(_device, _bufferBGRA, _sizeX * _sizeY * 4);
    
    //convert BGRA -> RGBA
    //need optimisation
    if (len)
    {                
        int imgSize=_sizeX*_sizeY;
        _tempOutData = _outData;
        _tempInData = _bufferBGRA;
        for (int i=0;i<imgSize;i++)
        {
            *(_tempOutData) = *(_tempInData+2);
            *(_tempOutData+1) = *(_tempInData+1);
            *(_tempOutData+2) = *(_tempInData);

            _tempOutData+=4;
            _tempInData+=4;
        }

    }
}










/* void Gear_VideoInput::resetInputDevice()                                                                                   */
/* {                                                                                                                          */
/*   if (_device!=0)                                                                                                          */
/*     close(_device);                                                                                                        */
/*                                                                                                                            */
/*   _sizeX=0;                                                                                                                */
/*   _sizeY=0;                                                                                                                */
/*                                                                                                                            */
/*     if (_bufferBGRA!=NULL)                                       */
/*     {                                                            */
/*          munmap(_bufferBGRA, _sizeX*_sizeY*VIDEO_PALETTE_RGB32); */
/*         _bufferBGRA=NULL;                                        */
/*     }                                                            */
/*                                                                  */
/*     memset(&_vidCap, 0, sizeof(video_capability));               */
/*     memset(&_vidWin, 0, sizeof(video_window));                   */
/*     memset(&_vidPic, 0, sizeof(video_picture));                  */
/* }                                                                                                                          */
/*                                                                                                                            */
/*                                                                                                                            */
/* void Gear_VideoInput::prePlay()                                                                                            */
/* {                                                                                                                          */
/*   //lavrec_main(_lavrecInfo);                                                                                              */
/*   //lavrec_start(_lavrecInfo);                                                                                             */
/*                                                                                                                            */
/* }                                                                                                                          */
/*                                                                                                                            */
/* void Gear_VideoInput::videoCallback(uint8_t *video, long size, long count) */
/* {                                                                          */
/*   std::cout << size << std::endl;                                          */
/*                                                                            */
/*                                                                            */
/*   unsigned char *data1 = (unsigned char*)malloc(160*120*4);                */
/*   unsigned char *data2 = (unsigned char*)malloc(160*120*4);                */
/*   decode_jpeg_raw(video, size, 0, 420, 160, 120, _data, data1, data2);     */
/*                                                                            */
/* }                                                                          */

/* void audioCallback(uint8_t *audio, long sampes) */
/* {                                               */
/*                                                 */
/* }                                               */

/* void Gear_VideoInput::initInputDevice() */
/* {                                       */
/*     _lavrecInfo = lavrec_malloc();               */
/*     _lavrecInfo->video_dev = "/dev/video0";      */
/*     _lavrecInfo->video_captured = videoCallback; */
/*     _lavrecInfo->audio_captured = audioCallback; */
/*     _lavrecInfo->use_read = 1;                   */
/*     _lavrecInfo->video_norm = 1;                 */
/*     rect *rec=(rect*)malloc(sizeof(rect));       */
/*     rec->x=0;                                    */
/*     rec->y=0;                                    */
/*     rec->w=640;                                  */
/*     rec->h=480;                                  */
/*     //_lavrecInfo->geometry = rec;               */
/*     _lavrecInfo->quality = 100;                  */
/*     _lavrecInfo->video_format = 'j';             */
/*                                                                                                                            */
/*                                                                                                                            */
/*   resetInputDevice();                                                                                                      */
/*                                                                                                                            */
/*   _device = open(_settings.get(SETTING_DEVICE)->valueStr().c_str(), O_RDWR);                                               */
/*                                                                                                                            */
/*   if (_device<=0)                                                                                                          */
/*   {                                                                                                                        */
/*     std::cout << "fail to open device " << _settings.get(SETTING_DEVICE)->valueStr().c_str() << std::endl;                 */
/*     return;                                                                                                                */
/*   }                                                                                                                        */
/*                                                                                                                            */
/*   //get info                                                                                                               */
/*   ioctl(_device, VIDIOCGCAP, &_vidCap);                                                                                    */
/*   ioctl(_device, VIDIOCGWIN, &_vidWin);                                                                                    */
/*   _vidWin.width=320;                                                                                                       */
/*   _vidWin.height=200;                                                                                                      */
/*   ioctl(_device, VIDIOCSWIN, &_vidWin);                                                                                    */
/*   ioctl(_device, VIDIOCGPICT, &_vidPic);                                                                                   */
/*   ioctl(_device, VIDIOCGFBUF, &_vidBuf);                                                                                   */
/*   //ioctl(_device, VIDIOCGTUNER, &_vidTuner);                                                                              */
/*   struct video_channel _vidChan;                                                                                           */
/*   _vidChan.channel = 0;                                                                                                    */
/*   ioctl(_device, VIDIOCGCHAN, &_vidChan);                                                                                  */
/*   //std::cout << "channel: " << _vidChan.channel << std::endl << _vidChan.name << std::endl << _vidChan.type << std::endl; */
/*   _vidChan.norm = 1;                                                                                                       */
/*   ioctl(_device, VIDIOCSCHAN, &_vidChan);                                                                                  */
/*                                                                                                                            */
/*                                                                                                                            */
/*                                                                                                                            */
/*   std::cout << "palette : " << _vidPic.palette << std::endl;                                                               */
/*                                                                                                                            */
/*   _vidPic.palette = VIDEO_PALETTE_RGB32;                                                                                   */
/*                                                                                                                            */
/*   ioctl(_device, VIDIOCSPICT, &_vidPic);                                                                                   */
/*                                                                                                                            */
/*                                                                                                                            */
/*   _vidWin.clips = _vidClips;                                                                                               */
/*   _vidWin.clipcount = 0;                                                                                                   */
/*   _sizeX = _vidCap.maxwidth;                                                                                               */
/*   _sizeY = _vidCap.maxheight;                                                                                              */
/*                                                                                                                            */
/*   //show info                                                                                                              */
/*   std::cout << "-- device info --" << std::endl;                                                                           */
/*   std::cout << "name   : " << _vidCap.name << std::endl;                                                                   */
/*   std::cout << "type   : " << _vidCap.type << std::endl;                                                                   */
/*   std::cout << "size X : " << _sizeX << std::endl;                                                                         */
/*   std::cout << "size Y : " << _sizeY << std::endl;                                                                         */
/*   std::cout << "bpp    : " << _vidPic.depth << std::endl;                                                                  */
/*   std::cout << "Overlay : " << ((_vidCap.type & VID_TYPE_OVERLAY) ? "Y" : "N") << std::endl;                               */
/*                                                                                                                            */
/*   struct zoran_params params;                                 */
/*   ioctl(_device, BUZIOC_G_PARAMS, &params);                   */
/*   std::cout << "decimationH: " << params.HorDcm << std::endl; */
/*   std::cout << "decimationV: " << params.VerDcm << std::endl; */
/*   std::cout << "quality: " << params.quality << std::endl;    */
/*   std::cout << "xres: " << params.img_width << std::endl;     */
/*   std::cout << "yres: " << params.img_height << std::endl;    */
/*                                                               */
/*   params.decimation=2;                                        */
/*   ioctl(_device, BUZIOC_S_PARAMS, &params);                   */

/*     _vidBuf.width = _sizeX;                */
/*     _vidBuf.height = _sizeY;               */
/*     ioctl(_device, VIDIOCSFBUF, &_vidBuf); */
/*                                                                                                                            */
/*   int temp=0;                                                                                                              */
/*   ioctl(_device, VIDIOCCAPTURE, &temp);                                                                                    */
/*                                                                                                                            */
/*   //prepare and allocate mmap                                                                                              */
/*   if (ioctl(_device, VIDIOCGMBUF, &_vidMBuf)==-1)                                                      */
/*   {                                                                                                    */
/*     std::cout << "fdgdfgfd" << std::endl;                                                              */
/*     perror("VIDIOCGMBUF");                                                                             */
/*   }                                                                                                    */
/*   std::cout << "buffer size : " << _vidMBuf.size << std::endl;                                         */
/*                                                                                                        */
/*   _bufferBGRA = (unsigned char*) mmap(0, _vidMBuf.size, PROT_READ|PROT_WRITE, MAP_SHARED, _device, 0); */
/*                                                                                                        */
/*   if ((int)_bufferBGRA < 0)                                                                            */
/*   {                                                                                                    */
/*     std::cout << "fail to create mmap!" << std::endl;                                                  */
/*     _bufferBGRA=NULL;                                                                                  */
/*     return;                                                                                            */
/*   }                                                                                                    */
/*                                                                                                        */
/*   _vidMMap.format = VIDEO_PALETTE_RGB32;                                                               */
/*   _vidMMap.frame  = 0;                                                                                 */
/*   _vidMMap.width  = _sizeX;                                                                            */
/*   _vidMMap.height = _sizeY;                                                                            */
/*                                                                                                                            */
/*   _VIDEO_OUT->type()->image()->resize(_sizeY, _sizeX);                                                                     */
/*   _data = (unsigned char*)malloc(_sizeX*_sizeY*4);                                                                         */
/* }                                                                                                                          */
/*                                                                                                                            */
/* void Gear_VideoInput::runVideo()                                                                                           */
/* {                                                                                                                          */
/*   _outData = (unsigned char*) _VIDEO_OUT->type()->image()->data();                                                         */
/*                                                                                                                            */
/*   //read(_device, _data, _sizeX*_sizeY*4);             */
/*   if (ioctl(_device, VIDIOCMCAPTURE, &(_vidMMap))<0)   */
/*   {                                                    */
/*     perror("VIDIOCMCAPTURE");                          */
/*     //return;                                          */
/*   }                                                    */
/*                                                        */
/*   if (ioctl(_device, VIDIOCSYNC, &(_vidMMap.frame))<0) */
/*   {                                                    */
/*     perror("VIDIOCSYNC");                              */
/*     //return(-1);                                      */
/*   }                                                    */
/*                                                        */
/*                                                        */
/*   //convert BGRA -> RGBA                               */
/*   //need optimisation                                  */
/*   int imgSize=_sizeX*_sizeY;                           */
/*   _tempOutData = _outData;                             */
/*   _tempInData =  _bufferBGRA;                          */
/*   for (int i=0;i<imgSize;i++)                          */
/*   {                                                    */
/*     *(_tempOutData) = *(_tempInData);                  */
/*     *(_tempOutData+1) = *(_tempInData+1);              */
/*     *(_tempOutData+2) = *(_tempInData+2);              */
/*                                                        */
/*     _tempOutData+=4;                                   */
/*     _tempInData+=4;                                    */
/*   }                                                    */
/*                                                                                                                            */
/*                                                                                                                            */
/*   int len=0;                                                                                                               */
/*                                                                                                                            */
/*   len = read(_device, _data, _sizeX * _sizeY * 4);                                                                         */
/*                                                                                                                            */
/*   //convert BGRA -> RGBA                                                                                                   */
/*   //need optimisation                                                                                                      */
/*   if (len)                                                                                                                 */
/*   {                                                                                                                        */
/*       int imgSize=_sizeX*_sizeY;                                                                                           */
/*       _tempOutData = _outData;                                                                                             */
/*       _tempInData = _data;                                                                                                 */
/*       for (int i=0;i<imgSize;i++)                                                                                          */
/*       {                                                                                                                    */
/*           *(_tempOutData) = *(_tempInData+2);                                                                              */
/*           *(_tempOutData+1) = *(_tempInData+1);                                                                            */
/*           *(_tempOutData+2) = *(_tempInData);                                                                              */
/*                                                                                                                            */
/*           _tempOutData+=4;                                                                                                 */
/*           _tempInData+=4;                                                                                                  */
/*       }                                                                                                                    */
/*                                                                                                                            */
/*   }                                                                                                                        */
/*                                                                                                                            */
/*                                                                                                                            */
/* }                                                                                                                          */
/*                                                                                                                            */
/*                                                                                                                            */

