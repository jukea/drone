/* 
 * File:   MetaGearGui.cpp
 * Author: julien
 * 
 * Created on January 24, 2013, 8:17 PM
 */

#include "MetaGearGui.h"
#include "Gear.h"
#include "MetaGear.h"
#include "MainWindow.h"
#include "SchemaEditor.h"
MetaGearGui::MetaGearGui(Gear *pgear, QColor color):
GearGui(pgear,color)
{
}

MetaGearGui::~MetaGearGui()
{
}

void MetaGearGui::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event )
{
  Q_UNUSED(event);
  startDiveInAnimation();
  QList<QGraphicsView*> v(scene()->views());
  if(v.count())
    static_cast<SchemaEditor*>(v.first())->startDiveInAnimation();
//  MainWindow::getInstance()->openMetaGear(_gear->getUUID());
}

