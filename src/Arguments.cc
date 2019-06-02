#include "Arguments.h"

#include "ArgumentParser/ArgumentParser.h"

Arguments parseArguments(int argc, char *argv[]) {
    Arguments arguments;
    ArgumentParser(argc, argv)
        .setProgramDescription("A CLI search engine for HTML documents with TF-IDF scoring.")
        .addPositional("stopwords_file", "The file for stopwords, one word per line.",
                       ArgumentParser::stringParser(arguments.stopWordsFile))
        .addPositional("index_directory", "The directory for index database.",
                       ArgumentParser::stringParser(arguments.indexDirectory))
        .parse();
    return arguments;
}
