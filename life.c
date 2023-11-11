#include "set.h"
#include "universe.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DELAY   50000
#define OPTIONS "htsn:i:o:"

char *help_msg = "\
 SYNOPSIS\n\
     Conway's Game of Life\n\
 \n\
 USAGE\n\
     ./life_amd64 tsn:i:o:h\n\
 \n\
 OPTIONS\n\
     -t             Create your universe as a toroidal\n\
     -s             Silent - do not use animate the evolution\n\
     -n {number}    Number of generations [default: 100]\n\
     -i {file}      Input file [default: stdin]\n\
     -o {file}      Output file [default: stdout]\n";

int main(int argc, char **argv) {
    Set opset = set_empty();
    int opt;
    FILE *infile = stdin;
    FILE *outfile = stdout;
    uint32_t rowsize, colsize;
    uint32_t generations = 100;
    bool toroidal = false;
    Universe *temp;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 't':
            opset = set_insert(opset, 0);
            toroidal = true;
            break;
        case 's': opset = set_insert(opset, 1); break;
        case 'n':
            opset = set_insert(opset, 2);
            generations = atoi(optarg);
            break;
        case 'i':
            opset = set_insert(opset, 3);
            infile = fopen(optarg, "r");
            break;
        case 'o':
            opset = set_insert(opset, 4);
            outfile = fopen(optarg, "w");
            break;
        case 'h': opset = set_insert(opset, 5); break;
        default: break;
        }
    }

    // Check help flag.

    if (set_member(opset, 5)) {
        printf("%s", help_msg);
        return 0;
    }

    // Check if user specifies input file.
    // Otherwise use stdin.

    int32_t items = fscanf(infile, "%u %u", &rowsize, &colsize);
    if (items < 2) {
        printf("Malformed input.\n");
        return 0;
    }
    Universe *A = uv_create(rowsize, colsize, toroidal);
    Universe *B = uv_create(rowsize, colsize, toroidal);

    if (!uv_populate(A, infile)) {
        printf("Malformed input.\n");
        return 0;
    }

    // Only execute ncruses instructions when
    // -s flag not enabled.
    if (!set_member(opset, 1)) {
        initscr();
        curs_set(FALSE);
    }

    for (uint32_t i = 0; i < generations; i++) {
        for (uint32_t n = 0; n < uv_rows(A); n++) {
            for (uint32_t m = 0; m < uv_cols(A); m++) {
                if (uv_get_cell(A, n, m) && (uv_census(A, n, m) == 2 || uv_census(A, n, m) == 3)) {
                    uv_live_cell(B, n, m);
                } else if (!(uv_get_cell(A, n, m)) && uv_census(A, n, m) == 3) {
                    uv_live_cell(B, n, m);
                } else {
                    uv_dead_cell(B, n, m);
                }
            }
        }
        temp = A;
        A = B;
        B = temp;
        if (!set_member(opset, 1)) {
            clear();
            for (uint32_t r = 0; r < uv_rows(A); r++) {
                for (uint32_t c = 0; c < uv_cols(A); c++) {
                    if (uv_get_cell(A, r, c)) {
                        mvprintw(r, c, "o");
                    }
                }
            }
            refresh();
            usleep(DELAY);
        }
    }
    if (!set_member(opset, 1)) {
        endwin();
    }
    uv_print(A, outfile);
    // Free all allocated memory.
    uv_delete(A);
    uv_delete(B);
    fclose(infile);
    fclose(outfile);
    return 0;
}
