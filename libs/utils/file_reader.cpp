#include "file_reader.hpp"

#include <fstream>

namespace ocv{

std::vector<std::string> read_file_per_lines(const std::string &fname)
{
    std::vector<std::string> names;
    std::ifstream in("assets/obj_detect/coco.names");
    std::string line;
    while(std::getline(in, line)){
        names.emplace_back(std::move(line));
    }

    return names;
}



}
