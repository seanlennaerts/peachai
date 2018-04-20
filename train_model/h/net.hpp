#include "neuron.hpp"

class Net {
 public:
  Net(const vector<unsigned> &topology);
  void feedForward(const vector<double> &inputVals);
  void backProp(const vector<double> &targetVals);
  void getResults(vector<double> &resultVals) const;
  double getRecentAverageError(void) const { return m_recentAverageError; }

 private:
  vector<vector<Neuron>> m_layers;
  double m_error;
  double m_recentAverageError;
  static double m_recentAverageSmoothingFactor;
};