
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
    p.add_parameter("p", "plot", "Thing to plot", argparse::STRING, false, "none");
    p.add_parameter("r", "reference", "Reference file", argparse::STRING, false, "");
    p.add_parameter("", "no-gui", "Disable GUI", argparse::NONE, false, "");


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

    std::string plot_what;
    p.get_parameter_value_to("--plot", &plot_what);
    std::cout << "Plot what: " << plot_what << std::endl;
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
    else if (plot_what == "var")
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


    return 0;
}