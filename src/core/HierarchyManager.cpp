/* 
 * File:   HierarchyManager.cpp
 * Author: julien
 * 
 * Created on January 25, 2013, 1:50 PM
 */

#include "MetaGear.h"
#include "HierarchyManager.h"

HierarchyManager *HierarchyManager::instance=0;

HierarchyManager::HierarchyManager():_rootMetaGear(NULL)
{
}


HierarchyManager::~HierarchyManager()
{
}

HierarchyManager* HierarchyManager::getInstance()
{
  if (!instance)
  {
    instance = new HierarchyManager();
  }
  return instance;
}

void HierarchyManager::onGearAdded(Schema &schema,Gear &gear)
{
  Q_UNUSED(schema);

  MetaGear* mg;
  if((mg=qobject_cast<MetaGear*>(&gear)))
  {  qDebug()<<"MW:"<<gear.getUUID()<<" is a metagear ! it has been creatd ! ";
    _metaGearMap[gear.getUUID()]=mg;
  }
  qDebug()<<"Updated HM map :"<< _metaGearMap;
}

void HierarchyManager::onGearPreRemoved(Schema &schema,Gear &gear)
{
  Q_UNUSED(schema);
  Q_UNUSED(schema);

  MetaGear* mg;
  if((mg=qobject_cast<MetaGear*>(&gear)))
  {
    qDebug()<<"MW:"<<gear.getUUID()<<" is a metagear ! it has been deleted! ";
    _metaGearMap.remove(gear.getUUID());
  }
  qDebug()<<"Updated HM map :"<< _metaGearMap;
}


void HierarchyManager::setRootMetaGear(MetaGear* mg)
{
  _rootMetaGear=mg;
  _metaGearMap[mg->getUUID()]=mg;
}

MetaGear* HierarchyManager::getMetaGearByUUID(QString uuid)
{
  qDebug()<<"looking for:"<<uuid<< " in "<<_metaGearMap;
  return _metaGearMap[uuid];
}

