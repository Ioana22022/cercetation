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
		{1, 4, {1, 2, 3, 4, 16}, 1, {40001, 40002, 40003}}, 
		{2, 4, {1, 2, 3, 4, 16}, 1, {40002}},
		{3, 4, {1, 2, 3, 16}, 1, {40005}},
		{4, 1, {0x01}, 1, {40004}}
};
