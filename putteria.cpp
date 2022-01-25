#include "putteria.h"
#include <math.h>


std::pair<int, int> NBDS[4] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};


putteria_puzzle::putteria_puzzle(std::string file) {
    std::ifstream inf(file);

    // first two lines are:
    //     pzprv3
    //     putteria
    std::string s1, s2;
    inf >> s1 >> s2 >> R >> C;


    graph = std::vector<std::vector<std::vector<std::pair<int, int>>>>(R, std::vector<std::vector<std::pair<int, int>>>(C));
    board_state = std::vector<std::vector<cell*>>(R, std::vector<cell*>(C));
    for (int r=0; r<R; ++r) {
        for (int c=0; c<C; ++c) {
            board_state[r][c] = new cell(r, c);
        }
    }

    clusters = {};

    // next block has dimension R x (C-1), shows whether the vertical wall is present or not
    bool vwall;
    for (int r=0; r<R; ++r) {
        for (int c=0; c<C-1; ++c) {
            inf >> vwall;
            if (!vwall) {
                graph[r][c].push_back({r,c+1});
                graph[r][c+1].push_back({r,c});
            }
        }
    }
    std::cout << "vwall complete\n";
    // next block is analogous for horizontal walls
    bool hwall;
    for (int r=0; r<R-1; ++r) {
        for (int c=0; c<C; ++c) {
            inf >> hwall;
            if (!hwall) {
                graph[r][c].push_back({r+1,c});
                graph[r+1][c].push_back({r,c});
            }
        }
    }
    std::cout << "hwall complete\n";

    // we can now construct the graph
    for (int r=0; r<R; ++r) {
        for (int c=0; c<C; ++c) {
            if (board_state[r][c]->owner) continue;
            else {
                int nxt = clusters.size();
                clusters.push_back(new cluster(nxt));
                dfs(r, c);
            }
        }
    }

    // next block contains the starting values.
    // '.' = empty
    // '-' = unplaceable
    // else it is a starting number
    std::string v;
    for (int r=0; r<R; ++r) {
        for (int c=0; c<C; ++c) {
            inf >> v;
            if (v == ".") continue;
            else if (v == "-") board_state[r][c]->value = -1;
            else {
                board_state[r][c]->value = stoi(v);
                board_state[r][c]->owner->selected = board_state[r][c];
            }
        }
    }

    //initialize row/col checks
    maxsz = 0;
    for (cluster* cl: clusters) {
        maxsz = std::max(maxsz, (int)cl->members.size());
    }
    rowcheck = std::vector<std::vector<bool>>(R, std::vector<bool>(maxsz,false));
    colcheck = std::vector<std::vector<bool>>(C, std::vector<bool>(maxsz,false));

    for (int r=0; r<R; ++r) {
        for (int c=0; c<C; ++c) {
            int v = board_state[r][c]->value;
            if (v > 0) {
                rowcheck[r][v-1] = true;
                colcheck[c][v-1] = true;
            }
        }
    }

}


putteria_puzzle::~putteria_puzzle() {
    for (auto r: board_state) for (cell* c: r) delete c;
    for (cluster* cl: clusters) delete cl;
}


void putteria_puzzle::dfs(int r, int c) {
    //do some dfs.
    if (board_state[r][c]->owner) return;
    board_state[r][c]->owner = (clusters.back());
    clusters.back()->members.push_back(board_state[r][c]);

    if (graph[r][c].size()) for (auto p: graph[r][c]) {
        dfs(p.first, p.second);
    }
}


bool putteria_puzzle::solve() {
    if (depth == clusters.size()) return true;
    // std::cerr << depth << " ";
    cluster* cur = clusters[depth];
    int lbl = cur->members.size();
    ++depth;
    if (cur->selected && solve()) {
        return true;
    } else {
        for (cell* m: cur->members) {
            if (m->value == 0 && check_adj_unocc(m->r, m->c)
                && !rowcheck[m->r][lbl-1]
                && !colcheck[m->c][lbl-1]) {
                m->value = lbl;
                rowcheck[m->r][lbl-1] = true;
                colcheck[m->c][lbl-1] = true;
                m->owner->selected = m;
                if (solve()) return true;
                m->value = 0;
                rowcheck[m->r][lbl-1] = false;
                colcheck[m->c][lbl-1] = false;
                m->owner->selected = nullptr;
            }
        }
    }
    
    --depth;
    return false;
}


bool putteria_puzzle::check_adj_unocc(int r, int c) {
    for (auto p: NBDS) {
        int rp = r+p.first, cp = c+p.second;
        if (0<=rp && rp<R && 0<=cp && cp<C 
        && board_state[rp][cp]->value > 0) {
            return false;
        } 
    }
    return true;
}





// pretty printing

void putteria_puzzle::print_layout() {
    int digits = ceil(log10(clusters.size()))+1;
    for (int r=0; r<R; ++r) {
        for (int c=0; c<C; ++c) {
            std::cout << std::setw(digits) << board_state[r][c]->owner->id;
        } std::cout << std::endl;
    }
}

void putteria_puzzle::print_values() {
    int digits = ceil(log10(clusters.size()))+1;
    for (int r=0; r<R; ++r) {
        for (int c=0; c<C; ++c) {
            std::cout << std::setw(2);
            if (board_state[r][c]->value > 0) {
                std::cout << board_state[r][c]->value;
            } else {
                std::cout << '.';
            }
            
        } std::cout << std::endl;
    }
}