#include <fstream>
#include <iostream>
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

using namespace std;
using namespace cv;

int main() {
  namedWindow("viewdata", WINDOW_KEEPRATIO);
  vector<uchar> trainingData;
  ifstream data;
  data.open("sample/screen.peach", ios::binary | ios::in);  // default read-only

  int size;
  data.read((char*)&size, sizeof(int));
  while (size > 0) {
    cout << "Reading session of size: " << size << "\n";
    trainingData.resize(size);
    data.read((char*)&trainingData[0], size * sizeof(uchar));

    // hopefully read all data here
    for (int i = 0; i < size; i += 2700) {
      vector<uchar>::const_iterator first = trainingData.begin() + i;
      vector<uchar>::const_iterator last = trainingData.begin() + i + 2700;
      vector<uchar> newVector(first, last);
      Mat sceen(45, 60, CV_8UC1, newVector.data());
      namedWindow("viewdata", WINDOW_KEEPRATIO);
      imshow("viewdata", sceen);
      waitKey(500);
    }
    size = 0;
    trainingData.clear();
    data.read((char*)&size, sizeof(int));
  }
  data.close();
}