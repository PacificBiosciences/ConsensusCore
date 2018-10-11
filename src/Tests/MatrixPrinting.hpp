// Author: David Alexander

#include <iomanip>
#include <iostream>
#include <string>

template <typename M>
std::string PrintMatrix(const M& m, int precision = 2)
{
    if (m.Rows() > 40 || m.Columns() > 40) {
        return "Cowardly refusing to print a large matrix\n";
    } else {
        std::stringstream ss;
        for (int row = 0; row < m.Rows(); ++row) {
            for (int col = 0; col < m.Columns(); ++col) {
                ss << std::setw(8) << std::setprecision(precision) << std::noshowpoint << std::right
                   << m(row, col);
            }
            ss << std::endl;
        }
        return ss.str();
    }
}
