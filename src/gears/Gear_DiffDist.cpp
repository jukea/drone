#include "Gear_DiffDist.h"
#include "Engine.h"

#include <iostream>

#include "GearMaker.h"


Register_Gear(MAKERGear_DiffDist, Gear_DiffDist, "DiffDist")

unsigned long xcng(void){  // takes 60 nanosecs, passes all tests
  static unsigned long x=123456789,y=521288629,z,w;
  z=x+y; x=y;  w=(z<<8); y=w+17*(z>>24);
  if (z<x) y=y+4352;
  if (y<w) y=y+17;
  return( y );}

Gear_DiffDist::Gear_DiffDist(Engine *engine, std::string name) : Gear(engine, "DiffDist", name)
{
  addPlug(_FACTOR_IN = new PlugIn<ValueType>(this, "Factor"));
  addPlug(_VIDEO_IN_A = new PlugIn<VideoTypeRGBA>(this, "ImgA"));
  addPlug(_VIDEO_IN_B = new PlugIn<VideoTypeRGBA>(this, "ImgB"));
  addPlug(_VIDEO_OUT = new PlugOut<VideoTypeRGBA>(this, "ImgOUT"));
}

Gear_DiffDist::~Gear_DiffDist()
{

}

bool Gear_DiffDist::ready()
{
  return(_VIDEO_IN_A->connected() && _VIDEO_IN_B->connected() && _VIDEO_OUT->connected());
}

void Gear_DiffDist::runVideo()
{
  _imageA = _VIDEO_IN_A->type()->image();
  _imageB = _VIDEO_IN_B->type()->image();    
  _outImage = _VIDEO_OUT->type()->image();
  float factor = _FACTOR_IN->type()->value();


  _imageASizeX = _imageA->width();
  _imageASizeY = _imageA->height();
  _imageBSizeX = _imageB->width();
  _imageBSizeY = _imageB->height();
  _iterSizeX = MIN(_imageASizeX,_imageBSizeX);
  _iterSizeY = MIN(_imageASizeY,_imageBSizeY);

  _imageOutSizeX = MAX(_imageASizeX, _imageBSizeX);
  _imageOutSizeY = MAX(_imageASizeY, _imageBSizeY);
  _outImage->resize(_imageOutSizeX, _imageOutSizeY);
  memset(_outImage->data(), 0, _outImage->size()*sizeof(RGBA));

  _dataA = _imageA->data();    
  _dataB = _imageB->data();    
  _outData = _outImage->data();

  int colorIntensity=0;
  int destX=0;
  int destY=0;
  int patchY=0;



  for (int y=0;y<_iterSizeY;y++)
    for (int x=0;x<_iterSizeX;x++)
    {

      colorIntensity = (_dataB[y*_imageBSizeX+x].r + _dataB[y*_imageBSizeX+x].g + _dataB[y*_imageBSizeX+x].b) / 3;            
      colorIntensity *= (int)fabs(factor);


      destX = x + colorIntensity;

      destY = y + colorIntensity;


      if (destX < 0)
        destX=0;

      if (destY < 0)
        destY=0;

      if (destX > _imageOutSizeX)
        destX = _imageOutSizeX;

      if (destY > _imageOutSizeY)
        destY = _imageOutSizeY;

      _outData[destY*_imageOutSizeX+destX].r = _dataA[y*_imageASizeX+x].r;
      _outData[destY*_imageOutSizeX+destX].g = _dataA[y*_imageASizeX+x].g;
      _outData[destY*_imageOutSizeX+destX].b = _dataA[y*_imageASizeX+x].b;

      if (destY != y)
      {
        patchY = y - colorIntensity;
        if (patchY < 0)
          patchY=0;


        _outData[y*_imageOutSizeX+x].r = _dataA[patchY*_imageASizeX+x].r;
        _outData[y*_imageOutSizeX+x].g = _dataA[patchY*_imageASizeX+x].g;
        _outData[y*_imageOutSizeX+x].b = _dataA[patchY*_imageASizeX+x].b;
      }

    }

}
