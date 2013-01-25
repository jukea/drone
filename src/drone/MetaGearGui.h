/* 
 * File:   MetaGearGui.h
 * Author: julien
 *
 * Created on January 24, 2013, 8:17 PM
 */

#ifndef METAGEARGUI_H
#define	METAGEARGUI_H
#include "GearGui.h"

class MetaGearGui: public GearGui {
public:
  MetaGearGui(Gear *pgear, QColor color=BOXNAME_COLOR);
  virtual ~MetaGearGui();
  void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event );
private:

};

#endif	/* METAGEARGUI_H */

