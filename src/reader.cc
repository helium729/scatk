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
    in = fopen(path.c_str(), "r");
    this->mode = mode;
}

reader::~reader()
{
}

bool reader::read(std::vector<f64>& buffer, u64 point_count, u64 trace_count)
{
    file->open(path);
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
    file->close();  
    if (counter < trace_count)
        return false;
    return true;
    
}

bool reader::transpose(u64 point_count, u64 trace_count, std::string output_file, u64 point_length)
{
    // open output file
    // if file exists, overwrite
    // else create new file
    FILE* out = fopen(output_file.c_str(), "w");
    if (out == NULL)
    {
        std::cerr << "Could not open file " << output_file << std::endl;
        return false;
    }
    // switch to C style file handling
    FILE* in = fopen(path.c_str(), "r");
    if (in == NULL)
    {
        std::cerr << "Could not open file " << path << std::endl;
        return false;
    }
    char* buffer = new char[point_length];
    // read file discretely
    for (u64 i = 0; i < point_count; i++)
    {
        for (u64 j = 0; j < trace_count; j++)
        {
            // seek to point
            fseek(in, (j * point_count + i) * (point_length+1), SEEK_SET);
            // read point
            u64 r = fread(buffer, sizeof(char), point_length, in);
            if (r != point_length)
            {
                std::cerr << "Error reading file " << path << std::endl;
                return false;
            }
            // write point
            fwrite(buffer, sizeof(char), point_length, out);
            // write space or newline
            if (j == trace_count - 1)
                fwrite("\n", sizeof(char), 1, out);
            else
                fwrite(" ", sizeof(char), 1, out);
        }
        printf("%llu/%llu \r", i+1, point_count);
        fflush(stdout);
    }
    printf("\n");
    // close files
    fclose(in);
    fclose(out);
    return true;
}

bool reader::readline(std::vector<f64>& buffer, u64 line_number, u64 point_count)
{
    if (in == NULL)
    {
        std::cerr << "Could not open file " << path << std::endl;
        return false;
    }
    char* line = new char[point_count * (3 + 1)];
    if (line_number != last_line + 1)
    {
        fseek(in, line_number * (point_count * (3 + 1)), SEEK_SET);
        last_line = line_number;
    }
    else
        last_line++;
    u64 r = fread(line, sizeof(char), point_count * (3 + 1), in);
    if (r != point_count * (3 + 1))
    {
        std::cerr << "Error reading file " << path << std::endl;
        return false;
    }
    std::string line_str(line);
    std::vector<std::string> tokens;
    std::istringstream iss(line_str);
    for (std::string s; iss >> s;)
        tokens.push_back(s);
    tokens.resize(point_count);
    // for each token in line
    u64 index = 0;
    for (std::string token : tokens)
    {
        u64 temp;
        switch (mode)
        {
            case HEX:
                // read hex value from tokens
                temp = std::stoul(token, 0, 16);
                // convert to double
                buffer[index] = (f64)temp;
                break;
            case DEC:
                temp = std::stoul(token, 0, 10);
                buffer[index] = (f64)temp;
            case BIN:
                std::cerr << "not implemented yet" << std::endl;
                break;
        }
        index++;
    }
    delete[] line;
    return true;
}

bool reader::readline(std::vector<f64>& buffer, u64 line_number, u64 point_count, u64 trace_count)
{
    
    if (in == NULL)
    {
        std::cerr << "Could not open file " << path << std::endl;
        return false;
    }
    char* line = new char[trace_count * (3 + 1)];
    if (line_number != last_line + 1)
    {
        fseek(in, line_number * (trace_count * (3 + 1)), SEEK_SET);
        last_line = line_number;
    }
    else
        last_line++;
    u64 r = fread(line, sizeof(char), trace_count * (3 + 1), in);
    if (r != trace_count * (3 + 1))
    {
        std::cerr << "Error reading file " << path << std::endl;
        return false;
    }
    std::string line_str(line);
    std::vector<std::string> tokens;
    std::istringstream iss(line_str);
    for (std::string s; iss >> s;)
        tokens.push_back(s);
    tokens.resize(trace_count);
    // for each token in line
    u64 index = 0;
    for (std::string token : tokens)
    {
        u64 temp;
        switch (mode)
        {
            case HEX:
                // read hex value from tokens
                temp = std::stoul(token, 0, 16);
                // convert to double
                buffer[index] = (f64)temp;
                break;
            case DEC:
                temp = std::stoul(token, 0, 10);
                buffer[index] = (f64)temp;
            case BIN:
                std::cerr << "not implemented yet" << std::endl;
                break;
        }
        index++;
    }
    delete[] line;
    return true;
}

void reader::close()
{
    fclose(in);
}

