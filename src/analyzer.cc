#include "analyzer.h"

using namespace scatk;

void scatk::t_test(const std::vector<f64>& x, const std::vector<f64>& y, std::promise<f64>&& p)
{
    // get vector from buffer
    std::vector<scatk::f64> vec1 = x;
    std::vector<scatk::f64> vec2 = y;
    // calculate mean without eigen
    scatk::f64 mean1 = std::accumulate(vec1.begin(), vec1.end(), 0.0) / vec1.size();
    scatk::f64 mean2 = std::accumulate(vec2.begin(), vec2.end(), 0.0) / vec2.size();
    // square each element
    scatk::f64 mean_squared1 = mean1 * mean1;
    scatk::f64 mean_squared2 = mean2 * mean2;
    // square each element of traces
    std::vector<scatk::f64> vec_squared1(vec1.size());
    std::vector<scatk::f64> vec_squared2(vec2.size());
    std::transform(vec1.begin(), vec1.end(), vec_squared1.begin(), [](scatk::f64 x) { return x * x; });
    std::transform(vec2.begin(), vec2.end(), vec_squared2.begin(), [](scatk::f64 x) { return x * x; });
    // for each column, subtract mean_squared
    std::vector<scatk::f64> vec_squared_minus_mean_squared1(vec_squared1.size());
    std::vector<scatk::f64> vec_squared_minus_mean_squared2(vec_squared2.size());
    std::transform(vec_squared1.begin(), vec_squared1.end(), vec_squared_minus_mean_squared1.begin(), [mean_squared1](scatk::f64 x) { return x - mean_squared1; });
    std::transform(vec_squared2.begin(), vec_squared2.end(), vec_squared_minus_mean_squared2.begin(), [mean_squared2](scatk::f64 x) { return x - mean_squared2; });
    // calculate sample variance
    scatk::f64 var1 = std::accumulate(vec_squared_minus_mean_squared1.begin(), vec_squared_minus_mean_squared1.end(), 0.0) / (x.size() - 1);
    scatk::f64 var2 = std::accumulate(vec_squared_minus_mean_squared2.begin(), vec_squared_minus_mean_squared2.end(), 0.0) / (y.size() - 1);
    // calculate t-statistic
    scatk::f64 t = (mean1 - mean2) / std::sqrt(var1 / x.size() + var2 / y.size());
    // return t-statistic
    p.set_value(t);
}

//calculate correlation coefficient
void scatk::corr(const std::vector<f64>& x, const std::vector<f64>& y, std::promise<f64>&& p)
{
    // get vector from buffer
    std::vector<scatk::f64> vec1 = x;
    std::vector<scatk::f64> vec2 = y;
    // calculate mean without eigen
    scatk::f64 mean1 = std::accumulate(vec1.begin(), vec1.end(), 0.0) / vec1.size();
    scatk::f64 mean2 = std::accumulate(vec2.begin(), vec2.end(), 0.0) / vec2.size();
    // square each element
    scatk::f64 mean_squared1 = mean1 * mean1;
    scatk::f64 mean_squared2 = mean2 * mean2;
    // square each element of traces
    std::vector<scatk::f64> vec_squared1(vec1.size());
    std::vector<scatk::f64> vec_squared2(vec2.size());
    std::transform(vec1.begin(), vec1.end(), vec_squared1.begin(), [](scatk::f64 x) { return x * x; });
    std::transform(vec2.begin(), vec2.end(), vec_squared2.begin(), [](scatk::f64 x) { return x * x; });
    // for each column, subtract mean_squared
    std::vector<scatk::f64> vec_squared_minus_mean_squared1(vec_squared1.size());
    std::vector<scatk::f64> vec_squared_minus_mean_squared2(vec_squared2.size());
    std::transform(vec_squared1.begin(), vec_squared1.end(), vec_squared_minus_mean_squared1.begin(), [mean_squared1](scatk::f64 x) { return x - mean_squared1; });
    std::transform(vec_squared2.begin(), vec_squared2.end(), vec_squared_minus_mean_squared2.begin(), [mean_squared2](scatk::f64 x) { return x - mean_squared2; });
    scatk::f64 var1 = std::accumulate(vec_squared_minus_mean_squared1.begin(), vec_squared_minus_mean_squared1.end(), 0.0) / (x.size());
    scatk::f64 var2 = std::accumulate(vec_squared_minus_mean_squared2.begin(), vec_squared_minus_mean_squared2.end(), 0.0) / (y.size());
    scatk::f64 cov = std::inner_product(vec1.begin(), vec1.end(), vec2.begin(), 0.0) / (x.size());
    // calculate correlation coefficient
    scatk::f64 corr = cov / std::sqrt(var1 * var2);
    // return correlation coefficient
    p.set_value(corr);
    return;
}
