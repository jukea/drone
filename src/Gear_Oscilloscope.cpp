#include "Gear_Oscilloscope.h"
#include "Engine.h"
#include <iostream>
#include "GearMaker.h"

#include <GL/gl.h>              
#include <GL/glu.h>


Register_Gear(MAKERGear_Oscilloscope, Gear_Oscilloscope, "Oscilloscope")

Gear_Oscilloscope::Gear_Oscilloscope(Engine *engine, std::string name) : Gear(engine, "Oscilloscope", name)
{
    _AUDIO_IN = addPlugSignalIn("In");
    _VIDEO_OUT = addPlugVideoOut("Out");
    _VCOMPOSE_IN = addPlugVideoComposeIn("VComIN");
        
    //_VIDEO_OUT->canvas()->allocate(512, 256);
}

Gear_Oscilloscope::~Gear_Oscilloscope()
{
    
}
    
bool Gear_Oscilloscope::ready()
{
    return (_AUDIO_IN->connected());// && _VIDEO_OUT->connected());
}

void Gear_Oscilloscope::runVideo()
{

    _outImage = _VIDEO_OUT->canvas();
    _outImage->allocate(512, 512);
    _outData = _outImage->_data;

    Signal_T *buffer = _AUDIO_IN->buffer();
    //Video_T *image = _VIDEO_OUT->canvas();
    int signal_blocksize = Engine::signalInfo().blockSize();

    glDisable(GL_TEXTURE_2D);
    
    glBegin(GL_LINES);
    glColor3f(0.0f, 1.0f, 0.0f);
    for(int i=0;i<signal_blocksize;i++) 
    {
        //image->_Data[ (((int)(buffer[i]*image->_SizeY) + (image->_SizeY/2)) * Image::MAX_SIZEX) + i].R = 255;
        //cout << buffer[i]*64 << endl;
        //image->pix(i,128 + (buffer[i]*32))->G = 255;
        glVertex2f((float)i, 128.0f + (buffer[i]*32.0f));
        //std::cout << (128.0f + (buffer[i]*32.0f)) << " : " << buffer[i] << std::endl;

    }
    glEnd();
    glEnable(GL_TEXTURE_2D);

    glReadPixels(0, 0, 512, 512, GL_RGBA, GL_UNSIGNED_BYTE, (void*)_outData);

/*     for (int i=0;i<512*512;i++)  */
/*     {                            */
/*         image->_Data[i].R = 255; */
/*     }                            */

}



