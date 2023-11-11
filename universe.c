#include "universe.h"

#include <inttypes.h>
#include <stdlib.h>

typedef struct Universe {
    uint32_t rows;
    uint32_t cols;
    bool **grid;
    bool toroidal;
} Universe;

// Prototyping static functions used only within this file.
static bool is_valid_cell(Universe *u, int32_t r, int32_t c);
static uint32_t mod_col(Universe *u, uint32_t c, int32_t x);
static uint32_t mod_row(Universe *u, uint32_t r, int32_t y);

// Creates and returns a Universe.
//
// rows : number of rows to make the matrix/universe
// cols : number of columns to make the matrix/universe
// toroidal : decides whether the universe is toroidal
//            or not.
Universe *uv_create(uint32_t rows, uint32_t cols, bool toroidal) {
    Universe *u = (Universe *) malloc(sizeof(Universe));
    u->toroidal = toroidal;
    u->rows = rows;
    u->cols = cols;
    u->grid = (bool **) calloc(u->rows, sizeof(bool *));
    for (uint32_t r = 0; r < u->rows; r++) {
        u->grid[r] = (bool *) calloc(cols, sizeof(bool));
    }
    return u;
}

// Deletes universe by freeing all allocated space.
//
// u : Universe (pointer)
void uv_delete(Universe *u) {
    // Freeing each column.
    for (uint32_t r = 0; r < u->rows; r++) {
        free(u->grid[r]);
        u->grid[r] = NULL;
    }
    // Freeing rows.
    free(u->grid);
    u->grid = NULL;
    // Free Universe.
    free(u);
    u = NULL;
}

// Returns number of rows in a Universe.
//
// u : Universe (pointer)
uint32_t uv_rows(Universe *u) {
    return u->rows;
}

// Returns number of columns.
//
// u : Universe (pointer)
uint32_t uv_cols(Universe *u) {
    return u->cols;
}

// Sets a cell in given Universe to "alive" (true).
//
// u : Universe (pointer)
// r : row of grid to alter
// c : column of grid to alter
void uv_live_cell(Universe *u, uint32_t r, uint32_t c) {
    u->grid[r][c] = true;
}

// Sets a cel in given Universe to "dead" (false)
//
// u : Universe (pointer)
// r : row of grid to alter
// c : column of grid to alter
void uv_dead_cell(Universe *u, uint32_t r, uint32_t c) {
    u->grid[r][c] = false;
}

// Return value of cell.
//
// u : Universe (pointer)
// r : row
// c : column
bool uv_get_cell(Universe *u, uint32_t r, uint32_t c) {
    return u->grid[r][c];
}

// Fills Universe with "live" cells given from infile.
// Returns true if population successful,
// otherwise exit 1 ???
//
// u : Universe (pointer)
// infile : file pointer to input file
bool uv_populate(Universe *u, FILE *infile) {
    uint32_t row, col;
    int32_t items;
    // Make those alive specified by file
    while ((items = fscanf(infile, "%" PRIu32 " %" PRIu32, &row, &col)) != EOF) {
        if (items < 2 || !is_valid_cell(u, row, col)) {
            return false;
        }
        uv_live_cell(u, row, col);
    }
    return true;
}

// Print Universe to outfile if specified by caller.
//
// u : Universe (pointer)
// outfile : file pointer to output file
void uv_print(Universe *u, FILE *outfile) {
    for (uint32_t r = 0; r < uv_rows(u); r++) {
        for (uint32_t c = 0; c < uv_cols(u); c++) {
            if (uv_get_cell(u, r, c)) {
                fprintf(outfile, "o");
            } else {
                fprintf(outfile, ".");
            }
        }
        fprintf(outfile, "\n");
    }
}

// Helper function that returns true if
// the cell is within the grid or false if not.
// Returns boolean.
//
// u : Universe
// r : row of cell
// c : column of cell
static bool is_valid_cell(Universe *u, int32_t r, int32_t c) {
    if ((0 <= r && r < (int32_t) uv_rows(u)) && (0 <= c && c < (int32_t) uv_cols(u))) {
        return true;
    }
    return false;
}

// BEGIN CITATION
// Both mod_row() and mod_col() used modulus operation.
// I borrowed this from Tutor Ben on Discord.
// Here's the message link and the message used below:
//
// https://discord.com/channels/1035678172856995900/1061813484670681108/1072642931388854373
//  /dev/nvme190n1 (Ben) — 02/07/2023 2:20 PM
//  ((x % y) + y) % y

// Helper function that returns row component of
// cell for a toroidal universe.
// Returns unsigned integer.
//
// u : Universe
// r : row of cell
// y : delta y/row - will be {-1, 0, 1}
static uint32_t mod_row(Universe *u, uint32_t r, int32_t y) {
    return (uint32_t) (((int32_t) r + y + (int32_t) uv_rows(u)) % (int32_t) uv_rows(u));
}

// Helper function that returns column component of
// cell for a toroidal universe.
// Returns unsigned integer.
//
// u : Universe
// c : column of cell
// x : delta x/column - will be {-1, 0, 1}
static uint32_t mod_col(Universe *u, uint32_t c, int32_t x) {
    return (uint32_t) (((int32_t) c + x + (int32_t) uv_cols(u)) % (int32_t) uv_cols(u));
}

// END CITATION

// Returns number of neighbors a cell has.
// Returns as unsigned integer.
//
// u : Universe
// r : row of cell
// c : column of cell
uint32_t uv_census(Universe *u, uint32_t r, uint32_t c) {
    uint32_t live_neighbors = 0;
    for (int32_t y = 1; y >= -1; y--) {
        for (int32_t x = 1; x >= -1; x--) {
            if (u->toroidal) {
                if (uv_get_cell(u, mod_row(u, r, y), mod_col(u, c, x))) {
                    // Check that we're not counting cell of interest.
                    if (x || y) {
                        live_neighbors++;
                    }
                }
            } else {
                if (is_valid_cell(u, (int32_t) r + y, (int32_t) c + x)) {
                    // Check the cell is alive and not cell of interest.
                    if (uv_get_cell(u, r + y, c + x) && (x || y)) {
                        live_neighbors++;
                    }
                }
            }
        }
    }
    return live_neighbors;
}
