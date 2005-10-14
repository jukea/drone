#ifndef TA_CITYGRAPH_INCLUDED
#define TA_CITYGRAPH_INCLUDED

#include "SimpleGraph.h"
#include "Array2D.h"
#include <string>

struct TA_CityVertex
{
  float x;
  float y;
  float energy;
  std::vector<std::string> clipFileNames;
  
  void *data;
};

float distance(TA_CityVertex *a, TA_CityVertex *b)
{
  float xdiff = (a->x - b->x);
  float ydiff = (a->y - b->y);
  return sqrt( xdiff*xdiff + ydiff*ydiff );
}

typedef Array2D<bool> TA_Grid;
typedef Array2D<TA_CityVertex*> TA_CentroidGrid;

class TA_CityGraph : public SimpleGraph<TA_CityVertex*>
{
public:
  TA_CityGraph(size_type nHotSpots, size_type nCentroids);
  TA_CityGraph(const std::string& filename);
  ~TA_CityGraph() {}

  void update(TA_Grid *grid);

protected:
  TA_CentroidGrid _gridCentroids;
};

#endif
