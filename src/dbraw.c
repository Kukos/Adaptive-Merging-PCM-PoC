#include <log.h>
#include <dbraw.h>
#include <dbstat.h>
#include <common.h>
#include <stdlib.h>

DB_raw *db_raw_create(PCM *pcm, size_t entry_size)
{
    DB_raw *raw;

    TRACE();

    raw = (DB_raw *)malloc(sizeof(DB_raw));
    if (raw == NULL)
        ERROR("malloc error\n", NULL);

    raw->entry_size = entry_size;
    raw->pcm = pcm;
    raw->num_entries = 0;

    return raw;
}

void db_raw_destroy(DB_raw *raw)
{
    TRACE();

    if (raw == NULL)
        return;

    FREE(raw);
}

double db_raw_insert(DB_raw *raw, size_t entries)
{
    double time = 0.0;
    size_t i;

    TRACE();

    for (i = 0; i < entries; ++i)
    {
        /* write at the end */
        time += pcm_write(raw->pcm, raw->entry_size);

        ++raw->num_entries;
    }

    db_stat_update_misc_time(time);
    return time;
}

double db_raw_bulkload(DB_raw *raw, size_t entries)
{
    double time = 0.0;

    TRACE();

    /* insert at the end */
    time += pcm_write(raw->pcm, raw->entry_size * entries);
    raw->num_entries += entries;

    db_stat_update_misc_time(time);
    return time;
}

double db_raw_point_search(DB_raw *raw, size_t entries)
{
    double time = 0.0;

    TRACE();

    (void)entries;

    /* data are unsorted, so scan all */
    time += pcm_read(raw->pcm, raw->entry_size * raw->num_entries);

    db_stat_update_misc_time(time);
    return time;
}

double db_raw_range_search(DB_raw *raw, size_t entries)
{
   double time = 0.0;

    TRACE();

    (void)entries;

    /* data are unsorted, so scan all */
    time += pcm_read(raw->pcm, raw->entry_size * raw->num_entries);

    db_stat_update_misc_time(time);
    return time;
}

double db_raw_delete(DB_raw *raw, size_t entries)
{
    double time = 0.0;

    TRACE();

    db_raw_range_search(raw, entries);
    db_raw_insert(raw, entries);

    // becuase insert add 1x entries, we need undo it and note deletion
    raw->num_entries -=  2 * entries;

    db_stat_update_misc_time(time);
    return time;
}

double db_raw_update(DB_raw *raw, size_t entries)
{
    double time = 0.0;

    TRACE();

    db_raw_range_search(raw, entries);
    db_raw_insert(raw, entries);

    // becuase insert add 1x entries, we need undo it
    raw->num_entries -= entries;

    db_stat_update_misc_time(time);
    return time;
}