#include <dbam.h>
#include <experiments.h>
#include <log.h>
#include <dbstat.h>
#include <common.h>
#include <time.h>
#include <stdlib.h>
#include <genrand.h>
#include <fcntl.h>
#include <unistd.h>

void experiment1(const char * const file, size_t key_size, size_t data_size, size_t entries, query_t type, double selectivity)
{
    PCM *pcm;
    DB_AM *am;

    size_t query;
    size_t i;

    int fd;
    char file_name[1024];

    const invalidation_type_t invalidation_type[] = {INVALIDATION_FLAG, INVALIDATION_BITMAP, INVALIDATION_JOURNAL};
    const char * const invalidation_names[] = {tostring(INVALIDATION_FLAG), tostring(INVALIDATION_BITMAP), tostring(INVALIDATION_JOURNAL)};

    TRACE();

    sgenrand((unsigned long)time(NULL));

    snprintf(file_name, sizeof(file_name), "%s_invalidation.txt", file);
    fd = open(file_name, O_CREAT | O_TRUNC | O_RDWR | O_APPEND, 0644);
    dprintf(fd, "TYPE\tTIME\tPCM WEAROUT\n");

    for (i = 0; i < ARRAY_SIZE(invalidation_type); ++i)
    {
        pcm = pcm_create_default_model();
        am = db_am_create(pcm, entries, key_size, data_size, (size_t)(0.01 * (double)entries * (double)data_size), 4000, invalidation_type[i]);
        am->index->type = BTREE_SKIP_COST;

        db_stat_reset();
        printf("%s\n", invalidation_names[i]);
        query = 0;
        do
        {
            db_stat_start_query();
            db_am_search(am, type, (size_t)((double)entries * selectivity));
            db_stat_finish_query();

            printf("QUERY %zu:\tENTRIES  (%zu/%zu)\n", query, am->index->num_entries, am->index->num_entries + am->num_entries_in_partitions);
            ++query;
        } while (am->num_entries_in_partitions > 0);

        printf("AFTER %zu queries we have full index\n", query);
        db_stat_summary_print();
        dprintf(fd, "%s\t%lf\t%zu\n", invalidation_names[i], db_total.invalidation_time, pcm->wearout);

        db_am_destroy(am);
        pcm_destroy(pcm);
    }

    close(fd);
}

void experiment2(const char * const file, size_t key_size, size_t data_size, size_t entries, query_t type, double selectivity)
{
    PCM *pcm;
    DB_AM *am;

    size_t query;
    size_t i;

    int fd;
    char file_name[1024];

    const btree_type_t btree_type[] = {BTREE_NORMAL, BTREE_UNSORTED_LEAVES, BTREE_UNSORTED_INNERS_UNSORTED_LEAVES};
    const char * const btree_names[] = {tostring(BTREE_NORMAL), tostring(BTREE_UNSORTED_LEAVES), tostring(BTREE_UNSORTED_INNERS_UNSORTED_LEAVES)};

    TRACE();

    sgenrand((unsigned long)time(NULL));

    snprintf(file_name, sizeof(file_name), "%s_BTREE.txt", file);
    fd = open(file_name, O_CREAT | O_TRUNC | O_RDWR | O_APPEND, 0644);
    dprintf(fd, "TYPE\tTIME\tPCM WEAROUT\n");

    for (i = 0; i < ARRAY_SIZE(btree_type); ++i)
    {
        pcm = pcm_create_default_model();
        am = db_am_create(pcm, entries, key_size, data_size, (size_t)(0.01 * (double)entries * (double)data_size), 4000, INVALIDATION_SKIP);
        am->index->type = btree_type[i];

        db_stat_reset();
        printf("%s\n", btree_names[i]);
        query = 0;
        do
        {
            db_stat_start_query();
            db_am_search(am, type, (size_t)((double)entries * selectivity));
            db_stat_finish_query();

            printf("QUERY %zu:\tENTRIES  (%zu/%zu)\n", query, am->index->num_entries, am->index->num_entries + am->num_entries_in_partitions);
            ++query;
        } while (am->num_entries_in_partitions > 0);

        printf("AFTER %zu queries we have full index\n", query);
        db_stat_summary_print();
        dprintf(fd, "%s\t%lf\t%zu\n", btree_names[i], db_total.index_time, pcm->wearout);

        db_am_destroy(am);
        pcm_destroy(pcm);
    }

    close(fd);
}