#ifndef VIDEOOUTPUTGL_INCLUDED
#define VIDEOOUTPUTGL_INCLUDED

#include "VideoOutputX11Base.h"

namespace X11
{

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include <X11/extensions/xf86vmode.h>

class Canvas;

class VideoOutputGl : public VideoOutputX11Base
{
public:
  VideoOutputGl();
  ~VideoOutputGl();

  void fullscreen(bool fs);
  void destroy();
  bool init(int xRes, int yRes, bool fullscreen);
  void render(const MatrixType<RGBA> &image);


protected:

  void onResize(int sizeX, int sizeY);
  bool createGLXContext();               
  int initGl(int xRes, int yRes);
  void resizeGl(int sizeX, int sizeY);
  void destroyGLXContext();


private:

  GLXContext _XGLXContext;
  int _frameSizeX;
  int _frameSizeY;
  int _frameSize;
  bool _glInitialized;
  float _texSizeX, _texSizeY;

};
}

#endif
