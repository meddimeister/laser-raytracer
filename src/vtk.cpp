#include "vtk.h"
#include <filesystem>

void VTKWriter::add(const Line2D &line) { lines2D.push_back(line); }

void VTKWriter::add(const Ray2D &ray) { rays2D.push_back(ray); }

void VTKWriter::add(const vector<Line2D> &lines) {
  lines2D.insert(lines2D.end(), lines.begin(), lines.end());
}

void VTKWriter::add(const vector<Ray2D> &rays) {
  rays2D.insert(rays2D.end(), rays.begin(), rays.end());
}

void VTKWriter::write(const string &outputDirectory) {

  if (filesystem::create_directories(outputDirectory + "/")) {
    cerr << "Could not create directory " << outputDirectory << endl;
    return;
  }

  fstream lines2DFile(outputDirectory + "/lines2D.vtk",
                      fstream::in | fstream::out | fstream::trunc);

  if (!lines2DFile.is_open()) {
    cerr << "Could not open " << outputDirectory + "/lines2D.vtk" << endl;
    return;
  }

  lines2DFile << "# vtk DataFile Version 2.0" << endl;
  lines2DFile << "Lines2D" << endl;
  lines2DFile << "ASCII" << endl;
  lines2DFile << "DATASET POLYDATA" << endl;

  unsigned int nPointsLines2D = 2 * lines2D.size();

  lines2DFile << "POINTS " << nPointsLines2D << " float" << endl;
  for (auto &l : lines2D) {
    lines2DFile << l.a.x << " " << l.a.y << " 0.0" << endl;
    lines2DFile << l.b.x << " " << l.b.y << " 0.0" << endl;
  }
  lines2DFile << "LINES " << lines2D.size() << " " << 3 * lines2D.size()
              << endl;
  for (unsigned int idx = 0; idx < 2 * lines2D.size(); idx += 2) {
    lines2DFile << "2 " << idx << " " << idx + 1 << endl;
  }

  lines2DFile.close();

  fstream rays2DFile(outputDirectory + "/rays2D.vtk",
                     fstream::in | fstream::out | fstream::trunc);

  if (!rays2DFile.is_open()) {
    cerr << "Could not open " << outputDirectory + "/rays2D.vtk" << endl;
    return;
  }

  rays2DFile << "# vtk DataFile Version 2.0" << endl;
  rays2DFile << "Rays2D" << endl;
  rays2DFile << "ASCII" << endl;
  rays2DFile << "DATASET POLYDATA" << endl;

  unsigned int nPointsRays2D = 2 * rays2D.size();

  rays2DFile << "POINTS " << nPointsRays2D << " float" << endl;
  for (auto &r : rays2D) {
    rays2DFile << r.origin.x << " " << r.origin.y << " 0.0" << endl;
		if(r.hit){
    rays2DFile << r.origin.x + r.t * r.direction.x << " "
               << r.origin.y + r.t * r.direction.y << " 0.0" << endl;
  	}
		else{
    rays2DFile << r.origin.x + r.direction.x << " "
               << r.origin.y + r.direction.y << " 0.0" << endl;
		}
	}
  rays2DFile << "LINES " << rays2D.size() << " " << 3 * rays2D.size() << endl;
  for (unsigned int idx = 0; idx < 2 * rays2D.size(); idx += 2) {
    rays2DFile << "2 " << idx << " " << idx + 1 << endl;
  }

  rays2DFile.close();
}
