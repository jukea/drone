#include "ValueType.h"

#include <QtXml>

const QString ValueType::TYPENAME = "Value";


void ValueType::save(QDomDocument &doc, QDomElement &parent) const
{
  QDomAttr valAttr;
  valAttr = doc.createAttribute("Value");
  valAttr.setValue(QString::number(_value));	
  parent.setAttributeNode(valAttr);
 
}

void ValueType::load(QDomElement &typeElem)
{
  _value = typeElem.attribute("Value","0").toFloat();
}
