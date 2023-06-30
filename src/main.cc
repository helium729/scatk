#include "argparse/parser.h"
#include "defs.h"
#include "reader.h"
#include "matplotlibcpp.h"
#include "analyzer.h"

namespace plt = matplotlibcpp;

int main(int argc, char** argv)
{
    argparse::parser p;
    p.add_parameter("h", "help", "Print this help message", argparse::NONE, false, "");
    p.add_parameter("o", "output", "Output file", argparse::STRING, false, "");
    p.add_parameter("t", "trace", "Trace file to read", argparse::STRING, true, "0");
    p.add_parameter("x", "point-count", "Number of points in one trace", argparse::INTEGER, true, "0"); // todo: change to false later
    p.add_parameter("y", "trace-count", "Number of traces to read", argparse::INTEGER, true, "0"); // todo: change to false later
    p.add_parameter("p", "operation", "Thing to do", argparse::STRING, false, "none");
    p.add_parameter("r", "reference", "Reference file", argparse::STRING, false, "");
    p.add_parameter("", "no-gui", "Disable GUI", argparse::NONE, false, "");
    p.add_parameter("", "transposed", "Wave is transposed", argparse::NONE, false, "");
    p.add_parameter("", "point-index", "Point index to analyze/index step for t-graph", argparse::INTEGER, false, "0");


    bool parse_result = p.parse(argc, argv);

    bool help;
    p.get_parameter_value_to("--help", &help);
    if (help || argc == 1)
    {    
        std::string help_message = p.get_help_message();
        std::cout << help_message << std::endl;
        exit(0);
    }

    if (!parse_result)
    {    
        std::cout << "Error parsing arguments" << std::endl;
        return 1;
    }

    std::string trace_file;
    p.get_parameter_value_to("--trace", &trace_file);
    std::cout << "Trace file: " << trace_file << std::endl;

    scatk::u64 point_count;
    p.get_parameter_value_to("--point-count", &point_count);
    std::cout << "Point count: " << point_count << std::endl;

    scatk::u64 trace_count;
    p.get_parameter_value_to("--trace-count", &trace_count);
    std::cout << "Trace count: " << trace_count << std::endl;

    std::string output_file;
    p.get_parameter_value_to("--output", &output_file);
    if (output_file != "")
        std::cout << "Output file: " << output_file << std::endl;

    std::string reference_file;
    p.get_parameter_value_to("--reference", &reference_file);
    if (reference_file != "")
        std::cout << "Reference file: " << reference_file << std::endl;

    bool no_gui;
    p.get_parameter_value_to("--no-gui", &no_gui);
    std::cout << "No GUI: " << no_gui << std::endl;

    bool transposed;
    p.get_parameter_value_to("--transposed", &transposed);
    std::cout << "Transposed: " << transposed << std::endl;

    scatk::u64 point_index;
    p.get_parameter_value_to("--point-index", &point_index);
    if (point_index != 0)
        std::cout << "Point index: " << point_index << std::endl;


    std::string plot_what;
    p.get_parameter_value_to("-p", &plot_what);
    std::cout << "Thing to do: " << plot_what << std::endl;
    
    if (plot_what == "demo")
    {
        scatk::reader r(trace_file, scatk::reader::Mode::HEX);
        std::vector<scatk::f64> buffer(point_count * 1);
        r.read(buffer, point_count, 1);
        r.close();
        if (!no_gui)
        {
            plt::plot(buffer);
            plt::show();
        }
        if (output_file != "")
        {
            std::ofstream out(output_file);
            for (scatk::f64 value : buffer)
            {
                out << value << std::endl;
            }
            out.close();
        }
    }
    else if (plot_what == "mean")
    {
        if (!transposed)
        {
            std::vector<scatk::f64> result(point_count, 0);
            scatk::reader r(trace_file, scatk::reader::Mode::HEX);
            std::vector<scatk::f64> buffer(point_count);
            for (scatk::u64 i = 0; i < trace_count; i++)
            {
                r.readline(buffer, i, point_count);
                // add each element to result
                std::transform(result.begin(), result.end(), buffer.begin(), result.begin(), std::plus<scatk::f64>());
            }
            r.close();
            // divide each element by trace_count
            std::transform(result.begin(), result.end(), result.begin(), [trace_count](scatk::f64 x) { return x / trace_count; });
            if (!no_gui)
            {
                plt::plot(result);
                plt::show();
            }
        }
        else // transposed
        {
            std::cerr << "Mean is not supposed to be calculated with transposed wave." << std::endl;           
        }
    }
    else if (plot_what == "var")
    {
        std::vector<scatk::f64> result(point_count);
        scatk::reader r(trace_file, scatk::reader::Mode::HEX);
        if (!transposed)
        {
            std::cerr << "Too many points to plot variance\nPlease transpose the wave." << std::endl;
            return 1;
        }
        std::vector<scatk::f64> buffer(trace_count);
        for (scatk::u64 i = 0; i < point_count; i++)
        {
            r.readline(buffer, i, point_count, trace_count);
            // get vector from buffer
            std::vector<scatk::f64> vec = buffer;
            // calculate mean
            scatk::f64 mean = std::accumulate(vec.begin(), vec.end(), 0.0) / vec.size();
            scatk::f64 mean_squared = mean * mean;
            // square each element of traces
            std::transform(vec.begin(), vec.end(), vec.begin(), [](scatk::f64 x) { return x * x; });
            // for each column, subtract mean_squared
            std::transform(vec.begin(), vec.end(), vec.begin(), [mean_squared](scatk::f64 x) { return x - mean_squared; });
            // calculate sample variance
            result[i] = std::accumulate(vec.begin(), vec.end(), 0.0) / (trace_count - 1);
        }
        r.close();
        if (!no_gui)
        {
            plt::plot(result);
            plt::show();
        }
        if (output_file != "")
        {
            std::ofstream out(output_file);
            for (scatk::f64 value : result)
            {
                out << value << std::endl;
            }
            out.close();
        }
    }
    else if (plot_what == "transpose")
    {
        if (output_file == "")
        {
            std::cerr << "Please specify output file" << std::endl;
            return 1;
        }
        scatk::reader r(trace_file, scatk::reader::Mode::HEX);
        r.transpose(point_count, trace_count, output_file);
    }
    else if (plot_what == "t-test")
    {
        if (!transposed)
        {
            std::cerr << "Please transpose the wave" << std::endl;
            return 1;
        }
        scatk::reader r1(trace_file, scatk::reader::Mode::HEX);
        scatk::reader r2(reference_file, scatk::reader::Mode::HEX);
        std::vector<scatk::f64> buffer1(trace_count);
        std::vector<scatk::f64> buffer2(trace_count);
        std::vector<scatk::f64> result(point_count);
        // thread pool
        std::vector<std::thread> threads(THREAD_COUNT);
        std::vector<scatk::i64> thread_status(THREAD_COUNT, -1);
        std::vector<std::promise<scatk::f64>> promises(THREAD_COUNT);
        // initialize promises
        for (scatk::u64 i = 0; i < THREAD_COUNT; i++)
        {
            promises.at(i) = std::promise<scatk::f64>();
        }
        std::vector<std::future<scatk::f64>> futures(THREAD_COUNT);
        for (scatk::u64 i = 0; i < point_count; i++)
        {
            scatk::u64 j = 0;
            r1.readline(buffer1, i, point_count, trace_count);
            r2.readline(buffer2, i, point_count, trace_count);
            // check if thread is done
            while (1)
            {
                j = j % THREAD_COUNT;
                if (thread_status.at(j) == -1)
                {
                    // start thread
                    futures.at(j) = promises.at(j).get_future();
                    std::thread t(scatk::t_test, buffer1, buffer2, std::move(promises.at(j)));
                    threads.at(j) = std::move(t);
                    thread_status.at(j) = i;
                    printf("Start thread %lld for line %lld\n", j, i + 1);
                    fflush(stdout);
                    break;
                }
                else
                {
                    // check if thread is done, if not continue
                    if (futures.at(j).wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        // get result
                        result.at(thread_status.at(j)) = futures.at(j).get();
                        // if joinable, join
                        if (threads.at(j).joinable())
                            threads.at(j).join();
                        // set thread status to -1
                        thread_status.at(j) = -1;
                        // new promise
                        promises.at(j) = std::promise<scatk::f64>();
                    }
                    j++;
                }
            }
        }
        // wait for all threads to finish
        for (scatk::u64 j = 0; j < THREAD_COUNT; j++)
        {
            if (thread_status.at(j) != -1)
            {
                // get result
                result.at(thread_status.at(j)) = futures.at(j).get();
                // if joinable, join
                if (threads.at(j).joinable())
                    threads.at(j).join();
                // set thread status to -1
                thread_status.at(j) = -1;
            }
        }
        printf("\n");
        r1.close();
        r2.close();
        if (!no_gui)
        {
            plt::plot(result);
            plt::show();
        }
        if (output_file != "")
        {
            std::ofstream out(output_file);
            for (scatk::f64 value : result)
            {
                out << value << std::endl;
            }
            out.close();
        }

    }
    else if (plot_what == "point_dist")
    {
        if (!transposed)
        {
            std::cerr << "Please transpose the wave" << std::endl;
            return 1;
        }
        scatk::reader r(trace_file, scatk::reader::Mode::HEX);
        std::vector<scatk::f64> buffer(trace_count);
        std::vector<scatk::u64> result(1024);
        r.readline(buffer, point_index, point_count, trace_count);
        for (scatk::f64 value : buffer)
        {
            scatk::u64 index = (scatk::u64)(value * 1024);
            if (index < 1024)
            {
                result.at(index)++;
            }
        }
        r.close();
        if (!no_gui)
        {
            plt::plot(result);
            plt::show();
        }
    }
    else if (plot_what == "t-graph")
    {
        if (!transposed)
        {
            std::cerr << "Please transpose the wave" << std::endl;
            return 1;
        }
        scatk::reader r1(trace_file, scatk::reader::Mode::HEX);
        scatk::reader r2(reference_file, scatk::reader::Mode::HEX);
        // create a matrix of t-statistics
        std::vector<std::vector<scatk::f64>> t_matrix(point_count, std::vector<scatk::f64>(trace_count / point_index));
        for (scatk::u64 i = 0; i < point_count; i++)
        {
            std::vector<scatk::f64> buffer1(trace_count);
            std::vector<scatk::f64> buffer2(trace_count);
            r1.readline(buffer1, i, point_count, trace_count);
            r2.readline(buffer2, i, point_count, trace_count);
            // thread pool
            std::vector<std::thread> threads(THREAD_COUNT);
            std::vector<scatk::i64> thread_status(THREAD_COUNT, -1);
            std::vector<std::promise<scatk::f64>> promises(THREAD_COUNT);
            // initialize promises
            for (scatk::u64 j = 0; j < THREAD_COUNT; j++)
            {
                promises.at(j) = std::promise<scatk::f64>();
            }
            // future vector
            std::vector<std::future<scatk::f64>> futures(THREAD_COUNT);
            for (scatk::u64 j = point_index; j <= trace_count; j += point_index)
            {   
                // calculate t-statistic for each step in parallel
                for (scatk::u64 k = 0; 1; k++)
                {
                    k = k % THREAD_COUNT;
                    if (thread_status.at(k) == -1)
                    {
                        // start thread
                        futures.at(k) = promises.at(k).get_future();
                        std::vector<scatk::f64> vec1(j);
                        std::vector<scatk::f64> vec2(j);
                        std::copy(buffer1.begin(), buffer1.begin() + j, vec1.data());
                        std::copy(buffer2.begin(), buffer2.begin() + j, vec2.data());
                        std::thread t(scatk::t_test, vec1, vec2, std::move(promises.at(k)));                        
                        threads.at(k) = std::move(t);
                        thread_status.at(k) = j;
                        break;
                    }
                    else
                    {
                        // check if thread is done, if not continue
                        if (futures.at(k).wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                            // get result
                            t_matrix.at(i).at(thread_status.at(k) / point_index - 1) = futures.at(k).get();
                            // if joinable, join
                            if (threads.at(k).joinable())
                                threads.at(k).join();
                            // set thread status to -1
                            thread_status.at(k) = -1;
                            // new promise
                            promises.at(k) = std::promise<scatk::f64>();
                        }
                    }
                } 
            }
            // wait for all threads to finish
            for (scatk::u64 k = 0; k < THREAD_COUNT; k++)
            {
                if (thread_status.at(k) != -1)
                {
                    // get result
                    t_matrix.at(i).at(thread_status.at(k) / point_index - 1) = std::abs(futures.at(k).get());
                    // if joinable, join
                    if (threads.at(k).joinable())
                        threads.at(k).join();
                    // set thread status to -1
                    thread_status.at(k) = -1;
                }
            }
            // print progress
            printf("\r%lld/%lld", (scatk::u64)(i + 1), point_count);
            fflush(stdout);
        }
        printf("\n");
        r1.close();
        r2.close();

        // plot t-statistic matrix
        // get max t for each step
        std::vector<scatk::f64> max_t(trace_count / point_index);
        for (scatk::u64 i = 0; i < trace_count / point_index; i++)
        {
            scatk::f64 max = 0;
            for (scatk::u64 j = 0; j < point_count; j++)
            {
                if (t_matrix.at(j).at(i) > max)
                    max = t_matrix.at(j).at(i);
            }
            max_t.at(i) = max;
        }
        if (!no_gui)
        {
            plt::plot(max_t);
            plt::show();
        }
        // write max_t to file
        if (output_file != "")
        {
            std::ofstream out(output_file);
            for (scatk::f64 value : max_t)
            {
                out << value << std::endl;
            }
            out.close();
        }

    }
    else if (plot_what == "corr_single")
    {
        // transpose wave is required
        if (!transposed)
        {
            std::cerr << "Please transpose the wave" << std::endl;
            return 1;
        }
        scatk::reader r1(trace_file, scatk::reader::Mode::HEX);
        std::vector<scatk::f64> buffer1(trace_count);
        // middle value of trace_count
        scatk::reader r2(reference_file, scatk::reader::Mode::HEX);
        std::vector<scatk::f64> buffer2(trace_count);
        r2.readline(buffer2, 0, 1, trace_count);
        // now buffer2 is the estimated power
        std::vector<scatk::f64> result(point_count);
        // thread pool
        std::vector<std::thread> threads(THREAD_COUNT);
        std::vector<scatk::i64> thread_status(THREAD_COUNT, -1);
        std::vector<std::promise<scatk::f64>> promises(THREAD_COUNT);
        // initialize promises
        for (scatk::u64 i = 0; i < THREAD_COUNT; i++)
        {
            promises.at(i) = std::promise<scatk::f64>();
        }
        std::vector<std::future<scatk::f64>> futures(THREAD_COUNT);
        for (scatk::u64 i = 0; i < point_count; i++)
        {
            scatk::u64 j = 0;
            r1.readline(buffer1, i, point_count, trace_count);
            // check if thread is done
            while (1)
            {
                j = j % THREAD_COUNT;
                if (thread_status.at(j) == -1)
                {
                    // start thread
                    futures.at(j) = promises.at(j).get_future();
                    std::thread t(scatk::corr, buffer1, buffer2, std::move(promises.at(j)));
                    threads.at(j) = std::move(t);
                    thread_status.at(j) = i;
                    printf("Start thread %lld for line %lld\n", j, i + 1);
                    fflush(stdout);
                    break;
                }
                else
                {
                    // check if thread is done, if not continue
                    if (futures.at(j).wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                    {
                        // get result
                        result.at(thread_status.at(j)) = futures.at(j).get();
                        // if joinable, join
                        if (threads.at(j).joinable())
                            threads.at(j).join();
                        // set thread status to -1
                        thread_status.at(j) = -1;
                        // new promise
                        promises.at(j) = std::promise<scatk::f64>();
                    }
                    j++;
                }
            }
        }
        // wait for all threads to finish
        for (scatk::u64 j = 0; j < THREAD_COUNT; j++)
        {
            if (thread_status.at(j) != -1)
            {
                // get result
                result.at(thread_status.at(j)) = futures.at(j).get();
                // if joinable, join
                if (threads.at(j).joinable())
                    threads.at(j).join();
                // set thread status to -1
                thread_status.at(j) = -1;
            }
        }
        printf("\n");
        r1.close();
        r2.close();
        if (!no_gui)
        {
            plt::plot(result);
            plt::show();
        }
        if (output_file != "")
        {
            std::ofstream out(output_file);
            for (scatk::f64 value : result)
            {
                out << value << std::endl;
            }
            out.close();
        }
    }
    return 0;
}