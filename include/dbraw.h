#ifndef DBRAW_H
#define DBRAW_H

/*
    Simualtion of raw (unsorted) table

    Assumptions:
    2. Insert time = 1
    3. Bulkload Time = P(N)
    4. Search Time = P(N)
    5. Seq Search Time = P(N)
    6. Delete Time = P(N)
    7. Update Time =  P(N)

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE GPL 3.0
*/

#include <compiler.h>
#include <stddef.h>
#include <sys/types.h>
#include <pcm.h>

typedef struct DB_raw
{
    size_t num_entries;
    size_t entry_size; /* in bytes */

    PCM *pcm;

} DB_raw;

/*
    Create empty raw table

    PARAMS
    @IN PCM - pcm
    @IN entry_size - size of entry in Bytes

    RETURN
    Pointer to new raw Table
*/
DB_raw *db_raw_create(PCM *pcm, size_t entry_size);

/*
    Destroy raw table

    PARAMS
    @IN raw - pointer to raw table

    RETURN
    This is a void function
*/
void db_raw_destroy(DB_raw *raw);

/*
    Insert entries to raw

    PARAMS
    @IN raw - pointer to raw table
    @IN entries - entries to insert

    RETURN
    Insert time
*/
double db_raw_insert(DB_raw *raw, size_t entries);

/*
    Insert entries via bulkload method

    PARAMS
    @IN raw - pointer to raw table
    @IN entries - entries to insert

    RETURN
    Insert time
*/
double db_raw_bulkload(DB_raw *raw, size_t entries);

/*
    Find entries by point search

    PARAMS
    @IN raw - pointer to raw table
    @IN entries - entries to find

    RETURN
    Search time
*/
double db_raw_point_search(DB_raw *raw, size_t entries);

/*
    Find entries by range search

    PARAMS
    @IN raw - pointer to raw table
    @IN entries - entries to find

    RETURN
    Search time
*/
double db_raw_range_search(DB_raw *raw, size_t entries);

/*
    Delete entries from raw table

    PARAMS
    @IN raw - pointer to raw table
    @IN entries - entries to delete

    RETURN
    Delete time
*/
double db_raw_delete(DB_raw *raw, size_t entries);

/*
    Update entries in raw table

    PARAMS
    @IN raw - pointer to raw table
    @IN entries - entries to update

    RETURN
    Update Time
*/
double db_raw_update(DB_raw *raw, size_t entries);

#endif