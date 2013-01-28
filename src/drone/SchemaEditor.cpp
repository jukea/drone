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
#include "MainWindow.h"
#include "GearGui.h"
#include "ConnectionItem.h"
#include "PlugBox.h"
#include "Engine.h"
#include "Gear.h"
#include "MetaGear.h"
#include "commands/CommandGeneric.h"

#include "PanelScrollView.h"
#include "ControlPanel.h"
#include "GearListMenu.h"
#include "MetaGearListMenu.h"
#include "GearPropertiesDialog.h"
#include "gearFactory/GearMaker.h"
#include "gearFactory/GearMaker.h"

#include <Q3MainWindow>
#include <Q3FileDialog>
#include <Q3DragObject>

#include <iostream>

#include <QCursor>
#include <QLayout>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <QGLWidget>
#include <iostream>
#include <QPainter>
#include <QMatrix>

const QString SchemaEditor::NAME = "SchemaEditor";
const double SchemaEditor::ZOOM_FACTOR = 1.4;

SchemaEditor::SchemaEditor(QWidget *parent, SchemaGui *schemaGui, Engine * engine, PanelScrollView *panelScrollView) :
  QGraphicsView(schemaGui, parent),
  _contextMenuPos(0,0),
  _contextGear(NULL),
  _engine(engine),
  _schemaGui(schemaGui),
  _scale(1),
  _maxZValue(1),
  _selectionChangeNotificationBypass(false),
  _panelScrollView(panelScrollView)
  

{
  _schemaGui->setSchemaEditor(this);
  setDragMode(QGraphicsView::RubberBandDrag);
  setRenderHint(QPainter::Antialiasing, true);

  //setFrameStyle(Sunken | StyledPanel);
  setOptimizationFlags(QGraphicsView::DontSavePainterState);
  //setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
  

  // render with OpenGL
  if(1)
    setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers)));

  resetTransform();
  setAcceptDrops(TRUE);
}


QPropertyAnimation* SchemaEditor::getDiveInAnimation( bool in )
{
  QPropertyAnimation* diveInScaleAnimation;  

  diveInScaleAnimation = new QPropertyAnimation(this, "_scale");
  diveInScaleAnimation->setDuration(600);
  diveInScaleAnimation->setStartValue(in?_scale:15);
  diveInScaleAnimation->setEndValue(in?15:1);
  if(in)
    diveInScaleAnimation->setEasingCurve(QEasingCurve::InQuint);
  else
    diveInScaleAnimation->setEasingCurve(QEasingCurve::OutQuint);
  return diveInScaleAnimation;
}

void SchemaEditor::setSchemaGui(SchemaGui* sg)
{
  _schemaGui=sg;
  setScene(sg);
  zoom(1);
  _schemaGui->setSchemaEditor(this);

}

void SchemaEditor::buildContextMenus()
{
  _contextMenu = new QMenu();
  _gearListMenu = new GearListMenu(_contextMenu);    
  _gearListMenu->create();
  MainWindow* mainWindow = MainWindow::getInstance();

  QMenu* newGearMenu = _contextMenu->addMenu("Add gear");

  QAction * sub = newGearMenu->addMenu(_gearListMenu);
  sub->setText("Gears");
  
  QObject::connect(_gearListMenu, SIGNAL(gearSelected(QString)), this, SLOT(onMenuGearSelected(QString)));

  _metaGearListMenu = new MetaGearListMenu(_contextMenu);    
  _metaGearListMenu->create();
  
  QAction* mgSubAction = newGearMenu->addMenu(_metaGearListMenu);
  mgSubAction->setText("MetaGears");
  
  QObject::connect(_metaGearListMenu, SIGNAL(metaGearSelected(QFileInfo*)), this, SLOT(onMenuMetaGearSelected(QFileInfo*)));

  _contextMenu->addAction("New MetaGear", this, SLOT(onNewMetaGear()));
  _contextMenu->addAction(mainWindow->_actZoomIn);  
	_contextMenu->addAction(mainWindow->_actZoomOut);  
  _contextMenu->addAction(mainWindow->_actSelectAll);
  _contextMenu->addAction(mainWindow->_actDeleteSelected);
  _contextMenu->addAction(mainWindow->_actCopy);
  _contextMenu->addAction(mainWindow->_actPaste);
	
  
  _gearContextMenu = new QMenu();
  _gearContextMenu->addAction("Properties", this, SLOT(onGearProperties()));

  _gearContextMenu->addAction("About");    
  
  _metaGearContextMenu = new QMenu();
  _metaGearContextMenu->addAction("Properties", this, SLOT(onGearProperties()));  
  _metaGearContextMenu->addAction("About");    
  _metaGearContextMenu->addSeparator();
  _metaGearContextMenu->addAction("Save MetaGear",  this, SLOT(onSaveMetaGear()));


  // plug context menu initialization
  _plugContextMenu = new QMenu();
  _exposePlugAction = _plugContextMenu->addAction("Expose", this, SLOT(onPlugToggleExpose()));

}





void SchemaEditor::onMenuGearSelected(QString name)
{        
  addGear(name.ascii(), _contextMenuPos);    
}

void SchemaEditor::onMenuMetaGearSelected(QFileInfo* metaGearFileInfo)
{  
  addMetaGear(metaGearFileInfo->filePath().ascii(), _contextMenuPos);    
}

void SchemaEditor::onGearProperties()
{
  if (_contextGear == NULL)
    return;

  GearPropertiesDialog *gearPropertiesDialog;

  gearPropertiesDialog = new GearPropertiesDialog(NULL, _contextGear->gear(), _engine);
  gearPropertiesDialog->exec();
  delete gearPropertiesDialog;

  _contextGear=NULL;

}

void SchemaEditor::keyPressEvent(QKeyEvent *e)
{
  if (e->key() == Qt::Key_Backspace || e->key() == Qt::Key_Delete)
    deleteSelected();
}


void SchemaEditor::onDeleteSelected()
{
  deleteSelected();
}

/**
 * React on Expose selection in the plug context menu
 */
void SchemaEditor::onPlugToggleExpose()
{
  _contextPlug->plug()->exposed(!_contextPlug->plug()->exposed());
  _contextPlug->getGearGui()->update();
//  scene()->update();
} 


void SchemaEditor::onNewMetaGear()
{
  _schemaGui->newMetaGear(mapToScene(_contextMenuPos));
}

void SchemaEditor::onSaveMetaGear()
{
  /*
  if (!_contextGear->gear()->kind() == Gear::METAGEAR)
  {
    std::cout << "not a metagear, cannot save!!!" << std::endl; 
    return;
  }
   
  MetaGear* metaGear = (MetaGear*)_contextGear->gear();
  
  //mangle suggested filename
  QString suggestedFilename=metaGear->fullPath();  
//  if (suggestedFilename.empty())
//    suggestedFilename=MetaGearMaker::METAGEAR_PATH + "/" + metaGear->name();
  

  QString filename = Q3FileDialog::getSaveFileName(suggestedFilename.c_str(), ("*" + MetaGear::EXTENSION + ";;" + "*.*").c_str(), 
                                                      0, "Save as", "Save as").toStdString();
  
  if (!filename.empty())
  {
    //set the extension if not already present
    if (filename.find(MetaGear::EXTENSION.c_str(), filename.size()-MetaGear::EXTENSION.size())==QString::npos)
      filename.append(MetaGear::EXTENSION);  
    
    metaGear->save(filename);    
    
    //rename the metagear to the new saved name
    QFileInfo fileInfo(filename.c_str());
    _schemaGui->renameGear(_contextGear, fileInfo.baseName().toStdString());
    
  }
*/
}

void SchemaEditor::deleteSelected()
{
  CommandGeneric* com = new CommandGeneric();
  QList<GearGui*> allGears = _schemaGui->getSelectedGears();

  foreach(GearGui* gg,allGears)
    _schemaGui->removeGear(gg);
  _contextGear = NULL;

  com->saveSnapshotAfter();
  com->setText(QString("Deleted selected items"));

  MainWindow::getInstance()->pushUndoCommand(com);
}




void SchemaEditor::onGearCopy()
{
  _schemaGui->resetPasteOffset();
  QDomDocument doc("Clipboard");
  
  QDomElement clipboardElem = doc.createElement("Clipboard");
  doc.appendChild(clipboardElem);

  if(!_schemaGui->save(doc, clipboardElem,true))
    return;

  //save to file  
  QString str;
  QTextStream stream(&str,QIODevice::WriteOnly);
  doc.save(stream,4);
  _engine->setClipboardText(str.latin1());
  qDebug()<<"copied! size:"<<_engine->getClipboardText().length();
  //qDebug()<<"copied!:"<<_engine->getClipboardText();
  
}

void SchemaEditor::onGearPaste()
{
  
  CommandGeneric* com = new CommandGeneric();
  
  
  
  _schemaGui->clearSelection();
  QDomDocument doc("Clipboard");

  QString str(_engine->getClipboardText());

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
  
  _schemaGui->setSelectionChangeNotificationBypass(true);
  _schemaGui->setAutoSelectNewElements(true);
  
  _schemaGui->getSchema()->load(schemaElem, Drone::PasteFromClipboard);
  
  _schemaGui->setAutoSelectNewElements(false);
  _schemaGui->setSelectionChangeNotificationBypass(false);
  _schemaGui->selectionHasChanged();
  _schemaGui->moveItemsBy(_schemaGui->selectedItems(),_schemaGui->getPasteOffset());
  //_schemaGui->rebuildSchema();
  
  com->saveSnapshotAfter();
  com->setText(QString("Pasted"));
  MainWindow::getInstance()->pushUndoCommand(com);

}

void SchemaEditor::onSelectAll()
{
  QList<QGraphicsItem*> all = _schemaGui->items();
  QGraphicsItem* el=NULL;
  if(!all.count())
    return;
  _schemaGui->setSelectionChangeNotificationBypass(true);
  foreach(el,all)
  {
    el->setSelected(true);
  }
  el->setSelected(false);
  _schemaGui->setSelectionChangeNotificationBypass(false);
// hack to emit signal because we can't manually
  el->setSelected(true);
  update();
  
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
      if(((selectedPlugBox = gearGui->plugHit(gearGui->mapFromScene(scenePos))) != 0))
      {
        _exposePlugAction->setText(selectedPlugBox->plug()->exposed() ? "Unexpose":"Expose");
        _contextPlug = selectedPlugBox;
        _plugContextMenu->popup(QCursor::pos());
      
      } 
      else
      {
        
        // reenable for metagear restoration
        //if (_contextGear->gear()->kind() == Gear::METAGEAR)
        //{
        //  _metaGearContextMenu->popup(QCursor::pos());
        //}
        //else
          _gearContextMenu->popup(QCursor::pos());
      }
      contextMenuEvent->accept();
      return;
    }
  _contextGear = NULL;
  _contextMenuPos = contextMenuEvent->pos();
  _contextMenu->popup(mapToGlobal(contextMenuEvent->pos()));
  contextMenuEvent->accept();

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

void SchemaEditor::onZoomIn()
{
  zoom(qMin(_scale*ZOOM_FACTOR,3.0));
}

void SchemaEditor::onZoomOut()
{
  zoom(qMax(_scale/ZOOM_FACTOR,0.25));
}

void SchemaEditor::zoom(float factor)
{
  _scale=factor;
  setupMatrix();
  update();     
}

// add a gear by name and view coordinates
void SchemaEditor::addGear(QString name, QPoint pos)
{
  // defer to QGraphicsScene, but first convert to scene Coord
  _schemaGui->addGear(name, mapToScene(pos));    
}

void SchemaEditor::addMetaGear(QString filename, QPoint pos)
{  
  MetaGear *metaGear = _schemaGui->addMetaGear(filename, mapToScene(pos));
  associateControlPanelWithMetaGear(metaGear);
}

void SchemaEditor::associateControlPanelWithMetaGear(MetaGear *metaGear)
{
  Q_UNUSED(metaGear);
  //create and associate a control panel with this metagear
  //_panelScrollView->addControlPanel(metaGear);    
}

void SchemaEditor::dragEnterEvent(QDragEnterEvent *event)
 {
     if (event->mimeData()->hasText()) {
             event->accept();
             //qDebug()<<"accept!";
     } else {
         event->ignore();
     }
 }


void SchemaEditor::dragMoveEvent ( QDragMoveEvent * event ) 
{
     if (event->mimeData()->hasText()) {
             event->accept();
             //qDebug()<<"accept move!";
     } else {
         event->ignore();
     }
}

void SchemaEditor::dropEvent(QDropEvent* event)
{
  QString text;
     if (event->mimeData()->hasText()) {
             event->accept();
            addGear(event->mimeData()->text(), event->pos());
     } else {
         event->ignore();
     }
  
}


