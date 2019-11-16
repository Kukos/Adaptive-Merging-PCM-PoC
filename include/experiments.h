#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

/*
    Implementation of experiments proposed to proof value of new concept

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL 3.0
*/

#include <stddef.h>

/*
    Normal workload experiment

        1. Bulkload N /2,
        2. sqrt(N) point search,
        3. N / 2 x Insert,
        4. Nlog(N) range search with 10% selectivity,
        5. sqrt(N) delete,
        6. sqrt(N) point search,
        7. sqrt(N) update,
        8. Nlog(N) range search with 5% selectivity

    PARAMS
    @IN queries - number of queries in batch (N)

    RETURN
    This is a void function
*/
void db_raw_experiment_workload(size_t queries);

#endif