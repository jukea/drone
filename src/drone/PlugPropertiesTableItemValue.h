/*
 *  PlugParametersTableItemValue.h
 *  drone
 *
 *  Created by foogy on 11/09/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef PLUGParametersTABLEITEMVALUE_INCLUDED
#define PLUGParametersTABLEITEMVALUE_INCLUDED

#include "PlugParametersTableItem.h"
#include <qlineedit.h>

class PlugParametersTableItemValue : public PlugParametersTableItem
{
public:  
  PlugParametersTableItemValue(AbstractPlug *plug, QTable *table, EditType et);
  virtual ~PlugParametersTableItemValue();

  QWidget *createEditor() const;
  void setContentFromEditor(QWidget *w);
  void setText(const QString &s);


protected:
  QLineEdit *_lineEdit;
};
                               
#endif                               
