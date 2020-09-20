#include <gmsh.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
#include <string>
#include <map>
#include <set>

using namespace std;

class Point
{
public:
    Point(const double x = 0, const double y = 0, const double z = 0):_x(x), _y(y), _z(z) { }
    ~Point() { }
    const double getX() const {return _x;}
    const double getY() const {return _y;}
    const double getZ() const {return _z;}
    void setX(const double x) {_x = x;}
    void setY(const double y) {_y = y;}
    void setZ(const double z) {_z = z;}
    bool operator < (const Point& p) const;
    
protected:
    double _x, _y, _z;
};

bool Point::operator < (const Point& p) const
{
    double px = p.getX();
    double py = p.getY();
    if(px == _x) {
        return py < _y;
    } else {
        return px < _x;
    }
}


class Polygon
{
public:
    Polygon() : _vertex(), _name() {}
    Polygon(const vector<Point>& vertex, const string& name = "poly") :_vertex(vertex), _name(name) {}
    ~Polygon() {}
    const vector<Point>& getVertex() const {return _vertex;}
    void clear() {_vertex.clear();}
    void append(const Point& p) {_vertex.push_back(p);}
    const string& getName() const {return _name;}
    void setPoints(const vector<Point>& vertex) {_vertex = vertex;}
    void setName(const string& name) {_name = name;}
  
private:
    vector<Point> _vertex;
    string _name;
};

class Topology
{
public:
    Topology(const int firstId = -3, const int secondId = -3) : _firstId(firstId), _secondId(secondId) {}
    ~Topology() {}
    const int getFirstId() const {return _firstId;}
    const int getSecondId() const {return _secondId;}
    void setFirstId(const int firstId) {_firstId = firstId;}
    void setSecondId(const int secondId) {_secondId= secondId;}
    bool operator < (const Topology& topo) const;
    
private:
    int _firstId, _secondId;
};

bool Topology::operator < (const Topology& topo) const
{
    int tof = topo.getFirstId();
    int tos = topo.getSecondId();
    if(tof == _firstId) {
        return tos < _secondId;
    } else {
        return tof < _firstId;
    }
}

class Segment
{
public:
    Segment() : _startPt(), _endPt() {}
    Segment(const Point& startPt, const Point& endPt) : _startPt(startPt), _endPt(endPt) {}
    ~Segment() {}
    const Point& getStartPt() const {return _startPt;}
    const Point& getEndPt() const {return _endPt;}
    void setStartPt(const Point& startPt) {_startPt = startPt;}
    void setEndPt(const Point& endPt) {_endPt = endPt;}
  
protected:
    Point _startPt, _endPt;
};

class Element : public Segment
{
public:
    Element() : Segment(), _topo() {}
    Element(const Point& startPt, const Point& endPt, const Topology& topo) : Segment(startPt, endPt), _topo(topo) {}
    ~Element() {}
    
    const Topology& getTopology() const {return _topo;}
    void setTopology(const Topology& topo) {_topo = topo;}
    void setTopology(const int firstId, const int secondId) {_topo = Topology(firstId, secondId);}
    
private:
    Topology _topo;
};

void generatePolygons(vector<Polygon>& region)
{
    Polygon poly;
    double left, right, bot, top;
    double x, y, z = 0;
    Point p;
    
    poly.clear();
    p = Point(0, 0, z);
    poly.append(p);
    p = Point(8, 0, z);
    poly.append(p);
    p = Point(8, 1, z);
    poly.append(p);
    p = Point(6, 1, z);
    poly.append(p);
    p = Point(5, 1, z);
    poly.append(p);
    p = Point(3, 1, z);
    poly.append(p);
    p = Point(2, 1, z);
    poly.append(p);
    p = Point(0, 1, z);
    poly.append(p);
    poly.setName("poly_1");
    region.push_back(poly);
    
    poly.clear();
    p = Point(0, 1, z);
    poly.append(p);
    p = Point(2, 1, z);
    poly.append(p);
    p = Point(2, 2, z);
    poly.append(p);
    p = Point(0, 2, z);
    poly.append(p);
    poly.setName("poly_2");
    region.push_back(poly);
    
    poly.clear();
    p = Point(3, 1, z);
    poly.append(p);
    p = Point(5, 1, z);
    poly.append(p);
    p = Point(5, 2, z);
    poly.append(p);
    p = Point(3, 2, z);
    poly.append(p);
    poly.setName("poly_3");
    region.push_back(poly);

    poly.clear();
    p = Point(6, 1, z);
    poly.append(p);
    p = Point(8, 1, z);
    poly.append(p);
    p = Point(8, 2, z);
    poly.append(p);
    p = Point(6, 2, z);
    poly.append(p);
    poly.setName("poly_4");
    region.push_back(poly);

    poly.clear();
    p = Point(0, 2, z);
    poly.append(p);
    p = Point(2, 2, z);
    poly.append(p);
    p = Point(3, 2, z);
    poly.append(p);
    p = Point(5, 2, z);
    poly.append(p);
    p = Point(6, 2, z);
    poly.append(p);
    p = Point(8, 2, z);
    poly.append(p);
    p = Point(8, 3, z);
    poly.append(p);
    p = Point(0, 3, z);
    poly.append(p);
    poly.setName("poly_5");
    region.push_back(poly);
}

int main(int argc, char **argv)
{
	clock_t start, finish;
	start = clock();

	gmsh::initialize();
    gmsh::option::setNumber("General.Terminal", 1);
    gmsh::model::add("demo");
    
    double lc = 0.8;
    vector<Polygon> region;
    generatePolygons(region);
    
    int dim = 1;
    int tag = 1;
    double x, y, z;
    vector<int> loop, curveloop, planeSurface;
    
    int ct_point = 1;
    int ct_line = 1;
    
    // record the points where the previous polygon was inserted
    map<Point, int> recPoints;
    // record the line where the previous polygon wsa inserted
    map<Topology, int> recLines;
    vector<vector<int> > domElelIds(region.size());
    for(int i = 0; i < region.size(); i++)
    {
        loop.clear();
        Polygon poly = region[i];
        const vector<Point>& vertex = poly.getVertex();
        for(int j = 0; j < vertex.size(); j++)
        {
            Point p = vertex[j];
            auto iter = recPoints.find(p);
            if(iter == recPoints.end())
            {
                x = p.getX();
                y = p.getY();
                z = p.getZ();
                gmsh::model::geo::addPoint(x, y, z, lc, ct_point);
                recPoints[p] = ct_point;
                loop.push_back(ct_point);
                ct_point++;
            } else {
                loop.push_back(iter->second);
            }
        }
        curveloop.clear();
        for(int j = 0; j < loop.size(); j++)
        {
            int l1 = loop[j];
            int l2 = j == loop.size()-1 ? loop[0] : loop[j+1];
            Topology topo(l1, l2);
            Topology topo2(l2, l1);
            auto iterl = recLines.find(topo);
            auto iterl2 = recLines.find(topo2);
            bool flag = (iterl == recLines.end()) && (iterl2 == recLines.end());
            if(flag)
            {
                gmsh::model::geo::addLine(l1, l2, ct_line);
                recLines[topo] = ct_line;
                recLines[topo2] = -ct_line;
                curveloop.push_back(ct_line);
                ct_line++;
            } else {
                if(iterl != recLines.end())
                    curveloop.push_back(iterl->second);
                else
                    curveloop.push_back(iterl2->second);
            }
        }
        planeSurface.push_back(gmsh::model::geo::addCurveLoop(curveloop));
        tag = gmsh::model::addPhysicalGroup(dim, curveloop);
        string name = "Polygon_" + to_string(i);
        gmsh::model::setPhysicalName(dim, tag, name);
        domElelIds[i] = curveloop;
    }
    
    gmsh::model::geo::addPlaneSurface(planeSurface);
    
    dim = 1;
    gmsh::model::addPhysicalGroup(dim, planeSurface);
    
    gmsh::model::geo::synchronize();
    
    gmsh::model::mesh::generate(1);

#if 1
    clock_t info_s, info_e;
    info_s = clock();
    // get Mesh information
    
    // get Nodes
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
    
    // get Elemetnts
    dim = 1;
    tag = -1;
    vector<int> elementTypes;
    vector<vector<size_t> > elemetTags, nodeTagss;
    gmsh::model::mesh::getElements(elementTypes, elemetTags, nodeTagss, dim, tag);
    cout<<"The number of elemnets: "<<nodeTagss[0].size()/2<<endl;
    Topology topo;
    Element ele;
    Point p1, p2;
    unsigned int id1, id2;
    for(int i = 0; i < nodeTagss[0].size(); i+=2)
    {
        id1 = nodeTagss[0][i] - 1;
        id2 = nodeTagss[0][i+1] - 1;
        p1 = nodes[id1];
        p2 = nodes[id2];
        ele = Element(p1, p2, topo);
    }
    
    // get polygon nodes id and it's coordinate information
    // coord is coordinate information (x1, y1, z1, .......)
    dim = 1;
    vector<size_t> bouNodesIds;
    // boundaryTag = tag = gmsh::model::addPhysicalGroup(dim, curveloop)
    // then we can get one polygon points id on edges
    int boundaryTag = 1;
    gmsh::model::mesh::getNodesForPhysicalGroup(dim, boundaryTag, bouNodesIds, coord);
    vector<unsigned int> boundaryNodesIds(bouNodesIds.size());
    // just because index from star 0
    for(int i = 0; i < bouNodesIds.size(); i++)
    {
        boundaryNodesIds[i] = bouNodesIds[i]-1;
        //cout<<boundaryNodesIds[i]<<endl;
    }
    
    // get polygon arround elemnt id
    // bcurveLoop is line's tag(id) of one polygon
    // then we can get elemnts id of one polygon
    vector<int> bcurveLoop = domElelIds[0];
    vector<int> boundaryElementsIds;
    dim = 1;
    int dim2 = 1;
    bool strict = true;
    double xx, yy, zz;
    vector<size_t> elemetTag;
    for(int i = 0; i < bcurveLoop.size(); i++)
    {
        tag = abs(bcurveLoop[i]);
        gmsh::model::mesh::getElements(elementTypes, elemetTags, nodeTagss, dim, tag);
        for(int k = 0; k < nodeTagss[0].size(); k+=2)
        {
            id1 = nodeTagss[0][k]-1;
            id2 = nodeTagss[0][k+1]-1;
            xx = (nodes[id1].getX() + nodes[id2].getX())/2.0;
            yy = (nodes[id1].getY() + nodes[id2].getY())/2.0;
            zz = (nodes[id1].getZ() + nodes[id2].getZ())/2.0;
            gmsh::model::mesh::getElementsByCoordinates(xx, yy, zz, elemetTag, dim2, strict);
            for(int kk= 0; kk < elemetTag.size(); kk++)
            {
                boundaryElementsIds.push_back(elemetTag[kk]-1);
            }
        }
        if(bcurveLoop[i] < 0)
        {
            reverse(boundaryElementsIds.begin(), boundaryElementsIds.end());
        }
    }
    info_e = clock();
#endif
    gmsh::write("demo.msh");
    gmsh::finalize();
    finish = clock();
    cout<<"The run time of get information: "<<(double)(info_e - info_s)/CLOCKS_PER_SEC<<"s"<<endl;
    cout<<"The run time is: "<<(double)(finish - start)/CLOCKS_PER_SEC<<"s"<<endl;
    return 0;
}
