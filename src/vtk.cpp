#include "vtk.h"
#include <filesystem>

void VTKWriter::add(const shared_ptr<Shape2D> &shape, const string &name) {
  shapes2D[name].push_back(shape);
}

void VTKWriter::add(const Ray2D &ray, const string &name) {
  rays2D[name].push_back(ray);
}

void VTKWriter::add(const vector<shared_ptr<Shape2D>> &shapes,
                    const string &name) {
  shapes2D[name].insert(shapes2D[name].end(), shapes.begin(), shapes.end());
}

void VTKWriter::add(const vector<Ray2D> &rays, const string &name,
                    unsigned int previewEveryNth) {
  for (unsigned int i = 0; i < rays.size(); i += previewEveryNth) {
    rays2D[name].push_back(rays[i]);
  }
}

void VTKWriter::addAsComposition(const vector<vector<Ray2D>> &raySequence,
                                 const string &name,
                                 unsigned int previewEveryNth) {
  for (unsigned int i = 0; i < raySequence.size(); ++i) {
    add(raySequence[i], name, previewEveryNth);
  }
}

void VTKWriter::addAsSequence(const vector<vector<Ray2D>> &raySequence,
                              const string &name,
                              unsigned int previewEveryNth) {
  for (unsigned int i = 0; i < raySequence.size(); ++i) {
    add(raySequence[i], name + "_" + to_string(i), previewEveryNth);
  }
}

void VTKWriter::add(const Object2D &object, const string &name) {
  add(object.getShapes(), name);
}

void VTKWriter::add(const vector<Object2D> &objects, const string &name) {
  for (auto &object : objects)
    add(object, name);
}

void VTKWriter::add(const shared_ptr<Object2D> &object, const string &name) {
  add(object->getShapes(), name);
}

void VTKWriter::add(const vector<shared_ptr<Object2D>> &objects,
                    const string &name) {
  for (auto &object : objects)
    add(object, name);
}

void VTKWriter::add(const shared_ptr<Grid2D> &grid, const string &name) {
  grids2D[name] = grid;
}

void VTKWriter::write() const {

  if (!filesystem::exists(outputDirectory + "/")) {
    filesystem::create_directories(outputDirectory + "/");

    if (!filesystem::exists(outputDirectory + "/")) {
      cerr << "Could not create directory " << outputDirectory << endl;
      return;
    }
  }

  for (const auto &p : shapes2D) {
    string filename = p.first + ".vtk";
    const auto &shapes = p.second;

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

    unsigned int nPoints = 0;
    for (auto &shape : shapes) {
      nPoints += shape->lineRepresentation().size() * 2;
    }

    fs << "POINTS " << nPoints << " float" << endl;
    for (auto &shape : shapes) {
      vector<vec4> lines = shape->lineRepresentation();
      for (auto &line : lines) {
        fs << line.x << " " << line.y << " 0.0" << endl;
        fs << line.z << " " << line.w << " 0.0" << endl;
      }
    }

    unsigned int nLines = nPoints / 2;

    fs << "LINES " << nLines << " " << 3 * nLines << endl;
    for (unsigned int idx = 0; idx < nPoints; idx += 2) {
      fs << "2 " << idx << " " << idx + 1 << endl;
    }

    fs.close();
  }

  for (const auto &p : rays2D) {
    string filename = p.first + ".vtk";
    const auto &rays = p.second;

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
      if (r.terminated) {
        fs << r.origin.x + r.terminatedAt * r.direction.x << " "
           << r.origin.y + r.terminatedAt * r.direction.y << " 0.0" << endl;
      } else {
        fs << r.origin.x + 10.0f * r.direction.x << " "
           << r.origin.y + 10.0f * r.direction.y << " 0.0" << endl;
      }
    }
    fs << "LINES " << rays.size() << " " << 3 * rays.size() << endl;
    for (unsigned int idx = 0; idx < 2 * rays.size(); idx += 2) {
      fs << "2 " << idx << " " << idx + 1 << endl;
    }

    fs.close();
  }

  for (const auto &p : grids2D) {
    string filename = p.first + ".vtk";
    auto grid = p.second;

    fstream fs(outputDirectory + "/" + filename,
               fstream::in | fstream::out | fstream::trunc);

    if (!fs.is_open()) {
      cerr << "Could not open " << outputDirectory + "/" + filename << endl;
      return;
    }

    fs << "# vtk DataFile Version 2.0" << endl;
    fs << filename << endl;
    fs << "ASCII" << endl;
    fs << "DATASET STRUCTURED_POINTS" << endl;
    fs << "DIMENSIONS " << grid->maxX << " " << grid->maxY << " " << 1 << endl;
    fs << "ORIGIN " << grid->cornerMin.x << " " << grid->cornerMin.y << " "
       << 0.0f << endl;
    fs << "SPACING "
       << (grid->cornerMax.x - grid->cornerMin.x) / (grid->maxX - 1) << " "
       << (grid->cornerMax.y - grid->cornerMin.y) / (grid->maxY - 1) << " "
       << 0.0f << endl;
    fs << "POINT_DATA " << grid->data.size() << endl;
    fs << "SCALARS values float 1" << endl;
    fs << "LOOKUP_TABLE default" << endl;

    for (auto it = grid->data.begin(); it != grid->data.end(); it++) {
      fs << *it << endl;
    }

    fs.close();
  }
}
