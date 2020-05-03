#include "filter.h"

pkg filter[3] = {
		{1, {1, 2, 3, 4, 5, 6, 15, 16}, 8},
		{2, {1, 2, 4, 5, 6, 16}, 7}, // slave 2 cannot force multiple coils
		{3, {1, 2, 3, 4, 5, 6, 15, 16}, 8}
};
