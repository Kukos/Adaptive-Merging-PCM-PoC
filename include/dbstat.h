#ifndef DBSTAT_H
#define DBSTAT_H

/*
    Data base statistics included total time consumptions, current query time

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE GPL 3.0
*/

#include <compiler.h>
#include <stddef.h>
#include <sys/types.h>

typedef struct DB_snapshot
{
    /* time in seconds */
    double invalidation_time; /* data invalidation in partition */
    double index_time; /* index search / insert / delete time */
    double misc_time; /* others */
} DB_snapshot;

extern DB_snapshot db_current_query;
extern DB_snapshot db_total;

/*
    Private function, do not use directly

    PARAMS
    @IN sh - pointer to snapshot

    RETURN
    Sum of all fields related to time
*/
static ___inline___ double __db_stat_get_time(DB_snapshot *sh);


static ___inline___ double __db_stat_get_time(DB_snapshot *sh)
{
    return  sh->index_time + sh->invalidation_time + sh->misc_time;
}

/*
    Reset whole DB Stat

    PARAMS
    NO PARAMS

    RETURN
    This is a void function
*/
void db_stat_reset(void);

/*
    Reset only current query statictics

    PARAMS
    NO PARAMS

    RETURN
    This is a void function
*/
void db_stat_reset_query(void);

/*
    Start new query

    PARAMS
    NO PARAMS

    RETURN
    This is a void function
*/
void db_stat_start_query(void);

/*
    Finish current query and update total statistics

    PARAMS
    NO PARAMS

    RETURN
    This is a void function
*/
void db_stat_finish_query(void);

/*
    Print on stdout info about current query

    PARAMS
    NO PARAMS

    RETURN
    This is a void function
*/
void db_stat_current_print(void);

/*
    Print on stdout summary info

    PARAMS
    NO PARAMS

    RETURN
    This is a void function
*/
void db_stat_summary_print(void);

/*
    Update statistics  for current query

    PARAMS
    @IN s - seconds

    RETURN
    This is a void function
*/
static ___inline___ void db_stat_update_invalidation_time(double s);
static ___inline___ void db_stat_update_index_time(double s);
static ___inline___ void db_stat_update_misc_time(double s);

/*
    Get query time  current query and total time

    PARAMS
    NO PARAMS

    RETURN
    Query time
*/
static ___inline___ double db_stat_get_current_time(void);
static ___inline___ double db_stat_get_total_time(void);


static ___inline___ void db_stat_update_invalidation_time(double s)
{
    db_current_query.invalidation_time += s;
}

static ___inline___ void db_stat_update_index_time(double s)
{
    db_current_query.index_time += s;
}

static ___inline___ void db_stat_update_misc_time(double s)
{
    db_current_query.misc_time += s;
}

static ___inline___ double db_stat_get_current_time(void)
{
    return __db_stat_get_time(&db_current_query);
}

static ___inline___ double db_stat_get_total_time(void)
{
    return __db_stat_get_time(&db_total);
}

#endif
