// qalgorithms_utils.h
#ifndef QALGORITHMS_UTILS_H // Include guarde to prevent double inclusion
#define QALGORITHMS_UTILS_H

// internal
#include "qalgorithms_matrix.h"

// external
#include <vector>
#include <iostream>
#include <cmath>
#include <map>

namespace q
{

  int sum(const std::vector<int> &vec);
  size_t sum(const std::vector<size_t> &vec);
  double sum(const std::vector<double> &vec);
  int sum(const bool *vec, size_t n);

  /**
   * @brief Fast exponential approximation base on Bit Manipulation.
   * @details This function is a fast approximation of the exponential function. It is based on the bit manipulation of the double value. The function is not as accurate as the standard exponential function but is much faster. It is based on the following repository: https://github.com/edrosten/fasterexp
   * 
   * @param x : value between 0 and 26
   * @return double 
   */
  double exp_approx(const double x);

  template <typename T>
  std::vector<bool> operator<(
      const std::vector<T> &vec,
      T scalar);

  template <typename T>
  std::vector<bool> operator>(
      const std::vector<T> &vec,
      T scalar);

  template <typename T>
  std::vector<T> operator*(
      const std::vector<T> &A,
      const std::vector<T> &B);

  std::vector<bool> operator&&(
      const std::vector<bool> &A,
      const std::vector<bool> &B);

  std::vector<bool> operator!(const std::vector<bool> &A);

  void operator|=(
      std::vector<bool> &A,
      const std::vector<bool> &B);

  double erfi(const double x);


  class tValues
  {
  private:
    std::map<int, double> values;

  public:
    tValues();
    const double &operator[](size_t degreeOfFreedom) const;
  };

  const Matrix linreg(
      const std::vector<double> &xData,
      const std::vector<double> &yData,
      const int degree);

  class ProgressBar
  {
  private:
    int total;
    int width;
    std::vector<std::string> colors;

  public:
    ProgressBar(int total, int width = 50);
    void update(int current);
    void complete();
  };

  extern const double tValuesArray[500];
  extern const double chiSquareArray[500];
  extern const double SQRTPI_2;
} // namespace q
#endif // QALGORITHMS_UTILS_H