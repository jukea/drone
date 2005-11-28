#include "PlugParametersTableItemValue.h"
#include "AbstractPlug.h"
#include "ValueType.h"

PlugParametersTableItemValue::PlugParametersTableItemValue(AbstractPlug *plug, QTable * table, EditType et) :
  PlugParametersTableItem(plug, table, et, QString::number(static_cast<ValueType*>(plug->abstractDefaultType())->value())),
  _lineEdit(NULL)
{
  setReplaceable(false);  
}

PlugParametersTableItemValue::~PlugParametersTableItemValue()
{

}

QWidget *PlugParametersTableItemValue::createEditor() const
{
  std::cout << "createEditor" << std::endl;
  //create a lineedit
  ((PlugParametersTableItemValue*)this)->_lineEdit = new QLineEdit(table()->viewport());
  _lineEdit->setText(QString::number(static_cast<ValueType*>(_plug->abstractDefaultType())->value()));  
  
  QObject::connect(_lineEdit, SIGNAL( textChanged(const QString&)), table(), SLOT( doValueChanged()) );
  return _lineEdit;
}

void PlugParametersTableItemValue::setContentFromEditor(QWidget *w)
{
  std::cout << "setcontentfromeditor" << std::endl;
  if (w->inherits("QLineEdit"))
    setText(((QLineEdit*)w)->text());
  else
    QTableItem::setContentFromEditor(w);
}

void PlugParametersTableItemValue::setText(const QString &s)
{
  std::cout << "settext" << std::endl;
  ValueType *data = static_cast<ValueType*>(_plug->abstractDefaultType());
  data->setValue(s.toFloat());
  
  if (_lineEdit)
    _lineEdit->setText(s);
  
  QTableItem::setText(s);
}


