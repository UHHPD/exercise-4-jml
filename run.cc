#include <iostream>
#include <vector>
#include <functional>
#include <string>

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
#include <iostream>
#include <vector>
#include <cmath>    // fabs, sqrt

// runTests() and Data are assumed to be declared above / in headers

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

    // --- part 1: explicit compatibility check in bin 27 (A vs B) ---
    double yA  = experiments[0].measurement(bin);
    double eA  = experiments[0].error(bin);
    double yB  = experiments[1].measurement(bin);
    double eB  = experiments[1].error(bin);

    double diffAB       = std::fabs(yA - yB);
    double sigmaDeltaAB = std::sqrt(eA * eA + eB * eB);
    double pullsAB      = diffAB / sigmaDeltaAB;

    int nCheck = 2;  // nσ threshold to use (e.g. 2σ)

    cout << "A vs B in bin " << bin << ":\n"
         << "  |Δy| = " << diffAB
         << ", σ_Δy = " << sigmaDeltaAB
         << ", |Δy|/σ_Δy = " << pullsAB << endl;

    if (diffAB < nCheck * sigmaDeltaAB)
        cout << "  -> compatible within " << nCheck << "σ" << endl;
    else
        cout << "  -> NOT compatible within " << nCheck << "σ" << endl;

    cout << "------------------------------------------------------" << endl;

    // --- part 2: global compatibility using Data::checkCompatibility ---
    int n = 2;  // number of σ for the global check

    int nBadAB = experiments[0].checkCompatibility(experiments[1], n);
    int nBadAC = experiments[0].checkCompatibility(experiments[2], n);
    int nBadAD = experiments[0].checkCompatibility(experiments[3], n);
    int nBadBC = experiments[1].checkCompatibility(experiments[2], n);
    int nBadBD = experiments[1].checkCompatibility(experiments[3], n);
    int nBadCD = experiments[2].checkCompatibility(experiments[3], n);

    cout << "Number of bins differing by more than " << n << "σ:" << endl;
    cout << "  A vs B: " << nBadAB << endl;
    cout << "  A vs C: " << nBadAC << endl;
    cout << "  A vs D: " << nBadAD << endl;
    cout << "  B vs C: " << nBadBC << endl;
    cout << "  B vs D: " << nBadBD << endl;
    cout << "  C vs D: " << nBadCD << endl;

    return 0;
}
