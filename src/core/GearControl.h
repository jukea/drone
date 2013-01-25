#ifndef GEARCONTROL_INCLUDED
#define GEARCONTROL_INCLUDED

#include "Gear.h"

class BaseControl;


class GearControl : public Gear
{
  Q_OBJECT

public:

  GearControl(QString type);

  virtual ~GearControl(){}
  void setControl(BaseControl* control);
  BaseControl* getControl(){return _control;}
  
protected:
  BaseControl* _control;
  
};

#endif
