#include <fstream>
#include <iostream>
#include <vector>
#include "opencv2/imgproc.hpp"
#include "h/net.hpp"
#include "h/neuron.hpp"
#include "h/training_data.hpp"

using namespace std;
using namespace cv;


void printVector(string label, vector<double> &v) {
  cout << label << " ";
  for (unsigned i = 0; i < v.size(); ++i) {
    cout << v[i] << " ";
  }
  cout << endl;
}

int main(int argc, char* argv[]) {
  if (argc < 4) {
    cerr << "Usage: " << argv[0] << "[toplogy (i.e. 3600 16 16 3)]" << endl;
    return 1;
  }

  TrainingData trainData("sample/screen.peach");
  vector<unsigned> topology;
  for (int i = 0; i < argc; ++i) { // apparently ++i is better
    topology.push_back(atoi(argv[i]));
  }

  // trainData.getTopology(topology);
  Net myNet(topology);

  vector<double> inputVals, targetVals, resultVals;
  int trainingPass = 0;
  while (!trainData.isEof()) {
    ++trainingPass;
    cout << endl << "Pass" << trainingPass;

    if (trainData.getNextInputs(inputVals) != topology[0]) break;
    myNet.feedForward(inputVals);

    myNet.getResults(resultVals);
    printVector("Output:", resultVals);

    trainData.getTargetOutputs(targetVals);
    assert(targetVals.size() == topology.back());

    myNet.backProp(targetVals);
  }

  cout << endl << "Done" << endl;
}
