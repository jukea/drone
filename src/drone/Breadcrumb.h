/* 
 * File:   Breadcrumb.h
 * Author: julien
 *
 * Created on January 25, 2013, 1:59 AM
 */

#ifndef BREADCRUMB_H
#define	BREADCRUMB_H

class MetaGear;

#include <QPushButton>

class Breadcrumb: public QAbstractButton {

  Q_OBJECT
public:
  Breadcrumb(QWidget * parent);
  virtual ~Breadcrumb();

public slots:
  void onClick();
    
  void setValue(QList<MetaGear*> b);

  private:
  void paintEvent(QPaintEvent *e);
  void drawWidget(QPainter &qp);
  QList<MetaGear*> _list;
  QString _str;
};

#endif	/* BREADCRUMB_H */

