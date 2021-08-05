#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

/*
    Implementation of experiments proposed to proof value of new concept

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL 3.0
*/

#include <stddef.h>
#include <dbutils.h>

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
void db_index_experiment_workload(size_t queries);

/*
    This is only test workload for db la to check all of functions

    PARAMS
    @IN entries - number of entries in table

    RETURN
    This is a void function
*/
void db_am_experiment_workload(size_t entries);

/*
    This is only test workload for db la to check all of functions

    PARAMS
    @IN entries - number of entries in table

    RETURN
    This is a void function
*/
void db_pam_experiment_workload(size_t entries);

/*
    This experiment shows invalidation impact on whole AM process time and PCM wearout

    PARAMS
    @IN file - base file name
    @IN key_size - sizeof(key) in Table T
    @IN data_size - sizeof(Record) in Table T
    @IN entries - how many entries is in Table T
    @IN type - query type used to build index
    @IN selectivity - query selectivity

    RETURN
    This is a void function
*/
void experiment1(const char *file, size_t key_size, size_t data_size, size_t entries, query_t type, double selectivity);

/*
    This experiment shows Btree Type impact on whole AM process time and PCM wearout

    PARAMS
    @IN file - base file name
    @IN key_size - sizeof(key) in Table T
    @IN data_size - sizeof(Record) in Table T
    @IN entries - how many entries is in Table T
    @IN type - query type used to build index
    @IN selectivity - query selectivity

    RETURN
    This is a void function
*/
void experiment2(const char *file, size_t key_size, size_t data_size, size_t entries, query_t type, double selectivity);

/*
    AM vs PAM.
    Time per query + Total Time + Total Wearout

    PARAMS
    @IN file - base file name
    @IN key_size - sizeof(key) in Table T
    @IN data_size - sizeof(Record) in Table T
    @IN entries - how many entries is in Table T
    @IN type - query type used to build index
    @IN selectivity - query selectivity

    RETURN
    This is a void function
*/
void experiment3(const char *file, size_t key_size, size_t data_size, size_t entries, query_t type, double selectivity);

void experiment3_1(const char * const file, size_t key_size, size_t data_size, size_t entries, query_t type, double selectivity);

typedef struct StressBatch
{
    size_t rsearches;
    size_t psearches;
    size_t inserts;
    size_t deletes;

    double selectivity_min; /* when step == 0 use min */
    double selectivity_max;
    double selectivity_step;
} StressBatch;

void experiment_stress(const char* file, size_t key_size, size_t data_size, size_t entries, StressBatch* batch, size_t batches);

void experiment_stress_step(const char* file, size_t key_size, size_t data_size, size_t entries, StressBatch* batch, size_t batches);

void experiment_stress_pam_step(const char* file, size_t key_size, size_t data_size, size_t entries, StressBatch* batch, size_t batches);

void experiment_index(const char * const file, size_t key_size, size_t data_size, size_t entries, StressBatch* batch, size_t batches);

#endif