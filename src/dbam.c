#include <compiler.h>
#include <common.h>
#include <stdlib.h>
#include <dbam.h>
#include <log.h>
#include <dbutils.h>
#include <genrand.h>
#include <dbstat.h>
#include <math.h>

#define DB_AM_LOG(n, k) (log(n) / log(k))

/*
    Based on query type calculate number of entries to load from index

    PARAMS
    @IN am - pointer to AM system
    @IN type - query type
    @IN entries - entries needed by query

    RETURN
    Number of entries to load from index
*/
static ___inline___ size_t get_num_entries_from_index(DB_AM *am, query_t type, size_t entries);

/*
    Copy entries from partition to index

    PARAMS
    @IN am - pointer to AM system
    @IN entries - number of entries to copy

    RETURN
    Time consumed by this operation
*/
static ___inline___ double copy_entries_to_index(DB_AM *am, size_t entries);

/*
    Copy all entries from am to index

    PARAMS
    @IN AM - pointer to AM system

    RETURN
    Consumed time
*/
static ___inline___ double db_am_write_all_to_index(DB_AM *am);

/*
    Load entries from index

    PARAMS
    @IN am - pointer to AM system
    @IN entries - number of entries to load

    RETURN
    Time consumed by this operation
*/
static ___inline___ double load_entries_from_index(DB_AM *am, size_t entries);

/*
    Read entries from partitions

    PARAMS
    @IN am - pointer to AM
    @IN entries - number pf entries to load

    RETURN
    Time consumed by loading from partitions
*/
static ___inline___ double load_entries_from_partition(DB_AM *am, size_t entries);

/*
    Init Adaptive Merging System (Call it only once at first query)

    PARAMS
    @IN am - pointer to DB_AM
    @IN entries - number of entries needed for first query

    RETURN
    Time needed for init
*/
static double db_am_init(DB_AM *am, size_t entries);

/*
    Create initial partition set for entries

    PARAMS
    @IN am - pointer to DB_AM
    @IN entries - number of entries
*/
static double create_partitions_for_entries(DB_AM *am, size_t entries);


static ___inline___ size_t get_num_entries_from_index(DB_AM *am, query_t type, size_t entries)
{
    size_t entries_from_index = 0;
    size_t i;
    unsigned long r;

    TRACE();

    switch (type)
    {
        case QUERY_ALWAYS_NEW:
        {
            if (am->num_entries_in_partitions >= entries)
                entries_from_index = 0;
            else
                entries_from_index = entries - am->num_entries_in_partitions;
            break;
        }
        case QUERY_RANDOM:
        {
            entries_from_index = 0;
            for (i = 0; i < entries; ++i)
            {
                r = genrand() % am->num_entries;
                if ((size_t)r >= am->num_entries_in_partitions)
                    ++entries_from_index;
            }
            break;
        }
        case QUERY_SEQUENTIAL_PATTERN:
        {
            if (am->num_entries == am->index->num_entries)
                entries_from_index = entries;
            else
                entries_from_index = entries / 2;

            break;
        }
        default:
        {
            ERROR("Incorrect query type\n", 0);
            break;
        }
    }

    /*
        from_index = distribution based on query
        from_part = entries - from_index

        from_index <= am->index->num_entries
        from_part <= am->set.num_entries

        after combining that we have:

        1. from_index <= am->index->num_entries
        2. from_index >= entries - am->set.num_entries;
    */

    entries_from_index = MIN(entries_from_index, am->index->num_entries);

    if (entries > am->num_entries_in_partitions)
        entries_from_index = MAX(entries_from_index, entries - am->num_entries_in_partitions);

    return entries_from_index;
}

static ___inline___ double db_am_write_all_to_index(DB_AM *am)
{
    double time = 0.0;
    TRACE();

    time += copy_entries_to_index(am, am->num_entries_in_partitions);
    am->num_entries_in_partitions = 0;

    return time;
}

static ___inline___ double load_entries_from_index(DB_AM *am, size_t entries)
{
    if (entries > 0)
        return db_index_range_search(am->index, entries);

    return 0.0;
}

static ___inline___ double copy_entries_to_index(DB_AM *am, size_t entries)
{
    if (entries > 0)
        return db_index_bulkload(am->index, entries);

    return 0.0;
}

static double db_am_init(DB_AM *am, size_t entries)
{
    double total_time = 0.0;
    double time;
    size_t entries_to_sort;

    TRACE();

    /* read entries from table */
    time = pcm_read(am->pcm, am->num_entries * am->entry_size);
    db_stat_update_misc_time(time);
    total_time += time;

    /* create index with entries */
    total_time += db_index_bulkload(am->index, entries);

    /* entries to sort and write into partitions */
    entries_to_sort = am->num_entries - entries;

    /* create new partitions */
    total_time += create_partitions_for_entries(am, entries_to_sort);

    return total_time;
}

static double create_partitions_for_entries(DB_AM *am, size_t entries)
{
    double time = 0.0;

    TRACE();

    am->num_entries_in_partitions += entries;
    am->num_of_partitions = INT_CEIL_DIV(entries * am->entry_size, am->sort_buffer_size);

    time += pcm_write(am->pcm, entries * am->entry_size);
    db_stat_update_misc_time(time);

    return time;
}

static ___inline___ double load_entries_from_partition(DB_AM *am, size_t entries)
{
    double total_time = 0.0;
    double time;

    size_t i;

    TRACE();

    /* seek partitions in logarithmic way */
    for (i = 0; i < am->num_of_partitions; ++i)
    {
        time = pcm_read(am->pcm, (size_t)DB_AM_LOG((double)(am->num_entries_in_partitions / am->num_of_partitions * am->entry_size), 2.0));
        db_stat_update_misc_time(time);
        total_time += time;
    }

    /* load entries */
    time = pcm_read(am->pcm, entries * am->entry_size);
    db_stat_update_misc_time(time);
    total_time += time;

    /* invalid entries */
    switch (am->invalidation_type)
    {
        case INVALIDATION_FLAG:
        {
            /* each entry needs a flag set to invalid */
            for (i = 0; i < entries; ++i)
            {
                time = pcm_write(am->pcm, 1);
                db_stat_update_invalidation_time(time);
                total_time += time;
            }

            break;
        }
        case INVALIDATION_BITMAP:
        {
            /* each partition has a bitmap, also 1 bitmap can store 64 entries */
            time = pcm_write(am->pcm, (am->num_of_partitions + entries / 64) * 8);
            db_stat_update_invalidation_time(time);
            total_time += time;

            break;
        }
        case INVALIDATION_JOURNAL:
        {
            /* we need to write down 2x key into journal */
            time = pcm_write(am->pcm, am->index->key_size * 2);
            db_stat_update_invalidation_time(time);
            total_time += time;

            break;
        }

        default:
            break;
    }

    am->num_entries_in_partitions -= entries;
    return time;
}

DB_AM *db_am_create(PCM *pcm, size_t num_entries, size_t key_size, size_t entry_size, size_t buffer_size, size_t index_node_size, invalidation_type_t invalidation_type)
{
    DB_AM *am;

    TRACE();

    am = malloc(sizeof(DB_AM));
    if (am == NULL)
        ERROR("malloc error\n", NULL);

    am->num_entries = num_entries;
    am->entry_size = entry_size;
    am->pcm = pcm;
    am->sort_buffer_size = buffer_size;
    am->invalidation_type = invalidation_type;
    am->num_entries_in_partitions = 0;
    am->num_of_partitions = 0;

    am->index = db_index_create(pcm, key_size, entry_size, index_node_size, 0.8, BTREE_NORMAL);
    if (am->index == NULL)
    {
        FREE(am);
        ERROR("db_index_create error\n", NULL);
    }

    return am;
}

void db_am_destroy(DB_AM *am)
{
    TRACE();

    if (am == NULL)
        return;

    db_index_destroy(am->index);

    FREE(am);
}

double db_am_search(DB_AM *am, query_t type, size_t entries)
{
    double time = 0.0;
    size_t entries_from_index;
    size_t entries_from_partition;

    TRACE();

    if (am->index->num_entries == 0 && am->num_entries_in_partitions == 0)
    {
        time += db_am_init(am, entries);
        return time;
    }

    entries_from_index = get_num_entries_from_index(am, type, entries);
    entries_from_partition = entries - entries_from_index;

    /* load from index */
    time += load_entries_from_index(am, entries_from_index);

    /* load from partition */
    time += load_entries_from_partition(am, entries_from_partition);

    /* insert loaded entries from partition into index */
    time += copy_entries_to_index(am, entries_from_partition);

    if (am->num_entries_in_partitions * am->entry_size <= am->sort_buffer_size)
        time += db_am_write_all_to_index(am);

    return time;
}