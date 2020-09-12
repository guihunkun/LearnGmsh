#include <iostream>
#include <gmsh.h>
#include <vector> 
#include <fstream>
#include <iomanip>
using namespace std;


void writeNodes(vector<double>& nodes, string s)
{
	string filename = s + ".txt";
	ofstream outfileb(filename.c_str(), ios::out);
	for(int i = 0; i < nodes.size(); i++)
	{
		outfileb<<(nodes[i])<<endl;
	}
	outfileb.close();
}


void writeElements(vector<std::vector<std::size_t> >& elements, string s)
{
	string filename = s + ".txt";
	ofstream outfileb(filename.c_str(), ios::out);
	for(int i = 0; i < elements[1].size(); i++)
	{
		outfileb<<(elements[1][i])<<endl;
	}
	outfileb.close();
}


int main(int argc, char **argv)
{
	// Before using any functions in the C++ API, Gmsh must be initialized.
	gmsh::initialize();
	// By default Gmsh will not print out any messages: in order to output
	// messages on the terminal, just set the standard Gmsh option
	// "General.Terminal" (same format and meaning as in .geo files) using
	// gmsh::option::setNumber():
	//gmsh::option::setNumber("General.Terminal", 1);

	// This adds a new model, named "t1". If gmsh::model::add() is not called, a
	// new default (unnamed) model will be created on the fly, if necessary.
	gmsh::model::add("Test1");
	// The C++ API provides direct access to the internal CAD kernels. The
    // built-in CAD kernel was used in t1.geo: the corresponding API functions
    // live in the "gmsh::model::geo" namespace. To create geometrical points with
    // the built-in CAD kernel, one thus uses gmsh::model::geo::addPoint():
    //
    // - the first 3 arguments are the point coordinates (x, y, z)
    //
    // - the next (optional) argument is the target mesh size close to the point
    //
    // - the last (optional) argument is the point tag



  	double lcc = 0.05;//2e-1;
  	gmsh::model::geo::addPoint(1.0/3, 1.0/3,  0, lcc, 100);
  	gmsh::model::geo::addPoint(2.0/3, 1.0/3,  0, lcc, 101);
  	gmsh::model::geo::addPoint(2.0/3, 2.0/3,  0, lcc, 102);
  	gmsh::model::geo::addPoint(1.0/3, 2.0/3,  0, lcc, 103);
  	gmsh::model::geo::addLine(100, 101, 100);
  	gmsh::model::geo::addLine(101, 102, 101);
  	gmsh::model::geo::addLine(102, 103, 102);
  	gmsh::model::geo::addLine(103, 100, 103);
  	gmsh::model::geo::addCurveLoop({100, 101, 102, 103}, 100);
  	//gmsh::model::geo::addPlaneSurface({100}, 100);



  	double lc = 0.2;//2e-1;
  	gmsh::model::geo::addPoint(-1, 0,  0, lc, 1);
  	gmsh::model::geo::addPoint(1,  0,  0, lc, 2);
  	gmsh::model::geo::addPoint(1,  1,  0, lc, 3);
  	gmsh::model::geo::addPoint(-1, 1,  0, lc, 4);
  	gmsh::model::geo::addPoint(0,  0,  0, lc, 555);


  	// The API to create lines with the built-in kernel follows the same
  	// conventions: the first 2 arguments are point tags, the last (optional one)
  	// is the line tag.
  	gmsh::model::geo::addLine(1, 555, 1);
  	gmsh::model::geo::addLine(555, 2, 555);
  	gmsh::model::geo::addLine(2, 3, 2);
  	gmsh::model::geo::addLine(3, 4, 3);
  	gmsh::model::geo::addLine(4, 1, 4);

  	// The philosophy to construct curve loops and surfaces is similar: the first
  	// argument is now a vector of integers.
  	gmsh::model::geo::addCurveLoop({1, 555, 2, 3, 4}, 1);
  	gmsh::model::geo::addPlaneSurface({1, -100}, 1);


  	gmsh::model::geo::addPoint(1,  2,  0, lc, 5);
  	gmsh::model::geo::addPoint(-1, 2,  0, lc, 6);
  	gmsh::model::geo::addLine(3, 5, 5);
  	gmsh::model::geo::addLine(5, 6, 6);
  	gmsh::model::geo::addLine(6, 4, 7);
  	gmsh::model::geo::addCurveLoop({-3, 5, 6, 7}, 2);
	gmsh::model::geo::addPlaneSurface({2}, 2);




  	// Physical groups are defined by providing the dimension of the group (0 for
  	// physical points, 1 for physical curves, 2 for physical surfaces and 3 for
  	// phsyical volumes) followed by a vector of entity tags. The last (optional)
  	// argument is the tag of the new group to create.

	gmsh::model::setPhysicalName(2, 6, "My surface");

	// Before it can be meshed, the internal CAD representation must be
	// synchronized with the Gmsh model, which will create the relevant Gmsh data
	// structures. This is achieved by the gmsh::model::geo::synchronize() API
	// call for the built-in CAD kernel. Synchronizations can be called at any
	// time, but they involve a non trivial amount of processing; so while you
	// could synchronize the internal CAD data after every CAD command, it is
	// usually better to minimize the number of synchronization points.
	gmsh::model::geo::synchronize();

	// We can then generate a 2D mesh...
	gmsh::model::mesh::generate(2);
	//gmsh::model::mesh::refine();

	// get mesh information
	// Nodes
	vector<double> nodes,y;
	vector<std::size_t> nodeTags;
	gmsh::model::mesh::getNodes(nodeTags, nodes, y, -2, -1, false, true);
	cout<<"The number of nodes: "<<nodes.size()/3<<endl;
#if 0
	cout<<endl;
	for(int i = 0; i < nodes.size(); i++)
	{
		cout<<nodes[i]<<endl;
	}
#endif
	writeNodes(nodes, "nodes");

	//Elements
	vector<int> elementTypes;
	vector<std::vector<std::size_t> > elementTags, nodeTags2;
	gmsh::model::mesh::getElements(elementTypes, elementTags, nodeTags2, -2, -1);
	cout<<"The number of elements: "<<nodeTags2[1].size()/3<<endl;
#if 0
	cout<<endl;
	for(int j = 0; j < nodeTags2[1].size(); j++)
	{
		cout<<nodeTags2[1][j]<<endl;
		
	}
#endif
	writeElements(nodeTags2, "elements");

	// ... and save it to disk
	gmsh::write("demo.msh");

	// Remember that by default, if physical groups are defined, Gmsh will export
	// in the output mesh file only those elements that belong to at least one
	// physical group. To force Gmsh to save all elements, you can use
	//
	// gmsh::option::setNumber("Mesh.SaveAll", 1);

	// This should be called at the end:
	gmsh::finalize();
	return 0;
}
