#ifndef DBPAM_H
#define DBPAM_H

/*
    Simulation of Adaptive Merging optimal for PCM

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
#include <dbam.h>

typedef struct DB_PAM
{
    DB_AM *am;
} DB_PAM;

/*
    Create instance of PAM system

    PARAMS
    @IN PCM - pcm
    @IN num_entries - number of entries in table (process works on those entries)
    @IN key_size - key size in Bytes
    @IN entry_size - size of entry in Bytes
    @IN buffer_size - buffer size (size in bytes)
    @IN index_node_size - size of B+TreeNode in Bytes

    RETURN
    Pointer to new raw Table
*/
DB_PAM *db_pam_create(PCM *pcm, size_t num_entries, size_t key_size, size_t entry_size, size_t buffer_size, size_t index_node_size);

/*
    Destroy PAM system

    PARAMS
    @IN pam - pointer to PAM system

    RETURN
    This is a void function
*/
void db_pam_destroy(DB_PAM *pam);

/*
    Find entries by key

    PARMAS
    @IN pam - pointer to PAM system
    @IN type - query type
    @IN entries - number of entries to find

    RETURN
    Query time
*/
double db_pam_search(DB_PAM *pam, query_t type, size_t entries);

#endif