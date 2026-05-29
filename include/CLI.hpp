#pragma once

class CLI {
public:
    int run(int argc, char* argv[]) const;

private:
    static void printUsage();
};
