#ifndef MATH_HPP
#define MATH_HPP

#include <array>
#include <algorithm>
#include <iostream>

namespace math 
{
  template <int R, int C>
  class Matrix
  {
  public:
    void put(int row, int col, float value) {
      data[(C*row)+col] = value;
    }
    float get(int row, int col) {
      return data[(C*row)+col];
    }

    void clear() {
      std::fill(data.begin(), data.end(), 0);
    }
  private:
    std::array<float, R * C> data;
  };

  // Identity generator
  template <int X>
  void setIdentity(Matrix<X, X>& m) {
    m.clear();
    for (int i = 0; i < X; i++)
      m.put(i, i, 1);
  }
};


// Operator overloads
using namespace math;

template <int R, int C, int R1, int C1>
Matrix<R, C1> operator*(Matrix<R, C>& m1, Matrix<R1, C1>& m2)
{
  static_assert(C == R1, "Cannot multiply matrices");
  Matrix<R, C1> result; 

  // for each row in the first matrix
  for (int r = 0; r < R; r++)
  {
    // for each column in the second matrix
    for (int c = 0; c < C1; c++)
    {
      // Multiply element-wise and summate
      int total = 0;
      for (int i = 0; i < R1; i++)
      {
        total +=
          (m1.get(r, i) * m2.get(i, c));
      }
      result.put(r, c, total);
    }
  }

  return result;
}

template <int R, int C>
std::ostream& operator<<(std::ostream& os, Matrix<R, C>& m)
{
  os << "Matrix (" << R << "x" << C << ") [\n";

  for (int r = 0; r < R; r++)
  {
    os << "  ";
    for (int c = 0; c < C; c++)
      os << m.get(r, c) << " ";
    os << '\n';
  }
  os << "]";

  return os;
}

#endif
