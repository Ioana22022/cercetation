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
		{1, 7, {1, 2, 3, 4, 5, 6, 16}, 3, {40001, 40002, 40003}, 4, {256, 258, 261, 263}}, 
		{2, 5, {1, 2, 3, 4, 16}, 1, {40002}, 2, {255, 256}},
		{3, 5, {1, 2, 3, 4, 16}, 2, {40005, 40006}, 3, {258, 261, 263}},
		{4, 1, {0x04}, 1, {40004}, 3, {256, 258, 261}}
};
