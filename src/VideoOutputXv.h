#ifndef VIDEOOUTPUTXV_INCLUDED
#define VIDEOOUTPUTXV_INCLUDED

#include "VideoOutputX11Base.h"

namespace X11
{

extern "C"{
#include <X11/extensions/Xvlib.h>
}

class Canvas;

class VideoOutputXv : public VideoOutputX11Base
{
public:
  VideoOutputXv();
  ~VideoOutputXv();

  void fullscreen(bool fs);
  void destroy();
  bool init(int xRes, int yRes, bool fullscreen);
  void render(const MatrixType<RGBA> &image);

protected:
  struct XvImageFormatValuesPortPair
  {
    XvImageFormatValues format;
    XvPortID port;
  } ;

  bool checkXvExtension();
  bool selectXvPort();
  void showFormat(const XvImageFormatValues &format);
  void unGrabXvPort();
  XvImage* createXvImage(int sizeX, int sizeY);
  void destroyXvImage();

  

private:

  XvImageFormatValuesPortPair _selectedFormat;

  XvImage* _xvImage;
  XShmSegmentInfo *_shmInfo;
  int _frameSizeX;
  int _frameSizeY;
  int _frameSize;

};
}

#endif
