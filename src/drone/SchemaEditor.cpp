/* SchemaEditor.cpp
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

#include "SchemaEditor.h"
#include "GearGui.h"
#include "ConnectionItem.h"
#include "PlugBox.h"
#include "Engine.h"
#include "Gear.h"
#include "MetaGear.h"

#include "MetaGearEditor.h"
#include "PanelScrollView.h"
#include "ControlPanel.h"
#include "GearListMenu.h"
#include "MetaGearListMenu.h"
#include "GearPropertiesDialog.h"
#include "GearMaker.h"
#include "MetaGearMaker.h"

#include <q3mainwindow.h>
#include <q3filedialog.h>
#include <iostream>
#include <q3dragobject>

#include <qcursor.h>
#include <qlayout.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <QGLWidget>
#include <iostream>
#include <qpainter.h>
#include <qmatrix.h>
#include <qmath.h>

#include <qdom.h>

const std::string SchemaEditor::NAME = "SchemaEditor";
const double SchemaEditor::ZOOM_FACTOR = 1.4;

SchemaEditor::SchemaEditor(QWidget *parent, SchemaGui *schemaGui, Engine * engine, PanelScrollView *panelScrollView) :
  QGraphicsView(schemaGui, parent),
  _contextMenuPos(0,0),
  _contextGear(NULL),
  _engine(engine),
  _schemaGui(schemaGui),
  _scale(1),
  _panelScrollView(panelScrollView)
  

{
  _schemaGui->setSchemaEditor(this);
  setDragMode(QGraphicsView::RubberBandDrag);
  setRenderHint(QPainter::Antialiasing, true);

  setFrameStyle(Sunken | StyledPanel);
  setOptimizationFlags(QGraphicsView::DontSavePainterState | QGraphicsView::IndirectPainting);
  setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
  
  // render with OpenGL
  if(1)
    setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));

  resetTransform();
  setAcceptDrops(TRUE);
  
  _contextMenu = new QMenu();
  _gearListMenu = new GearListMenu(_contextMenu);    
  _gearListMenu->create();
  
  QAction * sub = _contextMenu->addMenu(_gearListMenu);
  sub->setText("Gears");
  
  QObject::connect(_gearListMenu, SIGNAL(gearSelected(QString)), this, SLOT(slotMenuGearSelected(QString)));

  _metaGearListMenu = new MetaGearListMenu(_contextMenu);    
  _metaGearListMenu->create();
  
  QAction* mgSubAction = _contextMenu->addMenu(_metaGearListMenu);
  mgSubAction->setText("MetaGears");
  
  QObject::connect(_metaGearListMenu, SIGNAL(metaGearSelected(QFileInfo*)), this, SLOT(slotMenuMetaGearSelected(QFileInfo*)));

  _contextMenu->addAction("New MetaGear", this, SLOT(slotNewMetaGear()));

  
  _gearContextMenu = new QMenu();
  _gearContextMenu->addAction("delete",  this, SLOT(slotGearDelete()));
  _gearContextMenu->addAction("Properties", this, SLOT(slotGearProperties()));
	_gearContextMenu->addAction("ZoomIn", this, SLOT(zoomIn()));  
	_gearContextMenu->addAction("ZoomOut", this, SLOT(zoomOut()));  

  _gearContextMenu->addAction("About");    
  
  _metaGearContextMenu = new QMenu();
  _metaGearContextMenu->addAction("delete", this, SLOT(slotGearDelete()),Qt::Key_Delete);
  _metaGearContextMenu->addAction("Select All", this, SLOT(slotGearSelectAll()),Qt::CTRL + Qt::Key_A);
  _metaGearContextMenu->addAction("Copy", this, SLOT(slotGearCopy()),Qt::CTRL + Qt::Key_C);
  _metaGearContextMenu->addAction("Paste", this, SLOT(slotGearPaste()),Qt::CTRL + Qt::Key_V);
  _metaGearContextMenu->addAction("Properties", this, SLOT(slotGearProperties()));  
  _metaGearContextMenu->addAction("About");    
  _metaGearContextMenu->addSeparator();
  _metaGearContextMenu->addAction("Save MetaGear",  this, SLOT(slotSaveMetaGear()));


  // plug context menu initialization
  _plugContextMenu = new QMenu();
  _exposePlugAction = _plugContextMenu->addAction("Expose", this, SLOT(slotPlugToggleExpose()));

}


void SchemaEditor::slotMenuGearSelected(QString name)
{        
  addGear(name.ascii(), _contextMenuPos);    
}

void SchemaEditor::slotMenuMetaGearSelected(QFileInfo* metaGearFileInfo)
{  
  addMetaGear(metaGearFileInfo->filePath().ascii(), _contextMenuPos);    
}

void SchemaEditor::slotGearProperties()
{
  if (_contextGear == NULL)
    return;

  GearPropertiesDialog *gearPropertiesDialog;

  gearPropertiesDialog = new GearPropertiesDialog(NULL, _contextGear->gear(), _engine);
  gearPropertiesDialog->exec();
  delete gearPropertiesDialog;

  _contextGear=NULL;

}

void SchemaEditor::slotGearDelete()
{
  deleteSelectedGears();
  _contextGear = NULL;
}

/**
 * React on Expose selection in the plug context menu
 */
void SchemaEditor::slotPlugToggleExpose()
{
  _contextPlug->plug()->exposed(!_contextPlug->plug()->exposed());
  _contextPlug->getGearGui()->update();
} 


void SchemaEditor::slotNewMetaGear()
{
  _schemaGui->newMetaGear(_contextMenuPos);
}

void SchemaEditor::slotSaveMetaGear()
{
  if (!_contextGear->gear()->kind() == Gear::METAGEAR)
  {
    std::cout << "not a metagear, cannot save!!!" << std::endl; 
    return;
  }
   
  MetaGear* metaGear = (MetaGear*)_contextGear->gear();
  
  //mangle suggested filename
  std::string suggestedFilename=metaGear->fullPath();  
  if (suggestedFilename.empty())
    suggestedFilename=MetaGearMaker::METAGEAR_PATH + "/" + metaGear->name();
  

  std::string filename = Q3FileDialog::getSaveFileName(suggestedFilename.c_str(), ("*" + MetaGear::EXTENSION + ";;" + "*.*").c_str(), 
                                                      0, "Save as", "Save as").toStdString();
  
  if (!filename.empty())
  {
    //set the extension if not already present
    if (filename.find(MetaGear::EXTENSION.c_str(), filename.size()-MetaGear::EXTENSION.size())==std::string::npos)
      filename.append(MetaGear::EXTENSION);  
    
    metaGear->save(filename);    
    
    //rename the metagear to the new saved name
    QFileInfo fileInfo(filename.c_str());
    _schemaGui->renameGear(_contextGear, fileInfo.baseName().toStdString());
    
  }

}

void SchemaEditor::deleteSelectedGears()
{
  std::vector<GearGui*> allGears = _schemaGui->getSelectedGears();
  for(unsigned int i=0;i<allGears.size();++i)
    _schemaGui->removeGear(allGears[i]);
}

void SchemaEditor::unselectAllGears()
{
  std::vector<GearGui*> allGears = _schemaGui->getSelectedGears();
  for(unsigned int i=0;i<allGears.size();++i)
    allGears[i]->setSelected(false);
  update();
}

void SchemaEditor::selectAllGears()
{
  std::vector<GearGui*> allGears = _schemaGui->getAllGears();
  for(unsigned int i=0;i<allGears.size();++i)
    allGears[i]->setSelected(true);
  update();
}



void SchemaEditor::slotGearCopy()
{
  QDomDocument doc("Clipboard");
  
  QDomElement clipboardElem = doc.createElement("Clipboard");
  doc.appendChild(clipboardElem);

//   QDomElement cornerElem = doc.createElement("SelectedBoxUpperLeftCorner");
//   doc.appendChild(cornerElem);

//   QRect box = getBoundingBoxOfAllSelectedGears();

//   QDomAttr xcoord = doc.createAttribute("X");
//   xcoord.setValue(QString("%1").arg(box.left()));
//   clipboardElem.setAttributeNode(xcoord);

//   QDomAttr ycoord = doc.createAttribute("Y");
//   ycoord.setValue(QString("%1").arg(box.top()));
//   clipboardElem.setAttributeNode(ycoord);

  if(!_schemaGui->save(doc, clipboardElem,true))
    return;

  //save to file  
  QString str;
  QTextStream stream(&str,QIODevice::WriteOnly);
  doc.save(stream,4);
  _engine->setClipboardText(str.latin1());
  std::cerr<<_engine->getClipboardText()<<std::endl;
  
}

void SchemaEditor::slotGearPaste()
{
  unselectAllGears();
  QDomDocument doc("Clipboard");

  QString str(_engine->getClipboardText().c_str());

  QString errMsg;
  int errLine;
  int errColumn;
  if (!doc.setContent(str, true, &errMsg, &errLine, &errColumn))
  {
    std::cout << "parsing error in clipboard"<< std::endl;
    std::cout << errMsg.ascii() << std::endl;
    std::cout << "Line: " <<  errLine << std::endl;
    std::cout << "Col: " <<  errColumn << std::endl;
    return;
  }
  
  QDomNode clipboardNode = doc.firstChild();
  QDomNode schemaNode = clipboardNode.firstChild();
  
  if (schemaNode.isNull())
  {
    std::cout << "Bad drone project, main schema node isNull" << std::endl;
    return;
  }
  
  QDomElement schemaElem = schemaNode.toElement();
  
  if (schemaElem.isNull())
  {
    std::cout << "Bad drone project, main schema elem isNull" << std::endl;
    return;
  }
  
  _schemaGui->getSchema()->load(schemaElem, true);
  _schemaGui->rebuildSchema();

//  _state = MOVING_GEAR; 
  //QRectF rect = getBoundingBoxOfAllSelectedGears();
  //_movingGearStartPos = rect.center();
  
}

void SchemaEditor::slotGearSelectAll()
{
  std::cerr<<"select all!!!"<<std::endl;
  selectAllGears();
}

void SchemaEditor::contextMenuEvent(QContextMenuEvent *contextMenuEvent)
{
  QPointF scenePos(mapToScene(contextMenuEvent->pos()));
  PlugBox* selectedPlugBox;
  QList<QGraphicsItem*> items = scene()->items(scenePos);
  QGraphicsItem* el;
  GearGui* gearGui=NULL;
  foreach(el, items)
  {
    if((gearGui=qgraphicsitem_cast<GearGui*>(el)))
      break;
  }
  
    if (gearGui!=NULL)
    { 
      _contextGear = gearGui;
      if(((selectedPlugBox = gearGui->plugHit(scenePos)) != 0))
      {
        _exposePlugAction->setText(selectedPlugBox->plug()->exposed() ? "Unexpose":"Expose");
        _contextPlug = selectedPlugBox;
        _plugContextMenu->popup(QCursor::pos());
      
      } 
      else
      {
        if (_contextGear->gear()->kind() == Gear::METAGEAR)
        {
          _metaGearContextMenu->popup(QCursor::pos());
        }
        else
          _gearContextMenu->popup(QCursor::pos());
      }
      return;
    }
  _contextGear = NULL;
  _contextMenuPos = contextMenuEvent->pos();
  _contextMenu->popup(mapToGlobal(contextMenuEvent->pos()));

}

void SchemaEditor::setupMatrix()
{
    QMatrix matrix;
    matrix.scale(_scale, _scale);
    setMatrix(matrix);
}


SchemaEditor::~SchemaEditor()
{

}

void SchemaEditor::zoomIn()
{
  zoom(qMin(_scale*ZOOM_FACTOR,3.0));
}

void SchemaEditor::zoomOut()
{
  zoom(qMax(_scale/ZOOM_FACTOR,0.25));
}

void SchemaEditor::zoom(float factor)
{
  _scale=factor;
  setupMatrix();
  update();     
}

/* legacy code ....... for reference
  
 * void SchemaEditor::contentsMouseDoubleClickEvent(QMouseEvent *mouseEvent)
{    

 * //handle double-click on metagear
    if (gearGui!=NULL && (gearGui->gear()->kind() == Gear::METAGEAR))
    {
      QDialog metaGearEditorDialog(this);  
      Q3VBoxLayout layout(&metaGearEditorDialog, 1);
      metaGearEditorDialog.setCaption(gearGui->gear()->name().c_str());
      metaGearEditorDialog.resize(640,480);
      MetaGearEditor metaGearEditor(&metaGearEditorDialog, (MetaGear*)gearGui->gear(), _engine);
      layout.addWidget(&metaGearEditor);
      metaGearEditorDialog.exec();
      
      //todo : temp...
      ((MetaGear*)(gearGui->gear()))->createPlugs();
}

 */

// add a gear by name and view coordinates
void SchemaEditor::addGear(std::string name, QPoint pos)
{
  // defer to QGraphicsScene, but first convert to scene Coord
  _schemaGui->addGear(name, mapToScene(pos));    
}

void SchemaEditor::addMetaGear(std::string filename, QPoint pos)
{  
  MetaGear *metaGear = _schemaGui->addMetaGear(filename, mapToScene(pos));
  associateControlPanelWithMetaGear(metaGear);
}

void SchemaEditor::associateControlPanelWithMetaGear(MetaGear *metaGear)
{
  //create and associate a control panel with this metagear
  //_panelScrollView->addControlPanel(metaGear);    
}

void SchemaEditor::dragEnterEvent(QDragEnterEvent* event)
{
  event->accept(Q3TextDrag::canDecode(event));
}

void SchemaEditor::dragMoveEvent ( QDragMoveEvent * event ) 
{
  event->accept(Q3TextDrag::canDecode(event));
}

void SchemaEditor::dropEvent(QDropEvent* event)
{
  QString text;
  
  if ( Q3TextDrag::decode(event, text) )
  {
    event->accept(true);
    addGear(text.toStdString(), event->pos());
  }
}


