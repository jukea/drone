#include "Gear_VideoAdd.h"
#include "Engine.h"

#include <iostream>

#include "GearMaker.h"


Register_Gear(MAKERGear_VideoAdd, Gear_VideoAdd, "VideoAdd")

Gear_VideoAdd::Gear_VideoAdd(Engine *engine, std::string name) : Gear(engine, "VideoAdd", name)
{
    _VIDEO_IN_A = addPlugVideoIn("ImgA");
    _VIDEO_IN_B = addPlugVideoIn("ImgB");
    _VIDEO_OUT = addPlugVideoOut("ImgO");
}

Gear_VideoAdd::~Gear_VideoAdd()
{

}

bool Gear_VideoAdd::ready()
{
    return(_VIDEO_IN_A->connected() && _VIDEO_IN_B->connected() && _VIDEO_OUT->connected());
}



void Gear_VideoAdd::runVideo()
{
    _imageA = _VIDEO_IN_A->canvas();
    _imageB = _VIDEO_IN_B->canvas();    
    _outImage = _VIDEO_OUT->canvas();
    
    _imageASizeX = _imageA->sizeX();
    _imageASizeY = _imageA->sizeY();
    _imageBSizeX = _imageB->sizeX();
    _imageBSizeY = _imageB->sizeY();
    _iterSizeX = MIN(_imageASizeX,_imageBSizeX);
    _iterSizeY = MIN(_imageASizeY,_imageBSizeY);
    
    _imageOutSizeX = MAX(_imageASizeX, _imageBSizeX);
    _imageOutSizeY = MAX(_imageASizeY, _imageBSizeY);
    _outImage->allocate(_imageOutSizeX, _imageOutSizeY);
    
    _dataA = _imageA->_data;    
    _dataB = _imageB->_data;    
    _outData = _outImage->_data;
             
    register int mmxCols=_iterSizeX/2;
    register int index;    
            
    _largerImage = _imageASizeX > _imageBSizeX ? _dataA : _dataB;
    _tallerImage = _imageASizeY > _imageBSizeY ? _dataA : _dataB;

    for (int y=0;y<_iterSizeY;y++)
    {
        _mmxImageInA = (double*)&_dataA[y*_imageASizeX];
        _mmxImageInB = (double*)&_dataB[y*_imageBSizeX];
        _mmxImageOut = (double*)&_outData[y*_imageOutSizeX];

        for(index=0;index<mmxCols;index++)
        {
            __asm__ volatile (
                "\n\t movq %1,%%mm0        \t# (u) load imageA"
                "\n\t movq %2,%%mm1        \t# (u) load imageB"
                "\n\t psubusb %%mm0,%%mm1   \t# (u) L2 = L0 - L2"
                "\n\t movq %%mm1,%0        \t# (u) store result "
                : "=m" (_mmxImageOut[index])  // this is %0, output
                : "m"  (_mmxImageInA[index]), // this is %1, image A
                "m"  (_mmxImageInB[index]) // this is %1, image A
                );
        }

    }
    
    __asm__("emms" : : );

}

