#include "filter.h"

/*
 *
 * {SLAVE or SLAVES,
    {ALLOWED COMMAND 1, ADDRESS or ADDRESS INTERVAL, VALUE INTERVAL},
    {ALLOWED COMMAND 2, ADDRESS or ADDRESS INTERVAL, VALUE INTERVAL},
    {ALLOWED COMMAND 3, ADDRESS or ADDRESS INTERVAL, VALUE INTERVAL},
	}
*/


// possible function codes:
//
// 1, 2, 3, 4, 5, 6, 15, 16
pkg filter[4] = {
		{1, {1, 2, 3, 4, 5, 16, 40001, 40002, 40003}, 3, 3}, // slave 1 is allowed to do wtv he wants, no more than 3 read operations, no more than 3 write operations
		{2, {1, 2, 3, 4, 5, 16, 40001, 40004, 40005}, 7, 5}, // slave 2 cannot write multiple coils
		{3, {1, 2, 3, 4, 5, 6, 15, 16}, 8, 9}, // slave 3 can do anything he wants
		{4, {1, 2, 3, 4, 5, 6, 15}, 9, 9} // slave 4 cannot write multiple holding registers
};
