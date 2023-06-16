#include "reader.h"

using namespace scatk;

reader::reader(std::string path, Mode mode)
{
    this->path = path;
    file = new std::ifstream(path);
    // check if file exists
    if (!file->good())
    {
        std::cerr << "File " << path << " does not exist" << std::endl;
        exit(1);
    }
    // open file
    file->close();
    file->open(path);
    this->mode = mode;
}

reader::~reader()
{
}

bool reader::read(std::vector<f64>& buffer, u64 size)
{
    u64 counter = 0;
    
    for (std::string line; std::getline(*file, line);)
    {
        //split line with spaces
        std::vector<std::string> tokens;
        std::istringstream iss(line);
        for (std::string s; iss >> s;)
            tokens.push_back(s);
        // check if line is empty
        if (tokens.size() == 0)
            continue;

        u64 temp;
        if (counter >= size)
            return false;

        // for each token in line
        for (std::string token : tokens)
        {
            switch (mode)
            {
                case HEX:
                    // read hex value from tokens
                    temp = std::stoul(token, 0, 16);
                    // convert to double
                    buffer.push_back((f64)temp);
                    break;
                case DEC:
                    temp = std::stoul(token, 0, 10);
                    buffer.push_back((f64)temp);
                    break;
                case BIN:
                    std::cerr << "not implemented yet" << std::endl;
                    break;
            }
            counter++;
        }
    }
    return true;
}

bool reader::read(std::vector<f64>& buffer, u64 point_count, u64 trace_count)
{
    u64 counter = 0;
    u64 index = 0;
    for (std::string line; !(file->eof());)
    {
        std::getline(*file, line);
        if (counter >= trace_count)
            break;
        //split line with spaces
        std::vector<std::string> tokens;
        std::istringstream iss(line);
        for (std::string s; iss >> s;)
            tokens.push_back(s);
        // only keep the points we need defined by point_count
        tokens.resize(point_count);

        u64 temp;
        f64 tempd;
        // for each token in line
        for (std::string token : tokens)
        {
            switch (mode)
            {
                case HEX:
                    // read hex value from tokens
                    temp = std::stoul(token, 0, 16);
                    // convert to double
                    tempd = (f64)temp;
                    buffer.at(index) = tempd;
                    break;
                case DEC:
                    temp = std::stoul(token, 0, 10);
                    tempd = (f64)temp;
                    buffer.at(index) = tempd;
                case BIN:
                    std::cerr << "not implemented yet" << std::endl;
                    break;
            }
            index++;
        }
        counter++;
    }
    if (counter < trace_count)
        return false;
    return true;
    
}

void reader::close()
{
    file->close();
}

