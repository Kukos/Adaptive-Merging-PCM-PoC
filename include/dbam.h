#ifndef DBAM_H
#define DBAM_H

/*
    Simulation of Adaptive Merging

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE GPL 3.0
*/

#include <pcm.h>
#include <dbindex.h>
#include <stddef.h>
#include <darray.h>
#include <dbutils.h>
#include <partitions.h>

typedef struct DB_AM
{
    // table
    __extension__  struct
    {
        size_t num_entries; /* num entries in whole table (const value) */
        size_t entry_size;
    };

    size_t sort_buffer_size; /* in bytes */

    size_t num_of_partitions;
    size_t num_entries_in_partitions;

    DB_index *index;
    DB_index *deletion_index;
    PCM *pcm;

    invalidation_type_t invalidation_type;
} DB_AM;

/*
    Create instance of AM system

    PARAMS
    @IN PCM - pcm
    @IN num_entries - number of entries in table (process works on those entries)
    @IN key_size - key size in Bytes
    @IN entry_size - size of entry in Bytes
    @IN buffer_size - buffer size (size in bytes)
    @IN index_node_size - size of B+TreeNode in Bytes
    @IN invalidation_type - algorithm to invalidate entries during moving from partition to index
    @IN indeX_type - B+Tree type

    RETURN
    Pointer to new raw Table
*/
DB_AM *db_am_create(PCM *pcm, size_t num_entries, size_t key_size, size_t entry_size, size_t buffer_size, size_t index_node_size, invalidation_type_t invalidation_type, btree_type_t index_type);

/*
    Destroy AM system

    PARAMS
    @IN am - pointer to AM system

    RETURN
    This is a void function
*/
void db_am_destroy(DB_AM *am);

/*
    Find entries by key

    PARMAS
    @IN am - pointer to AM system
    @IN type - query type
    @IN entries - number of entries to find

    RETURN
    Query time
*/
double db_am_search(DB_AM *am, query_t type, size_t entries);

/*
    Insert entries into AM Index

    PARAMS
    @IN am - pointer to AM system
    @IN entries - number fo entries to insert

    RETURN
    Query time
*/
double db_am_insert(DB_AM *am, size_t entries);

/*
    Delete entries from AM

    PARAMS
    @IN am - pointer to AM system
    @IN entries - number fo entries to delete

    RETURN
    Query time
*/
double db_am_delete(DB_AM* am, size_t entries);

#endif