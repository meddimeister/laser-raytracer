#include "vtk.h"
#include <filesystem>

void VTKWriter::add(const Line2D &line, const string &name) { 
	lines2D[name].push_back(line); 
}

void VTKWriter::add(const Ray2D &ray, const string &name) {
	rays2D[name].push_back(ray);
}

void VTKWriter::add(const vector<Line2D> &lines, const string &name) {
  lines2D[name].insert(lines2D[name].end(), lines.begin(), lines.end());
}

void VTKWriter::add(const vector<Ray2D> &rays, const string &name) {
  rays2D[name].insert(rays2D[name].end(), rays.begin(), rays.end());
}

void VTKWriter::write() {

  if (!filesystem::create_directories(outputDirectory + "/")) {
    cerr << "Could not create directory " << outputDirectory << endl;
    return;
  }

	for(auto &p : lines2D){
		string filename = p.first + ".vtk";
		vector<Line2D> &lines = p.second;

  	fstream fs(outputDirectory + "/" + filename,
  	                    fstream::in | fstream::out | fstream::trunc);

  	if (!fs.is_open()) {
  	  cerr << "Could not open " << outputDirectory + "/" + filename << endl;
  	  return;
  	}

  	fs << "# vtk DataFile Version 2.0" << endl;
  	fs << filename << endl;
  	fs << "ASCII" << endl;
  	fs << "DATASET POLYDATA" << endl;

  	unsigned int nPoints = 2 * lines.size();

  	fs << "POINTS " << nPoints << " float" << endl;
  	for (auto &l : lines) {
  	  fs << l.a.x << " " << l.a.y << " 0.0" << endl;
  	  fs << l.b.x << " " << l.b.y << " 0.0" << endl;
  	}
  	fs << "LINES " << lines.size() << " " << 3 * lines.size()
  	            << endl;
  	for (unsigned int idx = 0; idx < 2 * lines.size(); idx += 2) {
  	  fs << "2 " << idx << " " << idx + 1 << endl;
  	}

  	fs.close();
	}

	for(auto &p : rays2D){
		string filename = p.first + ".vtk";
		vector<Ray2D> &rays = p.second;

  	fstream fs(outputDirectory + "/" + filename,
  	                    fstream::in | fstream::out | fstream::trunc);

  	if (!fs.is_open()) {
  	  cerr << "Could not open " << outputDirectory + "/" + filename << endl;
  	  return;
  	}

  	fs << "# vtk DataFile Version 2.0" << endl;
  	fs << filename << endl;
  	fs << "ASCII" << endl;
  	fs << "DATASET POLYDATA" << endl;

  	unsigned int nPoints = 2 * rays.size();

  	fs << "POINTS " << nPoints << " float" << endl;
  	for (auto &r : rays) {
    	fs << r.origin.x << " " << r.origin.y << " 0.0" << endl;
			if(r.hit){
    	fs << r.origin.x + r.t * r.direction.x << " "
    	           << r.origin.y + r.t * r.direction.y << " 0.0" << endl;
  		}
			else{
    	fs << r.origin.x + 1000.0f * r.direction.x << " "
    	           << r.origin.y + 1000.0f * r.direction.y << " 0.0" << endl;
			}
  	}
  	fs << "LINES " << rays.size() << " " << 3 * rays.size()
  	            << endl;
  	for (unsigned int idx = 0; idx < 2 * rays.size(); idx += 2) {
  	  fs << "2 " << idx << " " << idx + 1 << endl;
  	}

  	fs.close();
	}
}
