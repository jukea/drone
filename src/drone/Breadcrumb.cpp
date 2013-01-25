/* 
 * File:   Breadcrumb.cpp
 * Author: julien
 * 
 * Created on January 25, 2013, 1:59 AM
 */

#include "Breadcrumb.h"
#include "MainWindow.h"
#include <QDebug>
#include <QPainter>
#include <QStringList>
#include "MetaGear.h"

Breadcrumb::Breadcrumb(QWidget * parent):
QAbstractButton(parent)
{
  setFixedHeight(20);  
  QObject::connect(this,SIGNAL(clicked()),this,SLOT(onClick()));
}

void Breadcrumb::setValue(QList<MetaGear*> list)
{
  _list=list;
  QStringList strList;
  foreach(MetaGear* mg,_list)
  {
    strList<<mg->instanceName();
  }
  _str=strList.join(" > ");
  repaint();

}


void Breadcrumb::onClick()
{
  if(_list.count()<2)
    return;
  _list.removeLast();
  MainWindow::getInstance()->openMetaGear(_list.last()->getUUID());
  qDebug()<<"Youhou!!";
}

void Breadcrumb::paintEvent(QPaintEvent *e)
{
  QPainter qp(this);
  drawWidget(qp);
  
  QWidget::paintEvent(e);  
}

void Breadcrumb::drawWidget(QPainter &qp)
{
  qp.drawText(20,15, _str);
}


Breadcrumb::~Breadcrumb()
{
}

