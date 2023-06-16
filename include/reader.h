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

        bool read(std::vector<f64>& buffer, u64 size);
        bool read(std::vector<f64>& buffer, u64 point_count, u64 trace_count);

        void close();

    private:
        std::ifstream* file;
        std::string path;
        Mode mode;
    };
}


#endif // SCATK_READER_H