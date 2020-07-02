#include <dbam.h>
#include <dbraw.h>
#include <dbpam.h>
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
    const char * const invalidation_names[] = {"Flag", "Bitmap", "Journal"};

    TRACE();

    sgenrand((unsigned long)time(NULL));

    snprintf(file_name, sizeof(file_name), "%s_invalidation.txt", file);
    fd = open(file_name, O_CREAT | O_TRUNC | O_RDWR | O_APPEND, 0644);
    dprintf(fd, "TYPE\tTime\tPCM Wear-out\n");

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

    const btree_type_t btree_type[] = {BTREE_NORMAL, BTREE_UNSORTED_LEAVES, BTREE_UNSORTED_INNERS_UNSORTED_LEAVES, CBTREE, CBTREE_UNSORTED_INNSERS};
    const char * const btree_names[] = {"B+Tree", "Uns leaves", "Uns nodes",  "CBTree", "CBTree-unsorted-inners"};

    TRACE();

    sgenrand((unsigned long)time(NULL));

    snprintf(file_name, sizeof(file_name), "%s_BTREE.txt", file);
    fd = open(file_name, O_CREAT | O_TRUNC | O_RDWR | O_APPEND, 0644);
    dprintf(fd, "TYPE\tTime\tPCM Wear-out\n");

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

void experiment3(const char * const file, size_t key_size, size_t data_size, size_t entries, query_t type, double selectivity)
{
    PCM *pcm_am;
    PCM *pcm_index;
    PCM *pcm_raw;
    PCM *pcm_pam;

    DB_index *index;
    DB_raw *raw;
    DB_PAM *pam;
    DB_AM *am;

    size_t i;

    int fd;
    double raw_time;
    double index_time;
    double pam_time;
    double am_time;

    double total_am_time = 0.0;
    double total_pam_time = 0.0;

    char query_file_name[1024];
    char total_file_name[1024];

    const size_t node_size = 4000;
    const double node_factor = 0.8;
    const size_t buffer_size = (size_t)(0.01 * (double)entries * (double)data_size);

    TRACE();

    pcm_raw = pcm_create_default_model();
    pcm_index = pcm_create_default_model();
    pcm_am = pcm_create_default_model();
    pcm_pam = pcm_create_default_model();

    index = db_index_create(pcm_index, key_size, data_size, node_size, node_factor, BTREE_NORMAL);
    raw = db_raw_create(pcm_raw, data_size);
    am = db_am_create(pcm_am, entries, key_size, data_size, buffer_size, node_size, INVALIDATION_FLAG);
    pam = db_pam_create(pcm_pam, entries, key_size, data_size, buffer_size, node_size);

    db_index_bulkload(index, entries);
    db_raw_bulkload(raw, entries);

    /* time for search from index */
    db_stat_reset();
    db_stat_start_query();
    db_index_range_search(index, (size_t)((double)entries * selectivity));
    db_stat_finish_query();
    index_time = db_stat_get_current_time();
    db_stat_summary_print();

    /* time for search from raw */
    db_stat_reset();
    db_stat_start_query();
    db_raw_range_search(raw, (size_t)((double)entries * selectivity));
    db_stat_finish_query();
    raw_time = db_stat_get_current_time();
    db_stat_summary_print();

    db_pam_search(pam, type, 1);
    db_am_search(am, type, 1);
    db_stat_reset();
    snprintf(query_file_name, sizeof(query_file_name), "%s_per_query.txt", file);
    fd = open(query_file_name, O_CREAT | O_TRUNC | O_RDWR | O_APPEND, 0644);
    dprintf(fd, "Query\tIndex\tScan\tPAM\tAM\n");
    i = 0;
    do
    {
        db_stat_start_query();
        db_pam_search(pam, type, (size_t)((double)entries * selectivity));
        db_stat_finish_query();
        pam_time = db_stat_get_current_time();
        total_pam_time += pam_time;

        db_stat_start_query();
        db_am_search(am, type, (size_t)((double)entries * selectivity));
        db_stat_finish_query();
        am_time = db_stat_get_current_time();
        total_am_time += am_time;


        dprintf(fd, "%zu\t%lf\t%lf\t%lf\t%lf\n", i, index_time, raw_time, pam_time, am_time);
        printf("QUERY %zu:\tENTRIES  (%zu/%zu)\n", i, am->index->num_entries, am->index->num_entries + am->num_entries_in_partitions);

        ++i;
    } while (am->num_entries_in_partitions > 0 || pam->am->num_entries_in_partitions > 0);

    printf("AFTER %zu queries we have full index\n", i);
    db_stat_summary_print();

    db_index_destroy(index);
    db_raw_destroy(raw);
    db_pam_destroy(pam);
    db_am_destroy(am);
    close(fd);

    snprintf(total_file_name, sizeof(total_file_name), "%s_total.txt", file);
    fd = open(total_file_name, O_CREAT | O_TRUNC | O_RDWR | O_APPEND, 0644);
    dprintf(fd, "Type\tTime\tPCM Wear-out\n");
    dprintf(fd, "AM\t%lf\t%zu\n", total_am_time, pcm_am->wearout);
    dprintf(fd, "PAM\t%lf\t%zu\n", total_pam_time, pcm_pam->wearout);
    close(fd);

    pcm_destroy(pcm_index);
    pcm_destroy(pcm_raw);
    pcm_destroy(pcm_am);
    pcm_destroy(pcm_pam);
}