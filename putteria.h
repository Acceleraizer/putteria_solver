#ifndef PUTTERIA
#define PUTTERIA

#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>


struct cell {
    int r, c;
    int value = 0;
    struct cluster* owner = nullptr;

    cell(int r, int c) {
        this->r = r;
        this->c = c;
    }
};

struct cluster {
    int id;
    std::vector<cell*> members = {};
    cell* selected = nullptr;

    cluster(int i) {
        id = i;    
    }
};

struct putteria_puzzle {
    // each pair is (group_no, value)
    int R, C;
    std::vector<std::vector<cell*>> board_state;
    std::vector<cluster*> clusters;
    std::vector<std::vector<std::vector<std::pair<int, int>>>> graph;

    std::vector<std::vector<bool>> rowcheck, colcheck;

    putteria_puzzle(std::string file);
    ~putteria_puzzle();

    void dfs(int r, int c);
    void print_layout();
    void print_values();

    int depth = 0;
    int maxsz;
    bool solve();
    bool check_adj_unocc(int r, int c);
} ;

#endif