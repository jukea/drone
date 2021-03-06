/* Gear_VideoBlend.h
 * Copyright (C) 2004 Mathieu Guindon, Julien Keable
 * This file is part of Drone.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef GEAR_VIDEOBLEND_INCLUDED
#define GEAR_VIDEOBLEND_INCLUDED


#include "Gear.h"
#include "SignalType.h"
#include "VideoRGBAType.h"


class Gear_VideoBlend : public Gear
{
public:

  Gear_VideoBlend(Schema *schema, std::string uniqueName);
  virtual ~Gear_VideoBlend();

  void runVideo();
  bool ready();

private:

  PlugIn<VideoRGBAType> *_VIDEO_IN_A;
  PlugIn<VideoRGBAType> *_VIDEO_IN_B;
  PlugOut<VideoRGBAType> *_VIDEO_OUT;
  PlugIn<ValueType> *_ALPHA_IN;

  //local var
  const VideoRGBAType *_imageA; 
  const VideoRGBAType *_imageB; 
  VideoRGBAType *_outImage; 
  const RGBA *_dataA;
  const RGBA *_dataB;
  RGBA *_outData;

  int _iterSizeY;
  int _iterSizeX;    
  int _imageASizeX;
  int _imageASizeY;
  int _imageBSizeX;
  int _imageBSizeY;
  int _imageOutSizeX;
  int _imageOutSizeY;

  double *_mmxImageInA;
  double *_mmxImageInB;
  double *_mmxImageOut;
  unsigned char _mmxAlpha, _mmxAlpha2;
};

#endif
