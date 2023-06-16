
#include "argparse/parser.h"

int main(int argc, char** argv)
{
    argparse::parser p;
    p.add_parameter("h", "help", "Print this help message", argparse::NONE, false, "");
    p.add_parameter("o", "output", "Output file", argparse::STRING, true, "");
    p.add_parameter("w", "wave", "Wave file to read", argparse::STRING, true, "");

    bool parse_result = p.parse(argc, argv);
    if (!parse_result)
    {    std::cout << "Error parsing arguments" << std::endl;
        return 1;
    }

    bool help;
    p.get_parameter_value_to("--help", &help);
    if (help || argc == 1)
    {    std::string help_message = p.get_help_message();
        std::cout << help_message << std::endl;
    }

    std::string wave_file;
    p.get_parameter_value_to("--wave", &wave_file);
    std::cout << "Wave file: " << wave_file << std::endl;

    


    return 0;
}