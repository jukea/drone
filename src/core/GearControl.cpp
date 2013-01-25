#include "GearControl.h"

GearControl::GearControl(QString type) :
  Gear(type),
  _control(NULL)
{

}

void GearControl::setControl(BaseControl* control)
{
  _control=control;
}