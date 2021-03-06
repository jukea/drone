/* SchemaGui.cpp
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


#include "SchemaGui.h"

#include "Schema.h"
#include "Gear.h"
#include "MetaGear.h"
#include "GearGui.h"
#include "PlugBox.h"
//#include "commands/CommandMoveItems.h"
#include "ConnectionItem.h"
#include <QGraphicsSceneMouseEvent>
#include <QVarLengthArray>
#include <q3filedialog>
#include <qtextstream>

const int SchemaGui::DEFAULT_CANVAS_SIZE_X = 2048;
const int SchemaGui::DEFAULT_CANVAS_SIZE_Y = 2048;


SchemaGui::SchemaGui(Schema *schema, Engine *engine) :
  QGraphicsScene(0,0,DEFAULT_CANVAS_SIZE_X, DEFAULT_CANVAS_SIZE_Y),
  _activeConnection(NULL),
  _connecting(false),
  _moving(No),
  _engine(engine),
  _maxZValue(0),
  _movingItems(),
  _movingStartPos(0,0),
  _selectionChangeBypass(false)
{
  QObject::connect(this,SIGNAL(selectionChanged()),this,SLOT(selectionHasChanged()));
  //QObject::connect(this, SIGNAL(itemMoved(DiagramItem*,QPntF)),
  //           this, SLOT(itemMoved(DiagramItem*,QPointF)));
  setSchema(schema);
}

SchemaGui::~SchemaGui()
{
}


void SchemaGui::setSchemaEditor(SchemaEditor* se)
{
  _schemaEditor = se;
}

SchemaEditor* SchemaGui::getSchemaEditor() const
{
  return _schemaEditor;
}

void SchemaGui::setSchema(Schema *schema)
{
  clear(); 
  _schema = schema;
  rebuildSchema();
}

void SchemaGui::rebuildSchema()
{
  //add gearguis
  std::list<Gear*> gears = _schema->getGears();
  
  
  GearGui *gearGui=NULL;
  for (std::list<Gear*>::iterator it=gears.begin();it!=gears.end();++it)
  {
		gearGui=(*it)->getGearGui();
		addItem(gearGui);
    gearGui->show();
  }
	
  //add connectionItems
  std::list<Schema::Connection*> connections;
  _schema->getAllConnections(connections);
  
  PlugBox *sourcePlugBox;
  PlugBox *destPlugBox;
  Gear *gearA;
  Gear *gearB;

  for (std::list<Schema::Connection*>::iterator it = connections.begin(); it != connections.end(); ++it)
  {   
    gearA = _schema->getGearByName((*it)->gearA());
    gearB = _schema->getGearByName((*it)->gearB());

    if (gearA && gearB)
		{
			sourcePlugBox = gearA->getGearGui()->getOutputPlugBox((*it)->output());
			destPlugBox = gearB->getGearGui()->getInputPlugBox((*it)->input());
			sourcePlugBox->connect(destPlugBox);
		}
		
    delete (*it);//free temporary Connection
  }

  update();

}



  void SchemaGui::drawBackground ( QPainter * painter, const QRectF & rect )
  {
    painter->fillRect(rect,QColor(50,50,50));
    QVarLengthArray<QLineF, 36> lines;
        for (int i = 0; i <= 20; i ++) {
            lines.append(QLineF(i*100+0.5,0,i*100+0.5,10000));
            lines.append(QLineF(0,i*100+0.5,10000,i*100+0.5));
        }
    painter->setPen(QPen(QColor(70,70,70),0.5));
    painter->drawLines(lines.data(), lines.size());
  }

  
Gear* SchemaGui::addGear(std::string type, QPointF pos)
{            
  Gear *gear = _schema->addGear(type);    

  if (!gear)
    return NULL;
    
  GearGui *gearGui = gear->getGearGui();    

  addItem(gearGui);    
  gearGui->setPos(pos);    
  gearGui->setZValue(0);
  gearGui->show();
  update();

  return gear;
}

MetaGear *SchemaGui::addMetaGear(std::string filename, QPointF pos)
{    
  MetaGear *metaGear = _schema->addMetaGear(filename);    

  if (metaGear==NULL)
    return NULL;
  
  GearGui *gearGui = metaGear->getGearGui();    
  addItem(gearGui);
  gearGui->setPos(pos);    
  gearGui->setZValue(0);
  gearGui->show();
  update();

  return metaGear;
}


void SchemaGui::renameGear(GearGui *gearGui, std::string newName)
{
  if (!gearGui)
    return;

  _schema->renameGear(gearGui->gear(), newName);
}

MetaGear* SchemaGui::newMetaGear(QPointF pos)
{    
  MetaGear *metaGear = _schema->newMetaGear();    
  GearGui *gearGui = metaGear->getGearGui();    

  addItem(gearGui);
  gearGui->setZValue(0);
  gearGui->setPos(pos);
  gearGui->show();
  update();
  
  return metaGear;
}

void SchemaGui::removeGear(GearGui* gearGui)
{  
  Gear* gear = gearGui->gear();

  delete gearGui;

  _schema->removeDeepGear(gear);

  update();
}

void SchemaGui::clear()
{
  QList<QGraphicsItem *> l=items();
  std::vector<GearGui*> gearGuis;

  //first fill a vector with only gearGuis
  //we have to make it this way because we cannot
  //iterate on QGraphicsItem while removing them at the sametime
  foreach(QGraphicsItem * it,l) { 
    if ( qgraphicsitem_cast<GearGui*>(it))    
      gearGuis.push_back((GearGui*)(it));
  }
  
  //now remove gearGuis
  for (std::vector<GearGui*>::iterator it=gearGuis.begin();it!=gearGuis.end();++it)  
    removeGear((*it));
}

bool SchemaGui::load(QDomElement& parent)
{
  clear();
  if(_schema->load(parent))
  {
    setSchema(_schema);
    return true;
  }
  return false;
}

bool SchemaGui::save(QDomDocument& doc, QDomElement &parent, bool onlySelected)
{
  return _schema->save(doc, parent,onlySelected);
}

void SchemaGui::selectionHasChanged()
{
  QList<QGraphicsItem *> list(selectedItems());
  QGraphicsItem* el;
  GearGui* selectedGear=0;
  qreal zOffset;
  if(_selectionChangeBypass)
    return;
  if(list.count()==0)
    _maxZValue++;


  //std::cerr<<"Selected elements: "<<list.count()<<std::endl;
  foreach(el,list)
  {
    // create pseudo "layers" with slices of Z values so that comments
    // are always below gears (TODO/UPCOMING) and connections always on top 
    if(qgraphicsitem_cast<ConnectionItem*>(el))
      zOffset=1000000;
    else zOffset=0;
    selectedGear=qgraphicsitem_cast<GearGui*>(el);
    el->setZValue(_maxZValue+zOffset);
  }
  if(selectedGear)
    emit gearSelected(selectedGear);
}

void SchemaGui::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  QList<QGraphicsItem *> list(items(event->scenePos()));
  QList<QGraphicsItem *> selectedList(selectedItems());
  QPointF pos;
  GearGui* gearGui;
  PlugBox* selectedPlugBox;
  QGraphicsItem *el, *el2;

  foreach(el, list)
  {
    if ((gearGui = qgraphicsitem_cast<GearGui*>(el)))
    {
      pos = gearGui->mapFromScene(event->scenePos());
      selectedPlugBox = gearGui->plugHit(pos);
      if (selectedPlugBox)
      {
        _connecting = true;
        _activeConnection = new ConnectionItem();

        _activeConnection->setSourcePlugBox(selectedPlugBox);
        _activeConnection->setConnectionLineEndPoint(event->scenePos());
        addItem(_activeConnection);
        event->accept();
        return;
      }
    }

    //  see : http://harmattan-dev.nokia.com/docs/library/html/qt4/tools-undoframework-diagramscene-cpp.html
    // looks like we're starting an item(s) move, Yay! Let's capture some data to build the undo Command
    if (_moving==No && /*!event->modifiers() &&*/ event->button() == Qt::LeftButton && (el->flags() & QGraphicsItem::ItemIsMovable))
    {
      _moving = Pre;
      //std::cerr<<"About to move"<<std::endl;

    }
  }
  QGraphicsScene::mousePressEvent(event);

}

void SchemaGui::mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  QList<QGraphicsItem *> selectedList(selectedItems());
  QGraphicsItem* extra;
  GearGui* gearGui;
  QGraphicsItem *el;
  if (_connecting)
  {
    _activeConnection->setConnectionLineEndPoint(event->scenePos());
  }
  if (_moving == Pre)
  {
    _moving = Yes;
    _movingItems.clear();

    // mimick QGraphicsScene quirky behavior. Starting a move while Ctrl-clicking on
    // an item will select it and move it AFTER we're out of this handler
    if (event->modifiers() & Qt::ControlModifier)
    {
      extra = itemAt(event->scenePos());
      gearGui = qgraphicsitem_cast<GearGui*>(extra);
      if (gearGui && !gearGui->isSelected())
        selectedList << gearGui;
    }
    
    foreach(el, selectedList)
    {
      gearGui = qgraphicsitem_cast<GearGui*>(el);
      if (gearGui)std::cerr << gearGui->gear()->name().c_str() << std::endl;
      if (gearGui && (el->flags() & QGraphicsItem::ItemIsMovable))
        _movingItems << gearGui->gear()->name().c_str();
    }
    //std::cerr << "mod"<<event->modifiers()<<" Looks like we're moving" << _movingItems.count()<<std::endl;
    _movingStartPos = event->scenePos();

  }

  QGraphicsScene::mouseMoveEvent(event);
}

void SchemaGui::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
  QList<QGraphicsItem *> list(items(event->scenePos()));
  ConnectionItem* ci;
  QGraphicsItem* el;

  foreach(el, list)
  {
    std::cerr << "found item" << std::endl;

    if (ci = qgraphicsitem_cast<ConnectionItem*>(el))
    {
      std::cerr << "disconnect" << std::endl;

      disconnect(ci->sourcePlugBox(), ci->destPlugBox());
      return;
    }
  }

}

void SchemaGui::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
  QPointF dist(event->scenePos() - _movingStartPos);
  if (_connecting)
  {
    QList<QGraphicsItem *> list(items(event->scenePos()));
    QPointF pos;
    PlugBox* destPlug;
    QGraphicsItem* el;

    foreach(el, list)
    {
      if (qgraphicsitem_cast<GearGui*>(el))
      {
        pos = (static_cast<GearGui*>(el))->mapFromScene(event->scenePos());
        destPlug = (static_cast<GearGui*>(el))->plugHit(pos);
        if (destPlug)
        {
          _connecting = false;
          connect(_activeConnection->sourcePlugBox(), destPlug);
          delete _activeConnection;
          return;
        }
      }
    }
    delete _activeConnection;
    _connecting=false;
  }
  else if(_moving==Yes)
  {
    //std::cerr<<"Moved "<<_movingItems.count()<<" items by "<<dist.x()<<","<<dist.y()<<std::endl;
    //new CommandMoveItems(_movingItems,dist);
    emit itemsMoved(_movingItems, dist);

    _moving=No;
  }
  // happens when we didn't move
  else if(_moving==Pre)
  {
    _moving=No;
  }
  QGraphicsScene::mouseReleaseEvent( event);
}

bool SchemaGui::connect(PlugBox *plugA, PlugBox *plugB)
{
  if (!plugA || !plugB)
    return false;

  if (!plugA->canConnectWith(plugB))
    return false;
  
  //in the case of an input that is already
  //connected, we first need to disconnect
  if (plugA->inOut()==IN && plugA->connected())
  {
    disconnectAll(plugA);
  }
  else if (plugB->inOut()==IN && plugB->connected())
  {    
    disconnectAll(plugB);
  }
   
  //connect plugBox
  plugA->connect(plugB);

  //update the canvas
  update();

  //tell the schema to make the connection
  return _schema->connect(plugA->plug(), plugB->plug());
}

void SchemaGui::disconnect(PlugBox *plugA, PlugBox *plugB)
{
  if (!plugA || !plugB)
    return;
  
  //disconnect plugbox
  plugA->disconnect(plugB);
  
  //tell the schema to disconnect plugs
  _schema->disconnect(plugA->plug(), plugB->plug());  

  //update the canvas
  update();
}

void SchemaGui::disconnectAll(PlugBox *plugBox)
{
  if (!plugBox)
    return;
  
  plugBox->disconnectAll();

  update();

  //tell the schema to disconnect all from this plug
  _schema->disconnectAll(plugBox->plug());
}

std::vector<GearGui*> SchemaGui::getAllGears()
{
  std::vector<GearGui*> vec;
  QList<QGraphicsItem*> l=items();
  foreach(QGraphicsItem* it,l)
    if ( qgraphicsitem_cast<GearGui*>(it))    
      vec.push_back((GearGui*)(it));
  return vec;
}

std::vector<GearGui*> SchemaGui::getSelectedGears()
{
  std::vector<GearGui*> vec;
  QList<QGraphicsItem*> l=selectedItems();
  foreach(QGraphicsItem * it,l)
    if ( qgraphicsitem_cast<GearGui*>(it))    
      vec.push_back((GearGui*)(it));
  return vec;
}

// Warning : order is not preserved ! 
QList<QGraphicsItem*> SchemaGui::getItemsByName(QList<QString>& list)
{
  QList<QGraphicsItem*> all(items()),selection;
  GearGui* ggui;
  
  foreach(QGraphicsItem* el,all) 
  {
    // forced to do this since all elements don't have a common interface with name()
    if((ggui=qgraphicsitem_cast<GearGui*>(el))
            && list.contains(ggui->gear()->name().c_str()))
      selection<<el;
  }
  return selection;
}

void SchemaGui::moveItemsBy(QList<QGraphicsItem*> list,QPointF delta)
{
  QGraphicsItem* el;
  foreach(el,list)
    el->moveBy(delta.x(),delta.y());
  update();
}
