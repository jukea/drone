#include "Gear_Blur.h"
#include "Engine.h"

#include <iostream>

#include "GearMaker.h"


Register_Gear(MAKERGear_Blur, Gear_Blur, "Blur")

  Gear_Blur::Gear_Blur(Engine *engine, std::string name) : Gear(engine, "Blur", name), _amountMapData(0)
{
  _VIDEO_IN = addPlugVideoIn("ImgIN");
  _AMOUNT_MAP_IN = addPlugVideoIn("Amount Map");
    
  _VIDEO_OUT = addPlugVideoOut("ImgOUT");
  _AMOUNT_IN = addPlugSignalIn("Amount", 0.5f);
  _table = new SummedAreaTable();
}

Gear_Blur::~Gear_Blur()
{
  delete _table;
}

void Gear_Blur::init()
{
  _table->setCanvas(_VIDEO_IN->canvas());
}

bool Gear_Blur::ready()
{
  return (_VIDEO_IN->connected() && _VIDEO_OUT->connected());
}

void Gear_Blur::runVideo()
{
  _image = _VIDEO_IN->canvas();
    
  _outImage = _VIDEO_OUT->canvas();
  _outImage->allocate(_image->sizeX(), _image->sizeY());
  _sizeY = _image->sizeY();
  _sizeX = _image->sizeX();

/*   // we set a default value of 255 size of the image if amountmap not connected */
/*   if (!_AMOUNT_MAP_IN->connected())                                             */
/*   {                                                                             */
/*     _defaultAmountMapData.allocate(_image->sizeX(), _image->sizeY());           */
/*     _defaultAmountMapData.fill(255);                                            */
/*     _amountMapData = (unsigned char*)_defaultAmountMapData._data;               */
/*   }                                                                             */
/*   else                                                                          */
/*     _amountMapData = (unsigned char*)(_AMOUNT_MAP_IN->canvas()->_data);         */

  _data = (unsigned char*)_image->_data;    
  _outData = (unsigned char*)_outImage->_data;
    
  _table->setCanvas(_image);
  _table->buildTable();
  
  ////////////////////////////
    
  _iterAcc = (int*)_table->_acc;
            
  _blurSize=(int) (_AMOUNT_IN->buffer()[0]);

  if (_blurSize<2)
    _blurSize=2;

  for(int y=0;y<_sizeY;y++)
  {       
    for(int x=0;x<_sizeX;x++)
    {
      _x1 = x - _blurSize - 1;
      _x2 = x + _blurSize;
      _y1 = y - _blurSize - 1;
      _y2 = y + _blurSize;

      if(_x1 < 0)_x1 = 0;      
      if(_y1 < 0)_y1 = 0;

      if(_x2 >= _sizeX)_x2 = _sizeX-1;        
      if(_y2 >= _sizeY)_y2 = _sizeY-1;

      _dimension = (_x2-_x1)*(_y2-_y1);      
      for(int z=0;z<4;z++)
      {

        *(_outData++) = (_iterAcc[(_y2 * (_sizeX<<2)) + (_x2<<2) + z] - 
                         _iterAcc[(_y1) * (_sizeX<<2) + (_x2<<2) + z] -
                         _iterAcc[(_y2 * (_sizeX<<2)) + ((_x1)<<2) + z] +
                         _iterAcc[(_y1) * (_sizeX<<2) + ((_x1)<<2) + z]) / _dimension;
      }

    }
                                         
  }                                    
    
       
}
