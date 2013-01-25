/* 
 * File:   GearInfoControl.cpp
 * Author: julien
 * 
 * Created on January 20, 2013, 8:38 PM
 */

#include "GearInfoControl.h"
#include <QtXml>
#include <qfile.h>
#include <qdir.h>
#include <QStringList>
#include "GearControl.h"
#include <dlfcn.h>


const QString GearInfoControl::TYPENAME = "Control";


GearInfoControl::GearInfoControl(QFileInfo pluginFile) :
	GearInfoPlugin(TYPENAME, pluginFile),
	_handle(0)
{
}

GearInfoControl::~GearInfoControl()
{
	if (_handle)
		dlclose(_handle);
}

bool GearInfoControl::bindPlugin()
{
 	qDebug() << "binding: " << _pluginFile.fileName();

	//open file
	_handle = dlopen(_pluginFile.absoluteFilePath().toAscii(), RTLD_LAZY);

	if (!_handle)
	{
		qCritical() << _pluginFile.absoluteFilePath() << " : " << dlerror();
		return false;
	}
	
	//query makeControl ptrfun interface
	//Control* (*makeControl)();
	*(void**) (&_makeControl) = dlsym(_handle, "makeControl");	
	char*e=dlerror();
	if (e)
	{
		qCritical() << _pluginFile.absoluteFilePath() << " : not a drone control!. Error: '"<<e<<"'";
		return false;
	}
  
  
  

 	return true;
}

Gear* GearInfoControl::createGearInstance()
{
  BaseControl* control;
  if (_makeControl)
    control = dynamic_cast<BaseControl*>(_makeControl());
  else
    return NULL;

  qDebug()<<control;
  
  GearMaker* gm(GearMaker::instance());
  QString gt(control->getGearType());
  GearControl* gear = (GearControl*)gm->makeGear(gt);
  if(!gear)
    return NULL;
  gear->setControl(control);
  control->setGear(gear);
  return gear;
}
