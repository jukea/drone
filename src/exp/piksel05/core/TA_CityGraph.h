#ifndef TA_CITYGRAPH_INCLUDED
#define TA_CITYGRAPH_INCLUDED

#include "SimpleGraph.h"
#include "Array2D.h"
#include "OSCUtils.h"
#include <string>
#include <cmath>

#include <qdom.h>
#include <qfile.h>

// A point in the city.
class TA_Point
{
public:
  float x;
  float y;

  TA_Point() : x(0), y(0) {}
  TA_Point(float x_, float y_) : x(x_), y(y_) { }
};

// A point that is part of the city graph (usually refered to as a "spot"), with attached information.
class TA_CityVertex : public TA_Point
{
  typedef TA_Point parent;

public:
  float energy;
  std::vector<std::string> clipFileNames;
  void *data;

  TA_CityVertex() : parent() {}
  TA_CityVertex(float x_, float y_, float start_energy = 0.0f)
    : parent(x_, y_), energy(start_energy), data(0)
  { }
  
};

typedef TA_Point TA_Centroid;

// Distance between two points.
float distance(TA_Point& a, TA_Point& b);

typedef Array2D<bool> TA_Grid;
typedef SimpleGraph<TA_Centroid> TA_CentroidGrid;

// The graph of the city.
class TA_CityGraph : public SimpleGraph<TA_CityVertex>
{
  static const std::string OSC_PATH_LOCATION;
public:
  TA_CityGraph() {}
  TA_CityGraph(const std::string& filename);
  virtual ~TA_CityGraph() ;

  void update(TA_Grid *grid);

  virtual void load(const std::string& filename);

protected:
  TA_CentroidGrid _gridCentroids;
};

#endif
