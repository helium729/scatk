
#include "argparse/parser.h"
#include "defs.h"
#include "reader.h"
#include "matplotlibcpp.h"

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
    std::cout << "Output file: " << output_file << std::endl;

    bool no_gui;
    p.get_parameter_value_to("--no-gui", &no_gui);
    std::cout << "No GUI: " << no_gui << std::endl;

    bool transposed;
    p.get_parameter_value_to("--transposed", &transposed);
    std::cout << "Transposed: " << transposed << std::endl;

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
            scatk::reader r(trace_file, scatk::reader::Mode::HEX);
            std::vector<scatk::f64> buffer(point_count * trace_count);
            r.read(buffer, point_count, trace_count);
            r.close();
            // convert buffer to eigen matrix of traces
            Eigen::MatrixXd traces = Eigen::Map<Eigen::MatrixXd>(buffer.data(), point_count, trace_count);
            // drop buffer to save memory
            buffer.resize(0);
            // calculate mean of each row
            Eigen::VectorXd mean = traces.rowwise().mean();
            // convert mean to vector
            std::vector<scatk::f64> mean_vec(mean.data(), mean.data() + mean.size());
            // plot mean
            if (!no_gui)
            {
                plt::plot(mean_vec);
                plt::show();
            }
            if (output_file != "")
            {
                std::ofstream out(output_file);
                for (scatk::f64 value : mean_vec)
                {
                    out << value << std::endl;
                }
                out.close();
            }
        }
        else // transposed
        {
            std::vector<scatk::f64> result(point_count);
            scatk::reader r(trace_file, scatk::reader::Mode::HEX);
            std::vector<scatk::f64> buffer(trace_count);

            for (scatk::u64 i = 0; i < point_count; i++)
            {
                r.readline(buffer, i, point_count, trace_count);
                // get eigen vector from buffer
                Eigen::VectorXd vec = Eigen::Map<Eigen::VectorXd>(buffer.data(), buffer.size());
                // calculate mean
                result.at(i) = vec.mean();
            }
            r.close();
            if (!no_gui)
            {
                plt::plot(result);
                plt::show();
            }            
        }
    }
    else if (plot_what == "var")
    {
        if (!transposed)
        {
            if (point_count * trace_count > (scatk::u64)1E8)
            {
                std::cerr << "Too many points to plot variance\nPlease transpose the wave." << std::endl;
                return 1;
            }
            scatk::reader r(trace_file, scatk::reader::Mode::HEX);
            std::vector<scatk::f64> buffer(point_count * trace_count);
            r.read(buffer, point_count, trace_count);
            r.close();
            // convert buffer to eigen matrix of traces
            Eigen::MatrixXd traces = Eigen::Map<Eigen::MatrixXd>(buffer.data(), point_count, trace_count);
            // drop buffer to save memory
            buffer.resize(0);
            // calculate mean of each row
            Eigen::VectorXd mean = traces.rowwise().mean();
            // square each element
            Eigen::VectorXd mean_squared = mean.array().square();
            // square each element of traces
            Eigen::MatrixXd traces_squared = traces.array().square();
            // drop traces to save memory
            traces.resize(0, 0);
            // for each column, subtract mean_squared
            Eigen::MatrixXd traces_squared_minus_mean_squared = traces_squared.colwise() - mean_squared;
            // drop traces_squared to save memory
            traces_squared.resize(0, 0);
            // calculate sample variance
            Eigen::VectorXd var = traces_squared_minus_mean_squared.rowwise().sum() / (trace_count - 1);
            // drop traces_squared_minus_mean_squared to save memory
            traces_squared_minus_mean_squared.resize(0, 0);
            // plot var
            std::vector<scatk::f64> var_vec(var.data(), var.data() + var.size());
            if (!no_gui)
            {
                plt::plot(var_vec);
                plt::show();
            }        
            if (output_file != "")
            {
                std::ofstream out(output_file);
                for (scatk::f64 value : var_vec)
                {
                    out << value << std::endl;
                }
                out.close();
            }
        }
        else //transposed
        {
            std::vector<scatk::f64> result(point_count);
            scatk::reader r(trace_file, scatk::reader::Mode::HEX);
            std::vector<scatk::f64> buffer(trace_count);

            for (scatk::u64 i = 0; i < point_count; i++)
            {
                r.readline(buffer, i, point_count, trace_count);
                // get eigen vector from buffer
                Eigen::VectorXd vec = Eigen::Map<Eigen::VectorXd>(buffer.data(), buffer.size());
                // calculate mean
                Eigen::VectorXd mean = vec.mean() * Eigen::VectorXd::Ones(vec.size());
                // square each element
                Eigen::VectorXd mean_squared = mean.array().square();
                // drop mean to save memory
                mean.resize(0);
                // square each element of traces
                Eigen::VectorXd vec_squared = vec.array().square();
                // drop vec to save memory
                vec.resize(0);
                // for each column, subtract mean_squared
                Eigen::VectorXd vec_squared_minus_mean_squared = vec_squared - mean_squared;
                // drop vec_squared and mean_squared to save memory
                vec_squared.resize(0);
                mean_squared.resize(0);
                // calculate sample variance
                result.at(i) = vec_squared_minus_mean_squared.sum() / (trace_count - 1);
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


    return 0;
}