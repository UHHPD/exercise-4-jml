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
Data Data::averageWith(const Data& other, int nCheck) const {
  // same number of bins?
  if (size() != other.size() || m_bins.size() != other.m_bins.size()) {
    throw std::runtime_error("averageWith: datasets have different sizes");
  }

  // check that bin edges match (within tiny tolerance)
  for (unsigned int i = 0; i < m_bins.size(); ++i) {
    if (std::fabs(m_bins[i] - other.m_bins[i]) > 1e-9) {
      throw std::runtime_error("averageWith: bin edges differ between datasets");
    }
  }


  int nBad = checkCompatibility(other, nCheck);
  if (nBad > 0) {
    std::cerr << "Warning: " << nBad
              << " bins differ by more than "
              << nCheck << " sigma; averaging anyway.\n";
  }

  // prepare output vectors
  std::vector<double> outBins   = m_bins;           // same binning
  std::vector<double> outData(size());
  std::vector<double> outError(size());

  for (unsigned int i = 0; i < size(); ++i) {
    double y1 = measurement(i);
    double e1 = error(i);
    double y2 = other.measurement(i);
    double e2 = other.error(i);

    // weights w_i = 1 / sigma_i^2
    double w1 = 1.0 / (e1 * e1);
    double w2 = 1.0 / (e2 * e2);
    double wsum = w1 + w2;

    // weighted average
    double yavg = (w1 * y1 + w2 * y2) / wsum;
    double eavg = std::sqrt(1.0 / wsum);

    outData[i]  = yavg;
    outError[i] = eavg;
  }

  // use the vector-based constructor
  return Data(outBins, outData, outError);
}
