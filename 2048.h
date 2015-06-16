 /*************************************************************************
 * 
 * Head file for 2048 Artificial Intelligence
 *
 * The source code is modified by Zhou Ao on the basis of the work of Robert Xiao.
 * The original code of Robert is available at https://github.com/nneonneo/2048-ai.
 * And this copy of code is available at https://github.com/Zhou-Ao/2048-ai.
 * __________________
 * 
 *  Notice: the source code is under the MIT License
 *
 */

#include <stdlib.h>
#include <time.h>

typedef unsigned __int64 board_t;
typedef unsigned __int16 row_t;

//store the depth at which the heuristic was recorded as well as the actual heuristic
struct trans_table_entry_t{
	unsigned __int8 depth;
	float heuristic;
};

static const board_t ROW_MASK = 0xFFFFULL;
static const board_t COL_MASK = 0x000F000F000F000FULL;

static inline void print_board(board_t board) {
	int i, j;
	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			printf("%c", "0123456789abcdef"[(board)& 0xf]);
			board >>= 4;
		}
		printf("\n");
	}
	printf("\n");
}

static inline board_t unpack_col(row_t row) {
	board_t tmp = row;
	return (tmp | (tmp << 12ULL) | (tmp << 24ULL) | (tmp << 36ULL)) & COL_MASK;
}

static inline row_t reverse_row(row_t row) {
	return (row >> 12) | ((row >> 4) & 0x00F0) | ((row << 4) & 0x0F00) | (row << 12);
}

#define DELTA_EPOCH_IN_MICROSECS  116444736000000000ULL

struct timezone;

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	FILETIME ft;
	unsigned __int64 tmpres = 0;

	(void)tz;

	if (NULL != tv)
	{
		GetSystemTimeAsFileTime(&ft);

		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		/*converting file time to unix epoch*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		tmpres /= 10;  /*convert into microseconds*/
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}

	return 0;
}

static inline unsigned unif_random(unsigned n) {
	static int seeded = 0;

	if (!seeded) {
		srand(time(NULL));
		seeded = 1;
	}

	return rand() % n;
}

typedef int(*get_move_func_t)(board_t);