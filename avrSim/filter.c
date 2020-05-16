#include "filter.h"

// possible function codes:
//
// 1, 2, 3, 4, 5, 6, 15, 16
pkg filter[4] = {
		{1, {1, 2, 3, 4, 5, 6, 15, 16}, 3, 3}, // slave 1 is allowed to do wtv he wants, no more than 3 read operations, no more than 3 write operations
		{2, {1, 2, 4, 5, 6, 16}, 7, 5}, // slave 2 cannot write multiple coils
		{3, {1, 2, 3, 4, 5, 6, 15, 16}, 8, 9}, // slave 3 can do anything he wants
		{4, {1, 2, 3, 4, 5, 6, 15}, 9, 9} // slave 4 cannot write multiple holding registers
};
