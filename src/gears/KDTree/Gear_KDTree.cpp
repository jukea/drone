/* Gear_KDTree.cpp
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

#include "Gear_KDTree.h"
#include "Engine.h"

#include <iostream>

#include "GearMaker.h"

extern "C" {
Gear* makeGear(Schema *schema, std::string uniqueName)
{
  return new Gear_KDTree(schema, uniqueName);
}

GearInfo getGearInfo()
{
  GearInfo gearInfo;
  gearInfo.name = "KDTree";
  gearInfo.classification = GearClassifications::video().distortion().instance();
  return gearInfo;
}
}

Gear_KDTree::Gear_KDTree(Schema *schema, std::string uniqueName)
: Gear(schema, "KDTree", uniqueName)
{
  addPlug(_VIDEO_IN = new PlugIn<VideoRGBAType>(this, "ImgIN"));
  addPlug(_VIDEO_OUT = new PlugOut<VideoRGBAType>(this, "ImgOUT"));
  addPlug(_AMOUNT_IN = new PlugIn<ValueType>(this, "Depth", new ValueType(6, 0, 16)));
  addPlug(_AREA_OUT = new PlugOut<AreaArrayType>(this, "Segm"));
  _rasterer = new Rasterer();
  _table = new SummedAreaTable<>();
}

Gear_KDTree::~Gear_KDTree()
{
  delete _rasterer;
  delete _table;
}

bool Gear_KDTree::ready()
{
  return(_VIDEO_IN->connected() && (_VIDEO_OUT->connected() || _AREA_OUT->connected()));
}

void Gear_KDTree::init()
{
  _rasterer->setImage(_VIDEO_OUT->type());
  _rasterer->setColor(0, 0, 0); // black lines only
}

void Gear_KDTree::runVideo()
{
  // initialize
  _maxDepth = MAX((int)_AMOUNT_IN->type()->value(), 0);

  _image = _VIDEO_IN->type();
  if (_image->isNull())
    return;

  _outImage = _VIDEO_OUT->type();
  _outImage->resize(_image->width(), _image->height());

  _sizeX = _image->width();
  _sizeY = _image->height();

  _rasterer->setImage(_outImage);

  // build accumulation buffer
  _table->reset((unsigned char*)_image->data(), _image->width(), _image->height());

  // clear areas
  _AREA_OUT->type()->resize(0);
    
  // create splits
  split(0, _sizeX-1, 0, _sizeY-1, 0);
}

void Gear_KDTree::split(int x0, int x1, int y0, int y1, int depth)
{
  int x0minus1 = x0-1;
  int y0minus1 = y0-1;
  
  // Get the total values in the area.
  int rgba[SIZE_RGBA];
  int area;
  _table->getSum(rgba, area, x0minus1, y0minus1, x1, y1);

  if (depth == _maxDepth)
  {
    // Draw a rectangle around the area and paint it with the average color.
    if (_VIDEO_OUT->connected())
    {
      _rasterer->setColor(rgba[0] / area, rgba[1] / area, rgba[2] / area);
      _rasterer->rect(x0, y0, x1, y1, true);
      _rasterer->setColor(0,0,0);
      _rasterer->rect(x0, y0, x1, y1, false);
    }
    
    // Add area to list.
    if (_AREA_OUT->connected())
    {
      Area a;
      a.x0 = x0; a.y0 = y0;
      a.x1 = x1; a.y1 = y1;
      _AREA_OUT->type()->push_back(a);
    }

    return;
  }

  if (x1 == x0 || y1 == y0) // *** threshold to set
    return;

  // Useful values.
  //int area = _table->getArea(x0, y0, x1, y1);
  int cut = sum(rgba, SIZE_RGB) >> 1;

  // Increment depth by one level.
  depth++;

  // Now split.
  if (depth % 2)
  {
    // vertical split
    int upper = y1;
    int lower = y0;
    int mid = 0;
    // binary search
    while (lower != upper)
    {
      mid = (lower+upper) / 2; // take the mean
       /// XXX pas besoin de "area" ici, il faut du code separe getSumAndArea dans SummedAreaTable...
      _table->getSum(rgba, area, x0minus1, y0minus1, x1, mid);
      if (sum(rgba, SIZE_RGB) < cut)
        lower = mid+1; // look up //*** attention risque d'erreur : v�rifier
      else
        upper = mid;  // look down
    }
    // split the area in two
    split(x0, x1, y0, mid, depth);
    split(x0, x1, mid, y1, depth);
  }
  else
  {
    // horizontal split
    int upper = x1;
    int lower = x0;
    int mid = 0;
    // binary search
    while (lower != upper)
    {
      mid = (lower+upper) / 2; // take the mean
       /// XXX pas besoin de "area" ici, il faut du code separe getSumAndArea dans SummedAreaTable...
      _table->getSum(rgba, area, x0minus1, y0minus1, mid, y1);
      if (sum(rgba, SIZE_RGB) < cut)
        lower = mid+1; // look right //*** attention risque d'erreur : v�rifier
      else
        upper = mid;  // look left
    }
    // split the area in two
    split(x0, mid, y0, y1, depth);
    split(mid, x1, y0, y1, depth);
  }
}