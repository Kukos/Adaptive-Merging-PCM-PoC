#include <dbam.h>
#include <experiments.h>
#include <log.h>
#include <dbstat.h>
#include <common.h>
#include <time.h>
#include <stdlib.h>
#include <genrand.h>

void db_am_experiment_workload(size_t entries)
{
    DB_AM *am;
    PCM *pcm;
    size_t i;

    TRACE();

    sgenrand((unsigned long)time(NULL));

    pcm = pcm_create_default_model();
    am = db_am_create(pcm, entries, sizeof(int), 140, (size_t)(0.01 * (double)entries * 140), 10 * 140, INVALIDATION_FLAG);
    db_stat_reset();

    db_stat_start_query();
    db_am_search(am, QUERY_RANDOM, 100);
    db_stat_finish_query();

    for (i = 0; i < 20; ++i)
    {
        db_stat_start_query();
        db_am_search(am, QUERY_RANDOM, (size_t)((double)entries * 0.05));
        //db_stat_current_print();
        db_stat_finish_query();
    }

    db_stat_summary_print();

    db_am_destroy(am);
    pcm_destroy(pcm);
}