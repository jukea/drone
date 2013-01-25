#ifndef CONTROL_INCLUDED
#define CONTROL_INCLUDED

#include <qgraphicswidget.h>
#include "gui/BaseControl.h"

class ControlPanel;
class Gear;

class Control : public QGraphicsWidget, public BaseControl
{
public:  
  Control();
  virtual ~Control();
private:
  ControlPanel *_parentControlPanel;
  
};

#endif
