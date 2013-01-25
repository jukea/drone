/* 
 * File:   HierarchyManager.h
 * Author: julien
 *
 * Created on January 25, 2013, 1:50 PM
 */

#ifndef HIERARCHYMANAGER_H
#define	HIERARCHYMANAGER_H
#include <QObject>
#include <QString>
#include <QMap>

class Schema;
class Gear;
class MetaGear;

// this manages an up to date map of the metagear uuid with their instance 
// across all project levels 
class HierarchyManager: public QObject {

  Q_OBJECT

  public:
  static HierarchyManager* getInstance();
  MetaGear* getMetaGearByUUID(QString uuid);
  HierarchyManager();
  virtual ~HierarchyManager();
  void setRootMetaGear(MetaGear*);
  MetaGear* getRootMetaGear(){return _rootMetaGear;}
  
public slots:
  
  void onGearAdded(Schema &schema,Gear &gear);
  void onGearPreRemoved(Schema &schema,Gear &gear);


private:
  QMap<QString,MetaGear*> _metaGearMap;
  MetaGear* _rootMetaGear;
  static HierarchyManager* instance;

};

#endif	/* HIERARCHYMANAGER_H */

