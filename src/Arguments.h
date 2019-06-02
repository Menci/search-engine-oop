#ifndef _MENCI_SEARCH_ENGINE_ARGUMENTS_H
#define _MENCI_SEARCH_ENGINE_ARGUMENTS_H

#include <string>

struct Arguments {
    std::string stopWordsFile;
    std::string indexDirectory;
};

Arguments parseArguments(int argc, char *argv[]);

#endif // _MENCI_SEARCH_ENGINE_ARGUMENTS_H
