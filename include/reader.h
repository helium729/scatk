#ifndef SCATK_READER_H
#define SCATK_READER_H

#include "defs.h"

#include <iostream>
#include <fstream>
#include <string>


namespace scatk
{
    class reader
    {
    public:
        enum Mode
        {
            HEX, DEC, BIN
        };
        reader(std::string path, Mode mode = HEX);
        ~reader();

        bool read(std::vector<f64>& buffer, u64 point_count, u64 trace_count);
        bool transpose(u64 point_count, u64 trace_count, std::string output_file, u64 point_length = 3);
        bool readline(std::vector<f64>& buffer, u64 line_number, u64 point_count, u64 trace_count);

        void close();

    private:
        std::ifstream* file;
        FILE* in;
        std::string path;
        Mode mode;
        u64 last_line;
    };
}


#endif // SCATK_READER_H