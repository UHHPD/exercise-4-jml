#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <cmath>
#include "Data.hh"

// generic function comparing two values of some type T used later for int and
// double
template <class T>
bool testEqual(const std::string& name, T expected, T real) {
  if (expected != real) {
    std::cout << "(" << name << ": <" << expected << ">"
              << ", " << real << ") ";
    return false;
  }
  return true;
}

bool testReadingSize() {
  std::cout << "testReadingSize...";
  Data datA("testA");
  return testEqual<int>("size", 1, datA.size());
}

bool testReadingMeasurement() {
  std::cout << "testReadingMeasurement...";
  Data datA("testA");
  return testEqual("measurement", 10., datA.measurement(0));
}

bool testReadingBinEdges() {
  std::cout << "testReadingBinEdges...";
  Data datA("testA");
  return testEqual("bin low", 0., datA.binLow(0)) &&
         testEqual("bin high", 1., datA.binHigh(0));
}

bool testReadingErrors() {
  std::cout << "testReadingErrors...";
  Data datA("testA");
  return testEqual("error", 2., datA.error(0));
}

bool testCopyConstructor() {
  std::cout << "testCopyConstructor...";
  Data a("testA");
  Data b(a);
  Data c = a;  // equivalent to Data c(a)
  return testEqual("measurement", 10., b.measurement(0)) &&
         testEqual("measurement", 10., c.measurement(0));
}

void runTests() {
  std::cout << "running tests...\n";
  std::vector<std::function<bool()> > tests(
      {testReadingSize, testReadingMeasurement, testReadingBinEdges,
       testReadingErrors, testCopyConstructor});
  for (auto test : tests)
    std::cout << (test() ? " ok" : " FAILED!") << std::endl;
}


double background(double x) {
    const double alpha = 0.005;
    const double beta  = -0.00001;
    const double gamma = 0.08;
    const double delta = 0.015;

    return alpha + beta * x + gamma * std::exp(-delta * x);
}

// int main() {
//   using namespace std;

//   cout << "******************************************************" << endl;
//   runTests();
//   cout << "******************************************************" << endl;
//   // create an object which holds data of experiment A
//   // Data datA("exp_A");

//   // // here is the data from experiment A
//   // cout << "bin 27: from " << datA.binLow(27) << " to " << datA.binHigh(27)
//   //      << endl;
//   // cout << "measurement of experiment A in bin 27: " << datA.measurement(27)
//   //      << endl;
//   // cout << "uncertainty of experiment A in bin 27: " << datA.error(27)
//   //      << endl;


//   // Load all four experiments into a vector<Data>
//   vector<Data> experiments;
//   experiments.emplace_back("exp_A");
//   experiments.emplace_back("exp_B");
//   experiments.emplace_back("exp_C");
//   experiments.emplace_back("exp_D");
    
//   // Print the cross section (measurement) in bin 27 for each experiment
//   // for (size_t i = 0; i < experiments.size(); ++i) {
//   //   cout << "Experiment " << char('A' + i)
//   // 	 << " bin 27 measurement: "
//   // 	 << experiments[i].measurement(27)
//   // 	 << endl;
//   // }
  
//   int n = 2;
//   int nBadAB = experiments[0].checkCompatibility(experiments[1], n);
//   cout << "A vs B: " << nBadAB
//        << " bins differ by more than " << n << " sigma\n";
  

//   return 0;
// }

// runTests() and Data are assumed to be declared above / in headers
int nCheck = 2;
int main() {
    using namespace std;

    cout << "******************************************************" << endl;
    runTests();
    cout << "******************************************************" << endl;

    // --- read in all four data sets into a vector<Data> ---
    vector<Data> experiments;
    experiments.emplace_back("exp_A");
    experiments.emplace_back("exp_B");
    experiments.emplace_back("exp_C");
    experiments.emplace_back("exp_D");

    const int bin = 27;

    // --- print bin information and cross sections in bin 27 ---
    cout << "Bin " << bin << " edges (from exp_A): ["
         << experiments[0].binLow(bin) << ", "
         << experiments[0].binHigh(bin) << "]" << endl;

    for (size_t i = 0; i < experiments.size(); ++i) {
        double y  = experiments[i].measurement(bin);
        double ey = experiments[i].error(bin);

        cout << "Experiment " << char('A' + i)
             << " bin " << bin
             << " measurement: " << y
             << " ± " << ey << endl;
    }

    cout << "------------------------------------------------------" << endl;

    // --- weighted average of A,B,C,D in bin 27 (exercise e) ---
    int nCheck = 2;
    Data avgAB   = experiments[0].averageWith(experiments[1], nCheck);
    Data avgABC  = avgAB.averageWith(experiments[2], nCheck);
    Data avgABCD = avgABC.averageWith(experiments[3], nCheck);

    cout << "\nWeighted Average of A,B,C,D in bin " << bin << ":\n"
         << "  y_avg = " << avgABCD.measurement(bin)
         << " ± "      << avgABCD.error(bin) << endl;

    cout << "------------------------------------------------------" << endl;

    // --- Exercise 2: chi2/ndf for background hypothesis ---
    for (size_t i = 0; i < experiments.size(); ++i) {
        double chi2ndf = experiments[i].chi2ndfBackground();
        cout << "Experiment " << char('A' + i)
             << " chi2/ndf (background) = " << chi2ndf << endl;
    }

    return 0;
}
