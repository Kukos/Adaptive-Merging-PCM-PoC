#include <log.h>
#include <dbindex.h>
#include <pcm.h>
#include <math.h>
#include <stdlib.h>
#include <dbstat.h>
#include <dbutils.h>

#define DB_INDEX_LOG(n, k) (log(n) / log(k))

/*
    Get number of leaves needed for entries

    PARAMS
    @IN entries - number of entries
    @IN entry_size - size of 1 entry in bytes
    @IN node_size - size of index node in bytes

    RETURN
    Number of leaves needed to write entries into them
*/
static ___inline___ size_t db_index_get_leaves_number_for_entries(size_t entries, size_t entry_size, size_t node_size);

/*
    Get number of leaves in BTree

    PARAMS
    @IN index - pointer to Index

    RETURN
    Number of Leaves
*/
static ___inline___ size_t db_index_get_leaves_number(const DB_index *index);

/*
    Calculate height of BTree

    PARAMS
    @IN index - pointer to Index

    RETURN
    Index (BTree) Height
*/
static ___inline___ size_t db_index_get_height(const DB_index *index);

/*
    Get number of keys that can be write into one node

    PARAMS
    @IN index - pointer to Index

    RETURN
    Number of keys per node
*/
static ___inline___ size_t db_index_keys_per_node(const DB_index *index);

/*
    Get number of inner nodes

    PARAMS
    @IN index - pointer to Index

    RETURN
    Number of inner nodes
*/
static ___inline___ size_t db_index_get_inners_number(const DB_index *index);

static double db_index_find_node(DB_index* index);

static ___inline___ size_t db_index_keys_per_node(const DB_index *index)
{
    const double keys_per_node = ceil(((double)index->node_size * index->node_factor) / (double)(index->key_size + sizeof(void *)));

    return (size_t)keys_per_node;
}

static ___inline___ size_t db_index_get_leaves_number_for_entries(size_t entries, size_t entry_size, size_t node_size)
{
    const double leaves = ceil((double)(entries * entry_size) / (double)node_size);

    return (size_t)leaves;
}

static ___inline___ size_t db_index_get_leaves_number(const DB_index *index)
{
    return db_index_get_leaves_number_for_entries(index->num_entries, index->entry_size, (size_t)((double)index->node_size * index->node_factor));
}

static ___inline___ size_t db_index_get_height(const DB_index *index)
{
    const size_t keys_per_node = db_index_keys_per_node(index);
    const size_t leaves = db_index_get_leaves_number(index);
    const double height = leaves == 1 ? 1.0 : ceil(DB_INDEX_LOG((double)leaves, (double)keys_per_node));

    return (size_t)height + 1;
}

static ___inline___ size_t db_index_get_inners_number(const DB_index *index)
{
    size_t inners = 0;
    size_t nodes =  db_index_get_leaves_number(index);
    const size_t keys_per_node = db_index_keys_per_node(index);

    while (nodes > 1)
    {
        nodes = INT_CEIL_DIV(nodes, keys_per_node);
        inners += nodes;
    }

    return inners;
}

static double db_index_find_node(DB_index* index)
{
    double time = 0.0;
    ssize_t j;

    switch (index->type)
    {
        case BTREE_NORMAL:
        case CBTREE:
        case OCBTREE:
        case BTREE_2SECTION_NODE:
        case BTREE_UNSORTED_LEAVES:
        {
            /* everything is sorted so scan each level using binary search */
            for (j = 0; j < (ssize_t)(index->height - 1); ++j)
                    time += pcm_read(index->pcm, index->node_size / 2); /* binary search is 2x faster than linera in avg due to cache misses */

            break;
        }
        case BTREE_UNSORTED_INNERS_UNSORTED_LEAVES:
        {
            /* everything is unsorted so scan each level using linear search */
            for (j = 0; j < (ssize_t)(index->height - 1); ++j)
                time += pcm_read(index->pcm, index->node_size);

            break;
        }
        case BTREE_NORMAL_INNERS_RAM:
        case CBTREE_INNERS_RAM:
        case BTREE_2SECTION_NODE_INNERS_RAM:
        case BTREE_WITH_BUFFERED_TREE:
        case BTREE_UNSORTED_LEAVES_INNERS_RAM:
        {
            /* Inners in RAM so skip */
            break;
        }
        case BTREE_SKIP_COST:
            break;

        default:
            break;
    }

    return time;
}

DB_index *db_index_create(PCM *pcm, size_t key_size, size_t entry_size, size_t node_size, double node_factor, btree_type_t btree_type)
{
    DB_index *index;

    TRACE();

    index = (DB_index *)malloc(sizeof(DB_index));
    if (index == NULL)
        ERROR("malloc error\n", NULL);

    index->pcm = pcm;
    index->key_size = key_size;
    index->entry_size = entry_size;
    index->node_size = node_size;
    index->node_factor = node_factor;
    index->type = btree_type;

    index->num_entries = 0;
    index->height = 0;
    index->buffered_operation = 0;

    return index;
}

void db_index_destroy(DB_index *index)
{
    TRACE();

    if (index == NULL)
        return;

    FREE(index);
}

double db_index_insert(DB_index *index, size_t entries)
{
    double time = 0.0;

    size_t old_leaves;
    size_t new_leaves;
    size_t diff_leaves;
    size_t old_inners;
    size_t new_inners;
    size_t diff_inners;

    size_t i;
    ssize_t j;

    TRACE();

    if (index->type == BTREE_WITH_BUFFERED_TREE)
    {
        index->buffered_operation += entries;
        if (index->buffered_operation >= BTREE_WITH_BUFFERED_TREE_BUFFER_SIZE)
        {
            index->num_entries += entries;

            const size_t entries_to_insert = index->buffered_operation;
            index->buffered_operation = 0;
            /* temp workaround for breaking the unreal, perfect cache line insertion */
            time = db_index_bulkload(index, entries_to_insert) * 4.0;
            db_stat_update_index_time(time * 3);

            /* we added them during buffering and in bulkload so sub bulklaod */
            index->num_entries -= entries_to_insert;

            return time;
        }
    }

    for (i = 0; i < entries; ++i)
    {
        /* Find place to insert new data */
        time += db_index_find_node(index);

        /* check numbers of inners */
        old_inners = db_index_get_inners_number(index);

        /* check numbers of leaves */
        old_leaves = db_index_get_leaves_number(index);

        /* note insert */
        ++index->num_entries;

        new_inners = db_index_get_inners_number(index);
        new_leaves = db_index_get_leaves_number(index);

        diff_inners = new_inners - old_inners;
        diff_leaves = new_leaves - old_leaves;
        switch (index->type)
        {
            case BTREE_NORMAL:
            {
                /* insert in sorted order to leaf, so we need to move in avg half node */
                time += pcm_write(index->pcm, index->node_size / 2);

                /* now we can insert entry */
                time += pcm_write(index->pcm, index->entry_size);

                /* we need to insert new pointer to inners, if we need a new leaf */
                if (diff_leaves > 0)
                {
                    /* split node */
                    time += pcm_write(index->pcm, index->node_size / 2);

                    /* insert pointer to leaf into inner */
                    time += pcm_write(index->pcm, index->key_size + sizeof(void *));
                    time += pcm_write(index->pcm, index->node_size / 2);

                    /* insert new inners */
                    for (j = 0; j < (ssize_t)diff_inners; ++j)
                    {
                        /* split node */
                        time += pcm_write(index->pcm, index->node_size / 2);

                        /* we need a new pointer in inner node, but inners are sorted so make a gap, or move to new inner */
                        time += pcm_write(index->pcm, index->node_size / 2);

                        /* write down a key with pointer */
                        time += pcm_write(index->pcm, index->key_size + sizeof(void *));
                    }
                }

                break;
            }
            case BTREE_UNSORTED_LEAVES:
            {
                /* we need to write down, new entry at the end */
                time += pcm_write(index->pcm, index->entry_size);

                /* and we need to update bitmap */
                time += pcm_write(index->pcm, 1);

                if (diff_leaves > 0)
                {
                    /* if we need a new leaf, we need to split node, move half of node to another leaf and set bitmap */
                    time += pcm_write(index->pcm, index->node_size / 2);
                    time += pcm_write(index->pcm, 1);

                    /* insert pointer to leaf into inner */
                    time += pcm_write(index->pcm, index->node_size / 2);
                    time += pcm_write(index->pcm, index->key_size + sizeof(void *));

                    /* insert new inners */
                    for (j = 0; j < (ssize_t)diff_inners; ++j)
                    {
                        /* split node */
                        time += pcm_write(index->pcm, index->node_size / 2);

                        /* we need a new pointer in inner node, but inners are sorted so make a gap, or move to new inner */
                        time += pcm_write(index->pcm, index->node_size / 2);

                        /* write down a key with pointer */
                        time += pcm_write(index->pcm, index->key_size + sizeof(void *));
                    }
                }

                break;
            }
            case BTREE_UNSORTED_INNERS_UNSORTED_LEAVES:
            case BTREE_2SECTION_NODE:
            {
                /* we need to write down, new entry at the end */
                time += pcm_write(index->pcm, index->entry_size);

                /* and we need to update bitmap */
                time += pcm_write(index->pcm, 1);

                if (diff_leaves > 0)
                {
                    /* if we need a new leaf, we need to split node, move half of node to another leaf and set bitmap */
                    time += pcm_write(index->pcm, index->node_size / 2);
                    time += pcm_write(index->pcm, 1);

                    /* write down key + pointer to the new leaf */
                    time += pcm_write(index->pcm, index->key_size + sizeof(void *));
                    time += pcm_write(index->pcm, 1);

                    for (j = 0; j < (ssize_t)diff_inners; ++j)
                    {
                        /* split node */
                        time += pcm_write(index->pcm, index->node_size / 2);

                        /* write down a key with pointer */
                        time += pcm_write(index->pcm, index->key_size + sizeof(void *));

                        /* and we need to update bitmap */
                        time += pcm_write(index->pcm, 1);
                    }
                }
                break;
            }
            case CBTREE:
            {
                /* insert in sorted order to leaf, so we need to move in avg half node */
                time += pcm_write(index->pcm, index->node_size / 2);

                /* now we can insert entry */
                time += pcm_write(index->pcm, index->entry_size);

                /* split = writing to OVF so it cost only writing entry */

                index->buffered_operation += diff_leaves;
                while (index->buffered_operation >= CBTREE_OPERATION_BUFFER_SIZE)
                {
                    /* insert */
                    time += pcm_write(index->pcm, index->node_size / 2);
                    time += pcm_write(index->pcm, index->key_size + sizeof(void *));

                    index->buffered_operation -= CBTREE_OPERATION_BUFFER_SIZE;
                }

                /* inners are not buffered */
                for (j = 0; j < (ssize_t)diff_inners; ++j)
                {
                    /* split node */
                    time += pcm_write(index->pcm, index->node_size / 2);

                    /* we need a new pointer in inner node, but inners are sorted so make a gap, or move to new inner */
                    time += pcm_write(index->pcm, index->node_size / 2);

                    /* write down a key with pointer */
                    time += pcm_write(index->pcm, index->key_size + sizeof(void *));
                }

                break;
            }
            case OCBTREE:
            {
                /* insert in sorted order to leaf, so we need to move in avg half node */
                time += pcm_write(index->pcm, index->node_size / 2);

                /* now we can insert entry */
                time += pcm_write(index->pcm, index->entry_size);

                /* split = writing to OVF so it cost only writing entry */

                /* inners are also buffered */
                index->buffered_operation += diff_leaves + diff_inners;
                while (index->buffered_operation >= OCBTREE_OPERATION_BUFFER_SIZE)
                {
                    /* insert */
                    time += pcm_write(index->pcm, index->node_size / 2);
                    time += pcm_write(index->pcm, index->key_size + sizeof(void *));

                    index->buffered_operation -= OCBTREE_OPERATION_BUFFER_SIZE;
                }

                break;
            }
            case BTREE_NORMAL_INNERS_RAM:
            case CBTREE_INNERS_RAM: /* OVF nodes do nothing if it is in RAM */
            {
                /* insert in sorted order to leaf, so we need to move in avg half node */
                time += pcm_write(index->pcm, index->node_size / 2);

                /* now we can insert entry */
                time += pcm_write(index->pcm, index->entry_size);
                break;
            }
            case BTREE_UNSORTED_LEAVES_INNERS_RAM:
            case BTREE_2SECTION_NODE_INNERS_RAM:
            {
                /* we need to write down, new entry at the end */
                time += pcm_write(index->pcm, index->entry_size);

                /* and we need to update bitmap */
                time += pcm_write(index->pcm, 1);
                break;
            }
            case BTREE_SKIP_COST:
                break;
            default:
                break;
        }
    }

    index->height = db_index_get_height(index);
    db_stat_update_index_time(time);
    return time;
}

double db_index_bulkload(DB_index *index, size_t entries)
{
    double time = 0.0;

    size_t diff_leaves;
    size_t old_inners;
    size_t new_inners;
    size_t diff_inners;

    size_t i;
    ssize_t j;

    TRACE();

    diff_leaves = db_index_get_leaves_number_for_entries(entries, index->entry_size, (size_t)((double)index->node_size * index->node_factor));

    /* check numbers of inners */
    old_inners = db_index_get_inners_number(index);
    /* note insert */
    index->num_entries += entries;

    /* check number of inners after "fake" insertions to get a diff */
    new_inners = db_index_get_inners_number(index);

    diff_inners = new_inners - old_inners;

    /* build leaves */
    if (index->type !=  BTREE_SKIP_COST)
        time += pcm_write(index->pcm, (size_t)((double)index->node_size * index->node_factor) * diff_leaves);

    /* find place for each leaf */
    for (i = 0; i < diff_leaves; ++i)
        time += db_index_find_node(index);

    switch (index->type)
    {
        case BTREE_NORMAL:
        case BTREE_UNSORTED_LEAVES:
        {
            /* make a gap, or move to another inner */
            time += pcm_write(index->pcm, index->node_size / 2);

            for (i = 0; i < diff_leaves; ++i)
            {
                /* write down a key with pointer */
                time += pcm_write(index->pcm, index->key_size + sizeof(void *));
            }

            /* insert new inners */
            for (j = 0; j < (ssize_t)diff_inners; ++j)
            {
                /* split node */
                time += pcm_write(index->pcm, index->node_size / 2);

                /* we need a new pointer in inner node, but inners are sorted so make a gap, or move to new inner */
                time += pcm_write(index->pcm, index->node_size / 2);

                /* write down a key with pointer */
                time += pcm_write(index->pcm, index->key_size + sizeof(void *));
            }

            break;
        }
        case BTREE_UNSORTED_INNERS_UNSORTED_LEAVES:
        case BTREE_2SECTION_NODE:
        {
            for (i = 0; i < diff_leaves; ++i)
            {
                /* write down a key with pointer at the end */
                time += pcm_write(index->pcm, index->key_size + sizeof(void *));

                /* update bitmap */
                time += pcm_write(index->pcm, 1);
            }

            for (j = 0; j < (ssize_t)diff_inners; ++j)
            {
                /* split node */
                time += pcm_write(index->pcm, index->node_size / 2);

                /* write down a key with pointer */
                time += pcm_write(index->pcm, index->key_size + sizeof(void *));

                /* and we need to update bitmap */
                time += pcm_write(index->pcm, 1);
            }

            break;
        }
        case CBTREE:
        {
            index->buffered_operation += diff_leaves;

            while (index->buffered_operation >= CBTREE_OPERATION_BUFFER_SIZE)
            {
                time += pcm_write(index->pcm, index->key_size + sizeof(void *));

                index->buffered_operation -= CBTREE_OPERATION_BUFFER_SIZE;
            }

            /* inners are not buffered */
            for (j = 0; j < (ssize_t)diff_inners; ++j)
            {
                /* split node */
                time += pcm_write(index->pcm, index->node_size / 2);

                /* we need a new pointer in inner node, but inners are sorted so make a gap, or move to new inner */
                time += pcm_write(index->pcm, index->node_size / 2);

                /* write down a key with pointer */
                time += pcm_write(index->pcm, index->key_size + sizeof(void *));
            }

            break;
        }
        case OCBTREE:
        {
            /* inners are also buffered */
            index->buffered_operation += diff_inners;
            while (index->buffered_operation >= OCBTREE_OPERATION_BUFFER_SIZE)
            {
                /* insert */
                time += pcm_write(index->pcm, index->node_size / 2);
                time += pcm_write(index->pcm, index->key_size + sizeof(void *));

                index->buffered_operation -= OCBTREE_OPERATION_BUFFER_SIZE;
            }
        }
        case BTREE_NORMAL_INNERS_RAM:
        case BTREE_UNSORTED_LEAVES_INNERS_RAM:
        case CBTREE_INNERS_RAM:
        case BTREE_2SECTION_NODE_INNERS_RAM:
        case BTREE_WITH_BUFFERED_TREE:
        {
            /* leaves have been built. So inners stuffs cost as nothing */
            break;
        }
        case BTREE_SKIP_COST:
            break;
        default:
            break;
    }

    db_stat_update_index_time(time);

    index->height = db_index_get_height(index);
    return time;
}

double db_index_point_search(DB_index *index, size_t entries)
{
    double time = 0.0;

    size_t i;
    ssize_t j;

    TRACE();

    for (i = 0; i < entries; ++i)
    {
        switch (index->type)
        {
            case BTREE_NORMAL:
            case CBTREE:
            case OCBTREE:
            case BTREE_2SECTION_NODE:
            {
                /* everything is sorted so scan each level using binary search */
                for (j = 0; j < (ssize_t)index->height; ++j)
                    time += pcm_read(index->pcm, index->node_size / 2); /* binary search is 2x faster than linera in avg due to cache misses */

                break;
            }
            case BTREE_UNSORTED_LEAVES:
            {
                /* scan inner using binary search */
                for (j = 0; j < (ssize_t)(index->height - 1); ++j)
                    time += pcm_read(index->pcm, index->node_size / 2); /* binary search is 2x faster than linera in avg due to cache misses */


                /* scan unsorted leaf */
                time += pcm_read(index->pcm, index->node_size);

                break;
            }
            case BTREE_UNSORTED_INNERS_UNSORTED_LEAVES:
            {
                /* everything is unsorted so scan each level using linear search */
                for (j = 0; j < (ssize_t)index->height; ++j)
                    time += pcm_read(index->pcm, index->node_size);

                break;
            }
            case BTREE_NORMAL_INNERS_RAM:
            case CBTREE_INNERS_RAM:
            case BTREE_2SECTION_NODE_INNERS_RAM:
            case BTREE_WITH_BUFFERED_TREE:
            {
                /* scan sorted leaf */
                time += pcm_read(index->pcm, index->node_size / 2); /* binary search is 2x faster than linera in avg due to cache misses */
                break;
            }
            case BTREE_UNSORTED_LEAVES_INNERS_RAM:
            {
                /* scan unsortef leaf */
                time += pcm_read(index->pcm, index->node_size);
                break;
            }
            case BTREE_SKIP_COST:
                break;

            default:
                break;
        }
    }

    db_stat_update_index_time(time);
    return time;
}

double db_index_range_search(DB_index *index, size_t entries)
{
    double time = 0.0;

    size_t leaves;

    TRACE();

    leaves = db_index_get_leaves_number_for_entries(entries, index->entry_size, (size_t)((double)index->node_size * index->node_factor));

    /* seek 1st leaf */
    time += db_index_find_node(index);

    /* scan all */
    if (index->type != BTREE_SKIP_COST)
        time += pcm_read(index->pcm, leaves * (size_t)((double)index->node_size * index->node_factor));


    db_stat_update_index_time(time);
    return time;
}

double db_index_delete(DB_index *index, size_t entries)
{
    double time = 0.0;

    size_t old_leaves;
    size_t new_leaves;
    size_t diff_leaves;
    size_t old_inners;
    size_t new_inners;
    size_t diff_inners;

    size_t i;
    ssize_t j;

    entries = MIN(entries, index->num_entries);

    TRACE();

    /*
        delete = drop during merge
        so we will update the same bitmap,
        buffer is in RAM so delete is free
    */
    if (index->type == BTREE_WITH_BUFFERED_TREE)
    {
        index->num_entries -= entries;

        return 0.0;
    }

    for (i = 0; i < entries; ++i)
    {
        /* Find place to delete data */
        time += db_index_find_node(index);

        /* check numbers of inners */
        old_inners = db_index_get_inners_number(index);

        /* check numbers of leaves */
        old_leaves = db_index_get_leaves_number(index);

        /* note delete */
        --index->num_entries;

        new_inners = db_index_get_inners_number(index);
        new_leaves = db_index_get_leaves_number(index);

        diff_inners = old_inners - new_inners;
        diff_leaves = old_leaves - new_leaves;
        switch (index->type)
        {
            case BTREE_NORMAL:
            {
                /* insert in sorted order to leaf, so we need to move in avg half node */
                time += pcm_write(index->pcm, index->node_size / 2);

                /* now we can insert entry */
                time += pcm_write(index->pcm, index->entry_size);

                /* we need to insert new pointer to inners, if we need a new leaf */
                if (diff_leaves > 0)
                {
                    /* merge node */
                    time += pcm_write(index->pcm, index->node_size / 2);

                    /* insert pointer to leaf into inner */
                    time += pcm_write(index->pcm, index->key_size + sizeof(void *));
                    time += pcm_write(index->pcm, index->node_size / 2);

                    /* insert new inners */
                    for (j = 0; j < (ssize_t)diff_inners; ++j)
                    {
                        /* merge node */
                        time += pcm_write(index->pcm, index->node_size / 2);

                        /* we need a new pointer in inner node, but inners are sorted so make a gap, or move to new inner */
                        time += pcm_write(index->pcm, index->node_size / 2);

                        /* write down a key with pointer */
                        time += pcm_write(index->pcm, index->key_size + sizeof(void *));
                    }
                }

                break;
            }
            case BTREE_UNSORTED_LEAVES:
            {
                /* we need to write down, new entry at the end */
                time += pcm_write(index->pcm, index->entry_size);

                /* and we need to update bitmap */
                time += pcm_write(index->pcm, 1);

                if (diff_leaves > 0)
                {
                    /* if we need a new leaf, we need to split node, move half of node to another leaf and set bitmap */
                    time += pcm_write(index->pcm, index->node_size / 2);
                    time += pcm_write(index->pcm, 1);

                    /* insert pointer to leaf into inner */
                    time += pcm_write(index->pcm, index->node_size / 2);
                    time += pcm_write(index->pcm, index->key_size + sizeof(void *));

                    /* insert new inners */
                    for (j = 0; j < (ssize_t)diff_inners; ++j)
                    {
                        /* merge node */
                        time += pcm_write(index->pcm, index->node_size / 2);

                        /* we need a new pointer in inner node, but inners are sorted so make a gap, or move to new inner */
                        time += pcm_write(index->pcm, index->node_size / 2);

                        /* write down a key with pointer */
                        time += pcm_write(index->pcm, index->key_size + sizeof(void *));
                    }
                }

                break;
            }
            case BTREE_UNSORTED_INNERS_UNSORTED_LEAVES:
            case BTREE_2SECTION_NODE:
            {
                /* we need to write down, new entry at the end */
                time += pcm_write(index->pcm, index->entry_size);

                /* and we need to update bitmap */
                time += pcm_write(index->pcm, 1);

                if (diff_leaves > 0)
                {
                    /* if we need a new leaf, we need to split node, move half of node to another leaf and set bitmap */
                    time += pcm_write(index->pcm, index->node_size / 2);
                    time += pcm_write(index->pcm, 1);

                    /* write down key + pointer to the new leaf */
                    time += pcm_write(index->pcm, index->key_size + sizeof(void *));
                    time += pcm_write(index->pcm, 1);

                    for (j = 0; j < (ssize_t)diff_inners; ++j)
                    {
                        /* merge node */
                        time += pcm_write(index->pcm, index->node_size / 2);

                        /* write down a key with pointer */
                        time += pcm_write(index->pcm, index->key_size + sizeof(void *));

                        /* and we need to update bitmap */
                        time += pcm_write(index->pcm, 1);
                    }
                }
                break;
            }
            case CBTREE:
            {
                /* insert in sorted order to leaf, so we need to move in avg half node */
                time += pcm_write(index->pcm, index->node_size / 2);

                /* now we can insert entry */
                time += pcm_write(index->pcm, index->entry_size);

                index->buffered_operation += diff_leaves;
                while (index->buffered_operation >= CBTREE_OPERATION_BUFFER_SIZE)
                {
                    /* merge node */
                    time += pcm_write(index->pcm, index->node_size / 2);

                    /* insert */
                    time += pcm_write(index->pcm, index->node_size / 2);
                    time += pcm_write(index->pcm, index->key_size + sizeof(void *));

                    index->buffered_operation -= CBTREE_OPERATION_BUFFER_SIZE;
                }

                /* inners are not buffered */
                for (j = 0; j < (ssize_t)diff_inners; ++j)
                {
                    /* merge node */
                    time += pcm_write(index->pcm, index->node_size / 2);

                    /* we need a new pointer in inner node, but inners are sorted so make a gap, or move to new inner */
                    time += pcm_write(index->pcm, index->node_size / 2);
                    /* write down a key with pointer */
                    time += pcm_write(index->pcm, index->key_size + sizeof(void *));
                }

                break;
            }
            case OCBTREE:
            {
                /* insert in sorted order to leaf, so we need to move in avg half node */
                time += pcm_write(index->pcm, index->node_size / 2);

                /* now we can insert entry */
                time += pcm_write(index->pcm, index->entry_size);

                /* inners are also buffered */
                index->buffered_operation += diff_leaves + diff_inners;
                while (index->buffered_operation >= OCBTREE_OPERATION_BUFFER_SIZE)
                {
                    /* insert */
                    time += pcm_write(index->pcm, index->node_size / 2);
                    time += pcm_write(index->pcm, index->key_size + sizeof(void *));

                    index->buffered_operation -= OCBTREE_OPERATION_BUFFER_SIZE;
                }

                break;
            }
            case BTREE_NORMAL_INNERS_RAM:
            case CBTREE_INNERS_RAM:
            {
                /* delete from sorted order to leaf, so we need to move in avg half node */
                time += pcm_write(index->pcm, index->node_size / 2);

                /* now we can insert entry */
                time += pcm_write(index->pcm, index->entry_size);
                break;
            }
            case BTREE_UNSORTED_LEAVES_INNERS_RAM:
            case BTREE_2SECTION_NODE_INNERS_RAM:
            {
                /* we need to delete it down, new entry at the end */
                time += pcm_write(index->pcm, index->entry_size);

                /* and we need to update bitmap */
                time += pcm_write(index->pcm, 1);
                break;
            }
            case BTREE_SKIP_COST:
                break;
            default:
                break;
        }
    }

    index->height = db_index_get_height(index);
    db_stat_update_index_time(time);
    return time;
}

double db_index_update(DB_index *index, size_t entries)
{
    double time = 0.0;

    TRACE();

    /* delete old value */
    time += db_index_delete(index, entries);

    /* insert enrty with new value */
    time += db_index_insert(index, entries);

    db_stat_update_index_time(time);

    return time;
}