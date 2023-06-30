#ifndef SCATK_ANALYZER_H
#define SCATK_ANALYZER_H

#include "defs.h"
#include <vector>
#include <numeric>
#include <algorithm>
#include <functional>
#include <map>
#include <array>
#include <cstdint>
#include <cmath>

namespace scatk 
{
    void t_test(const std::vector<f64>& x, const std::vector<f64>& y, std::promise<f64>&& p);
    void corr(const std::vector<f64>& x, const std::vector<f64>& y, std::promise<f64>&& p);
} // namespace scatk

#endif