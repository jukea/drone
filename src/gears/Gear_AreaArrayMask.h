/* Gear_AreaArrayMask.h
 * Copyright (C) 2004 Jean-Sebastien Senecal
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

#ifndef GEAR_AREAARRAYMASK_INCLUDED
#define GEAR_AREAARRAYMASK_INCLUDED


#include "Gear.h"
#include "VideoRGBAType.h"
#include "AreaArrayType.h"
//#include "ValueType.h"
#include "error.h"

class Gear_AreaArrayMask : public Gear
{
public:
  Gear_AreaArrayMask(Engine *engine, std::string name);
  virtual ~Gear_AreaArrayMask();

  void runVideo();

  bool ready();

private:
  PlugOut<VideoRGBAType> *_VIDEO_OUT;
  PlugIn<ValueType> *_WIDTH_IN;
  PlugIn<ValueType> *_HEIGHT_IN;
  //  PlugIn<ValueType> *_OPACITY_IN;
  PlugIn<AreaArrayType> *_AREA_ARRAY_IN;

  //int _sizeX, _sizeY;

  VideoRGBAType *_imageOut;
  const AreaArrayType *_areaArray;
  int _sizeX, _sizeY;
};

#endif
