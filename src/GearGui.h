#ifndef GEARGUI_INCLUDED
#define GEARGUI_INCLUDED

#include <vector>
#include <qcanvas.h>
#include <qobject.h>


class Gear;
class PlugBox;
class Engine;
class QDomElement;
class QDomDocument;

class GearGui : public QCanvasRectangle, QObject
{
public:
  static const int CANVAS_RTTI_GEAR;
  static const int DEFAULT_SIZEX;
  static const int NAME_SIZEY;
  static const int PLUGBOXES_NOMINAL_INTERVAL;
  static const int PLUGBOXES_STARTING_OFFSET;
  static const int SHADOW_OFFSET;


  virtual int rtti() const;

  GearGui(Gear *pgear, QCanvas *canvas, int sizeX=DEFAULT_SIZEX, int sizeY=0, int updateRate=-1);
  virtual ~GearGui();

  PlugBox* getInputPlugBox(std::string name) const;
  PlugBox* getOutputPlugBox(std::string name) const;

  Gear* gear(){return _gear;};

  bool titleBarHitted(const QPoint& p);

  PlugBox* plugHitted(const QPoint& p);
  void performPlugHighligthing(const QPoint &p, PlugBox *onlyCompatibleWith=NULL);

  void setTitle(std::string title){_title=title;};

  void reDraw();

  virtual void save(QDomDocument &doc, QDomElement &gearElem);
  virtual void load(QDomElement &gearElem);

  virtual void mouseEvent(const QPoint&, Qt::ButtonState){}


  void getDrawableArea(int *ox, int *oy, int *sizeX, int *sizeY);

protected:

  void timerEvent(QTimerEvent*);

  virtual void drawShape(QPainter &painter);
  void drawPlugBoxes(QPainter &painter);



  Gear *_gear;
  std::vector<PlugBox*> _inputPlugBoxes;
  std::vector<PlugBox*> _outputPlugBoxes;
  std::vector<PlugBox*> _plugBoxes;//!contain all inputs and outputs
  int _sizeX;
  int _sizeY;
  int _inputsInterval;
  int _outputsInterval;
  std::string _title;


  //colors    
  static const QColor BOX_COLOR;
  static const QColor BOXNAME_COLOR;
  static const QColor SHADOW_COLOR;

  //fonts
  static const QFont NAME_FONT;

};

#endif

