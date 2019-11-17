#include <dbindex.h>
#include <experiments.h>
#include <log.h>
#include <dbstat.h>
#include <math.h>
#include <common.h>

void db_index_experiment_workload(size_t queries)
{
    DB_index *index;
    PCM *pcm;
    size_t i;
    double _sqrt_n = ceil(sqrt((double)queries));
    size_t sqrt_n = (size_t)_sqrt_n;
    size_t nlogn = queries * LOG2(queries);

    TRACE();

    pcm = pcm_create_default_model();
    index = db_index_create(pcm, sizeof(long), 140, 1000, 0.8, BTREE_NORMAL);
    db_stat_reset();

    /* bukload N / 2 */
    db_stat_start_query();
    db_index_bulkload(index, queries / 2);
    db_stat_finish_query();

    /* sqrt(N) point search */
    for (i = 0; i < sqrt_n; ++i)
    {
        db_stat_start_query();
        db_index_point_search(index, 1);
        db_stat_finish_query();
    }

    /* Normal Insert N / 2 */
    for (i = 0; i < queries / 2; ++i)
    {
        db_stat_start_query();
        db_index_insert(index, 1);
        db_stat_finish_query();
    }

    /* NlogN range search with 10% selecivity */
    for (i = 0; i < nlogn; ++i)
    {
        db_stat_start_query();
        db_index_range_search(index, (index->num_entries + 9) / 10);
        db_stat_finish_query();
    }

    /* sqrt(N) delete */
    for (i = 0; i < sqrt_n; ++i)
    {
        db_stat_start_query();
        db_index_delete(index, 1);
        db_stat_finish_query();
    }

    /* sqrt(N) point search */
    for (i = 0; i < sqrt_n; ++i)
    {
        db_stat_start_query();
        db_index_point_search(index, 1);
        db_stat_finish_query();
    }

    /* sqrt(N) update */
    for (i = 0; i < sqrt_n; ++i)
    {
        db_stat_start_query();
        db_index_update(index, 1);
        db_stat_finish_query();
    }

    /* NlogN range search with 5% selectivity */
    for (i = 0; i < nlogn; ++i)
    {
        db_stat_start_query();
        db_index_range_search(index, (index->num_entries + 19) / 20);
        db_stat_finish_query();
    }

    db_stat_summary_print();
    db_index_destroy(index);
    pcm_destroy(pcm);
}