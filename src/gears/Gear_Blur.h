#ifndef GEAR_BLUR_INCLUDED
#define GEAR_BLUR_INCLUDED


#include "Gear.h"
#include "SignalType.h"
#include "VideoType.h"
#include "SummedAreaTable.h"


class Gear_Blur : public Gear
{
public:

  Gear_Blur(Engine *engine, std::string name);
  virtual ~Gear_Blur();

  void runVideo();

  PlugIn<VideoTypeRGBA>* VIDEO_IN(){return _VIDEO_IN;};
  PlugOut<VideoTypeRGBA>* VIDEO_OUT(){return _VIDEO_OUT;};
  PlugIn<ValueType>* AMOUNT_IN(){return _AMOUNT_IN;};

  bool ready();

private:

  PlugIn<VideoTypeRGBA> *_VIDEO_IN;
  PlugIn<VideoTypeRGBA> *_AMOUNT_MAP_IN;
  PlugOut<VideoTypeRGBA> *_VIDEO_OUT;
  PlugIn<ValueType> *_AMOUNT_IN;

  //local var
  const VideoTypeRGBA *_image;     
  VideoTypeRGBA *_outImage; 
  unsigned char *_data;
  unsigned char *_amountMapData;

  SummedAreaTable *_table;
  RGBAint _sum;

  unsigned char *_outData;

  int _sizeY;
  int _sizeX;

  int _x1,_y1,_x2,_y2;

  int _blurSize;
  int _homeWeight;
  int _halfBlurSize;

  RGBA _acc;

  VideoTypeRGBA _defaultAmountMapData;

  void init();    
};

#endif
