/* 
 * File:   BaseControl.h
 * Author: julien
 *
 * Created on January 24, 2013, 11:23 PM
 */

#ifndef BASECONTROL_H
#define	BASECONTROL_H
#include <QString>
class Gear;

class BaseControl {
public:
  BaseControl();
  virtual ~BaseControl();
  virtual void onUpdate(){}
  virtual void setGear(Gear* g){_parentGear=g;}
  virtual void runVideo(){}
  
  // must return the GearControl class (Ex: Gear_PushButton)
  virtual QString getGearType()=0;
protected:
  Gear* _parentGear;

};

#endif	/* BASECONTROL_H */

