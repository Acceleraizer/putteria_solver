#include "putteria.h"

int main(int argv, char* argc[]) {
    if (argv != 2) {
        std::cout << "incorrect usage\n";
        return 1;
    }

    std::string infilename = argc[1];

    putteria_puzzle pz(infilename);
    pz.print_layout();
    pz.print_values();
    std::cerr << pz.solve() << "!\n";
    std::cerr << '\n';
    pz.print_values();
}