#include "Data.hh"

#include <iostream>
#include <fstream>
#include <cassert>
#include <stdexcept>
#include <cmath>    // fabs, sqrt

using namespace std;

Data::Data(const std::string& filename) {
  ifstream file(filename);

  // check if file opened correctly
  if (!file.is_open()) {
    throw runtime_error("Error could not open file with name: " + filename);
  }

  int size = -1;
  file >> size;

  // read in bin edges: size+1 numbers
  for (int i = 0; i < size + 1; ++i) {
    double edge;
    file >> edge;
    m_bins.push_back(edge);
  }

  // read in bin contents: size numbers
  for (int i = 0; i < size; ++i) {
    double entries;
    file >> entries;
    m_data.push_back(entries);
  }

  // read in bin errors: size numbers
  for (int i = 0; i < size; ++i) {
    double err;
    file >> err;
    m_errors.push_back(err);
  }

  // done! close the file
  file.close();

  assertSizes();
}

void Data::assertSizes() {
  assert(m_data.size() + 1 == m_bins.size());
  assert(m_errors.size() == m_data.size());
}

int Data::checkCompatibility(const Data& in, int n) const {
  if (size() != in.size()) {
    throw runtime_error("checkCompatibility: datasets have different sizes");
  }
  if (n <= 0) {
    throw runtime_error("checkCompatibility: n must be positive");
  }

  int nIncompatible = 0;

  for (unsigned int i = 0; i < size(); ++i) {
    double y1 = measurement(i);
    double e1 = error(i);
    double y2 = in.measurement(i);
    double e2 = in.error(i);

    double diff       = std::fabs(y1 - y2);
    double sigmaDelta = std::sqrt(e1 * e1 + e2 * e2);

    // if sigmaDelta is zero, any non-zero difference is incompatible
    if (sigmaDelta <= 0.0) {
      if (diff > 0.0) ++nIncompatible;
    } else if (diff > n * sigmaDelta) {
      ++nIncompatible;
    }
  }

  return nIncompatible;
}
