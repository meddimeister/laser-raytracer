#include "csv.h"
#include <filesystem>

void CSVWriter::write() const {

  if (!filesystem::exists(outputDirectory + "/")) {
    filesystem::create_directories(outputDirectory + "/");

    if (!filesystem::exists(outputDirectory + "/")) {
      cerr << "Could not create directory " << outputDirectory << endl;
      return;
    }
  }

  for (const auto &t : tables) {
    string filename = t.first + ".csv";
    const auto &lines = t.second;

    fstream fs(outputDirectory + "/" + filename,
               fstream::in | fstream::out | fstream::trunc);

    if (!fs.is_open()) {
      cerr << "Could not open " << outputDirectory + "/" + filename << endl;
      return;
    }

    for (auto &line : lines) {
      fs << line << endl;
    }

    fs.close();
  }
}
