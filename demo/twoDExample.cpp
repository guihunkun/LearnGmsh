#include <gmsh.h>
#include <iostream>
#include <vector> 
#include <fstream>
#include <iomanip>
#include <string>
using namespace std;


class Point
{
  public:
    Point(const double x = 0, const double y = 0, const double z = 0):_x(x), _y(y), _z(z) {}
    ~Point() {}
    const double getX() const {return _x;}
    const double getY() const {return _y;}
    const double getZ() const {return _z;}
    void setX(const double x) {_x = x;}
    void setY(const double y) {_y = y;}
    void setZ(const double z) {_z = z;}

  private:
    double _x, _y, _z;
};



class Triangle
{
  /**
   * Triangle vertex index
   * _id[0] ~ _id[2] : three vertices index of triangle
   * _id[3] is the midpoint between _id[0] and _id[1]
   * _id[4] is the midpoint between _id[1] and _id[2]
   * _id[5] is the midpoint between _id[2] and _id[0]
   */
  public:
    Triangle() {}
    ~Triangle() {}
    const vector<unsigned int>& getIds() const {return _ids;}
    void clear() {_ids.clear();}
    void append(unsigned int id) {_ids.push_back(id);}
    void resize(const unsigned int size) {_ids.resize(size);}

  private:
    vector<unsigned int> _ids;
};



void generateBoudaryAndHoles(vector<Point>& boundary, vector<vector<Point> >& holes)
{
  double left, right, bot, top;
  double x, y, z;
  left = 0;
  right = 5;
  bot = 0;
  top = 4;
  z = 0;
  Point p;
  p = Point(left, bot, z);
  boundary.push_back(p);
  p = Point(right, bot, z);
  boundary.push_back(p);
  p = Point(right, top, z);
  boundary.push_back(p);
  p = Point(left, top, z);
  boundary.push_back(p);
  p = Point(left, (bot+top)/2.0, z);
  boundary.push_back(p);


  vector<Point> hole;
  left = 1;
  right = 2;
  bot = 1;
  top = 2;
  p = Point(left, bot, z);
  hole.push_back(p);
  p = Point(right, bot, z);
  hole.push_back(p);
  p = Point(right, top, z);
  hole.push_back(p);
  p = Point(left, top, z);
  hole.push_back(p);
  p = Point(left, (bot+top)/2.0, z);
  hole.push_back(p);
  holes.push_back(hole);


  hole.clear();
  left = 3;
  right = 4;
  bot = 1;
  top = 2;
  p = Point(left, bot, z);
  hole.push_back(p);
  p = Point(right, bot, z);
  hole.push_back(p);
  p = Point(right, top, z);
  hole.push_back(p);
  p = Point(left, top, z);
  hole.push_back(p);
  holes.push_back(hole);
}


int main(int argc, char **argv)
{
  gmsh::initialize();
  gmsh::option::setNumber("General.Terminal", 1);
    
  gmsh::model::add("demo");

  double boundartLc = 1, holeLc = 0.4;
  vector<Point> boundary;
  vector<vector<Point> > holes;
  generateBoudaryAndHoles(boundary, holes);

  int dim = 1;
  int tag = 1;
  double x, y, z;
  vector<int> loop, curveLoop, planeSurface;

  int boundaryTag;
  for(int i = 0; i < boundary.size(); i++)
  {
    x = boundary[i].getX();
    y = boundary[i].getY();
    z = boundary[i].getZ();
    loop.push_back(gmsh::model::geo::addPoint(x, y, z, boundartLc));
  }

  for(int i = 0; i < loop.size(); i++)
  {
    curveLoop.push_back(gmsh::model::geo::addLine(loop[i], i == loop.size()-1 ? loop[0] : loop[i+1]));
  }
  planeSurface.push_back(gmsh::model::geo::addCurveLoop(curveLoop));
  boundaryTag = gmsh::model::addPhysicalGroup(dim, curveLoop);
  gmsh::model::setPhysicalName(dim, boundaryTag, "Boundary");

  vector<int> bcurveLoop = curveLoop;

  vector<vector<int> > recCurveLoop;
  vector<int> holesTags;
  for(int i = 0; i < holes.size(); i++)
  {
    loop.clear();
    curveLoop.clear();
    for(int j = 0; j < holes[i].size(); j++)
    {
      x = holes[i][j].getX();
      y = holes[i][j].getY();
      z = holes[i][j].getZ();
      loop.push_back(gmsh::model::geo::addPoint(x, y, z, holeLc));
    }

    for(int j = 0; j < loop.size(); j++)
    {
      curveLoop.push_back(gmsh::model::geo::addLine(loop[j], j == loop.size()-1 ? loop[0] : loop[j+1]));
    }
    planeSurface.push_back(-gmsh::model::geo::addCurveLoop(curveLoop));
    
    tag = gmsh::model::addPhysicalGroup(dim, curveLoop);
    holesTags.push_back(tag);
    string name = "hole" + to_string(i);
    gmsh::model::setPhysicalName(dim, tag, name);
    recCurveLoop.push_back(curveLoop);
  }

  gmsh::model::geo::addPlaneSurface(planeSurface);

  dim = 2;
  gmsh::model::addPhysicalGroup(dim, planeSurface);

  gmsh::model::geo::synchronize();

  gmsh::model::mesh::generate(2);


  // get mesh information
  // Nodes
  dim = -1;
  tag = -1;
  bool includeBoundary = false;
  vector<double> coord, parametricCoord;
  vector<size_t> nodeTags;
  gmsh::model::mesh::getNodes(nodeTags, coord, parametricCoord, dim, tag, includeBoundary, true);
  cout<<"The number of nodes: "<<coord.size()/3<<endl;
  vector<Point> nodes;
  nodes.reserve(1000);
  Point p;
  for(int i = 0; i < coord.size(); i+=3)
  {
    x = coord[i];
    y = coord[i+1];
    z = coord[i+2];
    p = Point(x, y, z);
    nodes.push_back(p);
  }



  //Elements
  dim = -1;
  tag = -1;
  vector<int> elementTypes;
  vector<vector<size_t> > elementTags, nodeTagss;
  gmsh::model::mesh::getElements(elementTypes, elementTags, nodeTagss, dim, tag);
  cout<<"The number of elements: "<<nodeTagss[1].size()/3<<endl;
  // boundaryNodesId include holes boundary nodes id
  // vector<size_t> boundaryNodesId = elementTags[0];
  vector<Triangle> triangles;
  triangles.reserve(1000);
  unsigned int id1, id2, id3;
  for(int i = 0; i < nodeTagss[1].size(); i += 3)
  {
    Triangle tri;
    id1 = nodeTagss[1][i]-1;
    id2 = nodeTagss[1][i+1]-1;
    id3 = nodeTagss[1][i+2]-1;
    tri.append(id1);
    tri.append(id2);
    tri.append(id3);
    triangles.push_back(tri);
  }
  


  // get boundary nodes id and it's coordinate information
  // coord is coordinate information (x1, y1, z1, .....)
  dim = 1;
  vector<size_t> bouNodesIds;
  gmsh::model::mesh::getNodesForPhysicalGroup(dim, boundaryTag, bouNodesIds, coord);
  vector<unsigned int> boundaryNodesIds(bouNodesIds.size());
  // just because index from start 0
  for(int i = 0; i < bouNodesIds.size(); i++)
  {
    boundaryNodesIds[i] = bouNodesIds[i]-1;
  }


  // get holes nodes id and it's coordinate information
  vector<vector<size_t> > hosNodesIds(holesTags.size());
  vector<vector<unsigned int> > holesNodesIds(holesTags.size());
  for(int i = 0; i < holesTags.size(); i++)
  {
    gmsh::model::mesh::getNodesForPhysicalGroup(dim, holesTags[i], hosNodesIds[i], coord);
  }
  for(int i = 0; i < hosNodesIds.size(); i++)
  {
    holesNodesIds[i].resize(hosNodesIds[i].size());
    for(int j = 0; j < hosNodesIds[i].size(); j++)
    {
      holesNodesIds[i][j] = hosNodesIds[i][j]-1;
    }
  }

#if 0
  for(int i = 0; i < holesNodesIds.size(); i++)
  {
    for(int j = 0; j < holesNodesIds[i].size(); j++)
    {
      cout<<holesNodesIds[i][j]<<" ";
    }
    cout<<endl;
  }
#endif



  // get holes arround triangle id
  vector<vector<int> > holesElementsIds(recCurveLoop.size());
  dim = 1;
  int dim2 = 2;
  vector<size_t> elementTag;
  bool strict = true;
  double xx, yy, zz;
  vector<int> holeElementsIds;
  for(int i = 0; i < recCurveLoop.size(); i++)
  {
    holeElementsIds.clear();
    for(int j = 0; j < recCurveLoop[i].size(); j++)
    {
      //elementTypes.clear();
      //elementTags.clear();
      //nodeTagss.clear();
      tag = recCurveLoop[i][j];
      gmsh::model::mesh::getElements(elementTypes, elementTags, nodeTagss, dim, tag);
      for(int k = 0; k < nodeTagss[0].size(); k += 2)
      {
        id1 = nodeTagss[0][k]-1;
        id2 = nodeTagss[0][k+1]-1;
        xx = (nodes[id1].getX() + nodes[id2].getX())/2.0;
        yy = (nodes[id1].getY() + nodes[id2].getY())/2.0;
        zz = (nodes[id1].getZ() + nodes[id2].getZ())/2.0;
        gmsh::model::mesh::getElementsByCoordinates(xx, yy, zz, elementTag, dim2, strict);
        for(int kk = 0; kk < elementTag.size(); kk++)
        {
          holeElementsIds.push_back(elementTag[kk]-1);
        }
      }
    }
    holesElementsIds.push_back(holeElementsIds);
  }
#if 0
  for(int i = 0; i < holesElementsIds.size(); i++)
  {
    for(int j = 0; j < holesElementsIds[i].size(); j++)
    {
      cout<<holesElementsIds[i][j]<<" ";
    }
    cout<<endl;
  }
#endif

  // get boundary arround triangle id
  vector<int> boundaryElementsIds;
  dim = 1;
  dim2 = 2;
  strict = true;
  for(int i = 0; i < bcurveLoop.size(); i++)
  {
    tag = bcurveLoop [i];
    gmsh::model::mesh::getElements(elementTypes, elementTags, nodeTagss, dim, tag);
    for(int k = 0; k < nodeTagss[0].size(); k += 2)
    {
      id1 = nodeTagss[0][k]-1;
      id2 = nodeTagss[0][k+1]-1;
      xx = (nodes[id1].getX() + nodes[id2].getX())/2.0;
      yy = (nodes[id1].getY() + nodes[id2].getY())/2.0;
      zz = (nodes[id1].getZ() + nodes[id2].getZ())/2.0;
      gmsh::model::mesh::getElementsByCoordinates(xx, yy, zz, elementTag, dim2, strict);
      for(int kk = 0; kk < elementTag.size(); kk++)
      {
        boundaryElementsIds.push_back(elementTag[kk]-1);
      }
    }
  }

  gmsh::write("demo.msh");
    
  gmsh::finalize();
  return 0;
}
