#ifndef DATA_HH
#define DATA_HH

#include <vector>
#include <string>
#include <stdexcept>

class Data {
 public:
  Data(const std::string& filename);

  unsigned int size() const { return m_data.size(); }
  // double measurement(int i) const { return m_data[i]; }
  // double binCenter(int i) const { return 0; }
  // double binLow(int i) const { return 0; }
  // double binHigh(int i) const { return 0; }
  // double error(int i) const { return 0; }
  double measurement(int i) const { return m_data[i]; }

  double binLow(int i) const {
    if (i < 0 || static_cast<unsigned int>(i) >= m_data.size())
      throw std::out_of_range("binLow: index out of range");
    return m_bins[i];
  }

  double binHigh(int i) const {
    if (i < 0 || static_cast<unsigned int>(i) >= m_data.size())
      throw std::out_of_range("binHigh: index out of range");
    return m_bins[i + 1];
  }

  double binCenter(int i) const {
    return 0.5 * (binLow(i) + binHigh(i));
  }

  int checkCompatibility(const Data& in, int n) const;

  // double error(int i) const {
  //   return 0.0;

  double error(int i) const { return m_errors[i]; }
  
 private:
  Data() {}  // disallow empty data set
  void assertSizes();
  // std::vector<double> m_data;
  // std::vector<double> m_bins;

  std::vector<double> m_data;   // bin contents
  std::vector<double> m_bins;   // bin edges (size+1)
  std::vector<double> m_errors; // bin errors (size)


};

#endif
