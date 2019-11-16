#include <log.h>
#include <dbstat.h>
#include <common.h>
#include <string.h>

DB_snapshot db_current_query;
DB_snapshot db_total;

/*
    Print on stdout info about snapshot

    PARAMS
    @IN sh - pointer to snapshot

    RETURN
    This is a void function
*/
static ___inline___ void __db_stat_print(DB_snapshot *sh);

static ___inline___ void __db_stat_print(DB_snapshot *sh)
{
    printf("\tINVALIDATION  TIME     = %lfs\n", sh->invalidation_time);
    printf("\tINDEX         TIME     = %lfs\n", sh->index_time);
    printf("\tMISC          TIME     = %lfs\n", sh->misc_time);
    printf("\tTOTAL         TIME     = %lfs\n", __db_stat_get_time(sh));
}

void db_stat_reset(void)
{
    TRACE();

    LOG("Reseting db statistics\n");
    (void)memset(&db_current_query, 0, sizeof(db_current_query));
    (void)memset(&db_total, 0, sizeof(db_total));
}

void db_stat_reset_query(void)
{
    TRACE();

    LOG("Reseting only current query stat\n");
    (void)memset(&db_current_query, 0, sizeof(db_current_query));
}

void db_stat_start_query(void)
{
    TRACE();

    LOG("Start new query\n");
    db_stat_reset_query();
}

void db_stat_finish_query(void)
{
    TRACE();

    LOG("Finishing query\n");

    /* update total */
    db_total.invalidation_time += db_current_query.invalidation_time;
    db_total.index_time += db_current_query.index_time;
    db_total.misc_time += db_current_query.misc_time;
}

void db_stat_current_print(void)
{
    printf("CURRENT QUERY\n\n");
    __db_stat_print(&db_current_query);
}

void db_stat_summary_print(void)
{
    printf("TOTAL\n\n");
    __db_stat_print(&db_total);
}