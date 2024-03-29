#ifndef DBINDEX_H
#define DBINDEX_H

/*
    Simualtion of B+Tree index on PCM

    Assumptions:
    1. Height H = log_{node_size / (sizeof(void *) + key_size}(n)
    2. Insert time = H
    3. Bulkload Time = P(N) + H
    4. Search Time = H
    5. Seq Search Time = H + P(N)
    6. Delete Time = H
    7. Update Time =  Delete Time + Insert Time

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE GPL 3.0
*/

#include <compiler.h>
#include <stddef.h>
#include <sys/types.h>
#include <pcm.h>

typedef enum
{
    BTREE_NORMAL, /* normal B+ like in RAM */
    BTREE_UNSORTED_LEAVES, /* B+ with unsorted leaves only */
    BTREE_UNSORTED_INNERS_UNSORTED_LEAVES, /* every node is unsorted */
    CBTREE, /* OverFlow node is added to each leaf to postpone split. But leaves are still sorted */
    OCBTREE, /* CBTree + OverFlow node also on the last level of inners */
    BTREE_2SECTION_NODE, /* 2sections, [SORTED|UNSORTED] to keep good insertion and good deletion */
    BTREE_SKIP_COST,
    /* buffered versions so inners are in RAM */
    BTREE_NORMAL_INNERS_RAM,
    BTREE_UNSORTED_LEAVES_INNERS_RAM,
    CBTREE_INNERS_RAM,
    BTREE_2SECTION_NODE_INNERS_RAM,
    BTREE_WITH_BUFFERED_TREE, /* B+Tree 2sectionNode + full buffered tree in RAM */
} btree_type_t;

#define CBTREE_OPERATION_BUFFER_SIZE         10
#define OCBTREE_OPERATION_BUFFER_SIZE        10
#define BTREE_WITH_BUFFERED_TREE_BUFFER_SIZE 1000
typedef struct DB_index
{
    size_t num_entries;
    size_t entry_size; /* in bytes */
    size_t key_size; /* in bytes */
    size_t height;

    size_t node_size; /* in bytes */
    double node_factor;

    btree_type_t type;

    size_t buffered_operation; /* used in CBTree and OCBTree */

    PCM *pcm;
} DB_index;

/*
    Create empty index

    PARAMS
    @IN PCM - pcm
    @IN key_size - size of key in Bytes
    @IN entry_size - size of entry in Bytes
    @IN node_size - size of node in Bytes
    @IN node_factor - node factor
    @IN btree_type - type of BTree

    RETURN
    Pointer to new index
*/
DB_index *db_index_create(PCM *pcm, size_t key_size, size_t entry_size, size_t node_size, double node_factor, btree_type_t btree_type);

/*
    Destroy index

    PARAMS
    @IN index - pointer to index

    RETURN
    This is a void function
*/
void db_index_destroy(DB_index *index);

/*
    Insert entries to index

    PARAMS
    @IN index - pointer to index
    @IN entries - entries to insert

    RETURN
    Insert time
*/
double db_index_insert(DB_index *index, size_t entries);

/*
    Insert entries via bulkload method

    PARAMS
    @IN index - pointer to index
    @IN entries - entries to insert

    RETURN
    Insert time
*/
double db_index_bulkload(DB_index *index, size_t entries);

/*
    Find entries by point search

    PARAMS
    @IN index - pointer to index
    @IN entries - entries to find

    RETURN
    Search time
*/
double db_index_point_search(DB_index *index, size_t entries);

/*
    Find entries by range search

    PARAMS
    @IN index - pointer to index
    @IN entries - entries to find

    RETURN
    Search time
*/
double db_index_range_search(DB_index *index, size_t entries);

/*
    Delete entries from index

    PARAMS
    @IN index - pointer to index
    @IN entries - entries to delete

    RETURN
    Delete time
*/
double db_index_delete(DB_index *index, size_t entries);

/*
    Update entries in index

    PARAMS
    @IN index - pointer to index
    @IN entries - entries to update

    RETURN
    Update Time
*/
double db_index_update(DB_index *index, size_t entries);

#endif