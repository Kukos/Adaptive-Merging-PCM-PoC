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
        am = db_am_create(pcm, entries, key_size, data_size, (size_t)(0.01 * (double)entries * (double)data_size), 4000, invalidation_type[i], BTREE_SKIP_COST);

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
        am = db_am_create(pcm, entries, key_size, data_size, (size_t)(0.01 * (double)entries * (double)data_size), 4000, INVALIDATION_SKIP, btree_type[i]);

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
    PCM *pcm_eam;
    PCM *pcm_index;
    PCM *pcm_raw;
    PCM *pcm_pam;

    DB_index *index;
    DB_raw *raw;
    DB_PAM *pam;
    DB_AM *am;
    DB_AM* eam;

    size_t i;

    int fd;
    double raw_time;
    double index_time;
    double pam_time;
    double am_time;
    double eam_time;

    double total_am_time = 0.0;
    double total_eam_time = 0.0;
    double total_pam_time = 0.0;

    char query_file_name[1024];
    char total_file_name[1024];

    const size_t node_size = 512;
    const double node_factor = 0.8;
    const size_t buffer_size = (size_t)(0.01 * (double)entries * (double)data_size);

    TRACE();

    pcm_raw = pcm_create_default_model();
    pcm_index = pcm_create_default_model();
    pcm_am = pcm_create_default_model();
    pcm_eam = pcm_create_default_model();
    pcm_pam = pcm_create_default_model();

    index = db_index_create(pcm_index, key_size, data_size, node_size, node_factor, BTREE_NORMAL);
    raw = db_raw_create(pcm_raw, data_size);
    am = db_am_create(pcm_am, entries, key_size, data_size, buffer_size, node_size, INVALIDATION_OVERWRITE, BTREE_NORMAL);
    eam = db_am_create(pcm_eam, entries, key_size, data_size, buffer_size, node_size, INVALIDATION_BITMAP, BTREE_UNSORTED_LEAVES);
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
    db_am_search(eam, type, 1);

    /* skip cost of init */
    pcm_am->wearout = 0;
    pcm_eam->wearout = 0;
    pcm_pam->wearout = 0;

    db_stat_reset();
    snprintf(query_file_name, sizeof(query_file_name), "%s_per_query.txt", file);
    fd = open(query_file_name, O_CREAT | O_TRUNC | O_RDWR | O_APPEND, 0644);
    dprintf(fd, "Query\tIndex\tScan\tPAM\tAM\teAM\n");
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

        db_stat_start_query();
        db_am_search(eam, type, (size_t)((double)entries * selectivity));
        db_stat_finish_query();
        eam_time = db_stat_get_current_time();
        total_eam_time += eam_time;

        dprintf(fd, "%zu\t%lf\t%lf\t%lf\t%lf\t%lf\n", i + 1, index_time, raw_time, pam_time, am_time, eam_time);
        printf("QUERY %zu:\tENTRIES  (%zu/%zu)\n", i + 1, am->index->num_entries, am->index->num_entries + am->num_entries_in_partitions);

        ++i;
    } while (am->num_entries_in_partitions > 0 || pam->am->num_entries_in_partitions > 0 || eam->num_entries_in_partitions > 0);

    printf("AFTER %zu queries we have full index\n", i);
    db_stat_summary_print();

    db_index_destroy(index);
    db_raw_destroy(raw);
    db_pam_destroy(pam);
    db_am_destroy(am);
    db_am_destroy(eam);
    close(fd);

    snprintf(total_file_name, sizeof(total_file_name), "%s_total.txt", file);
    fd = open(total_file_name, O_CREAT | O_TRUNC | O_RDWR | O_APPEND, 0644);
    dprintf(fd, "Type\tTime\tPCM Wear-out\n");
    dprintf(fd, "AM\t%lf\t%zu\n", total_am_time, pcm_am->wearout);
    dprintf(fd, "eAM\t%lf\t%zu\n", total_eam_time, pcm_eam->wearout);
    dprintf(fd, "PAM\t%lf\t%zu\n", total_pam_time, pcm_pam->wearout);
    close(fd);

    pcm_destroy(pcm_index);
    pcm_destroy(pcm_raw);
    pcm_destroy(pcm_am);
    pcm_destroy(pcm_eam);
    pcm_destroy(pcm_pam);
}

void experiment_stress(const char* file, size_t key_size, size_t data_size, size_t entries, StressBatch* batch, size_t batches)
{
    PCM *pcm_am;
    PCM *pcm_eam;
    PCM *pcm_pam;

    DB_PAM *pam;
    DB_AM *am;
    DB_AM *eam;

    int fd;
    double pam_time;
    double am_time;
    double eam_time;

    double total_am_time = 0.0;
    double total_eam_time = 0.0;
    double total_pam_time = 0.0;

    char total_file_name[1024];
    char query_file_name[1024];

    const size_t node_size = 512;
    const size_t buffer_size = (size_t)(0.01 * (double)entries * (double)data_size);

    TRACE();

    pcm_am = pcm_create_default_model();
    pcm_eam = pcm_create_default_model();
    pcm_pam = pcm_create_default_model();

    am = db_am_create(pcm_am, entries, key_size, data_size, buffer_size, node_size, INVALIDATION_OVERWRITE, BTREE_NORMAL);
    eam = db_am_create(pcm_eam, entries, key_size, data_size, buffer_size, node_size, INVALIDATION_BITMAP, BTREE_UNSORTED_LEAVES);
    pam = db_pam_create(pcm_pam, entries, key_size, data_size, buffer_size, node_size);

    db_pam_search(pam, QUERY_RANDOM, 1);
    db_am_search(am, QUERY_RANDOM, 1);
    db_am_search(eam, QUERY_RANDOM, 1);

    /* Lets skip cost of init */
    pcm_am->wearout = 0;
    pcm_eam->wearout = 0;
    pcm_pam->wearout = 0;
    db_stat_reset();

    snprintf(query_file_name, sizeof(query_file_name), "%s_per_batch.txt", file);
    fd = open(query_file_name, O_CREAT | O_TRUNC | O_RDWR | O_APPEND, 0644);
    dprintf(fd, "Query\tPAM\tAM\teAM\n");
    for (size_t i = 0; i < batches; ++i)
    {
        printf("%s: BATCH %zu/%zu\n", file, (i + 1), batches);

        /* PAM BATCH */
        db_stat_start_query();
        for (size_t q = 0; q < batch->rsearches; ++q)
            db_pam_search(pam, QUERY_RANDOM, (size_t)((double)entries * batch->selectivity_min));

        for (size_t q = 0; q < batch->inserts; ++q)
            db_pam_insert(pam, 1);

        for (size_t q = 0; q < batch->deletes; ++q)
            db_pam_delete(pam, 1);

        db_stat_finish_query();
        pam_time = db_stat_get_current_time();
        total_pam_time += pam_time;

        /* AM BATCH */
        db_stat_start_query();
        for (size_t q = 0; q < batch->rsearches; ++q)
            db_am_search(am, QUERY_RANDOM, (size_t)((double)entries * batch->selectivity_min));

        for (size_t q = 0; q < batch->inserts; ++q)
            db_am_insert(am, 1);

        for (size_t q = 0; q < batch->deletes; ++q)
            db_am_delete(am, 1);

        db_stat_finish_query();
        am_time = db_stat_get_current_time();
        total_am_time += am_time;

        /* eAM BATCH */
        db_stat_start_query();
        for (size_t q = 0; q < batch->rsearches; ++q)
            db_am_search(eam, QUERY_RANDOM, (size_t)((double)entries * batch->selectivity_min));

        for (size_t q = 0; q < batch->inserts; ++q)
            db_am_insert(eam, 1);

        for (size_t q = 0; q < batch->deletes; ++q)
            db_am_delete(eam, 1);

        db_stat_finish_query();
        eam_time = db_stat_get_current_time();
        total_eam_time += eam_time;

        dprintf(fd, "%zu\t%lf\t%lf\t%lf\n", (i + 1), pam_time, am_time, eam_time);
    }

    db_stat_summary_print();

    db_pam_destroy(pam);
    db_am_destroy(am);
    db_am_destroy(eam);

    snprintf(total_file_name, sizeof(total_file_name), "%s_total.txt", file);
    fd = open(total_file_name, O_CREAT | O_TRUNC | O_RDWR | O_APPEND, 0644);
    dprintf(fd, "Type\tTime\tPCM Wear-out\n");
    dprintf(fd, "AM\t%lf\t%zu\n", total_am_time, pcm_am->wearout);
    dprintf(fd, "eAM\t%lf\t%zu\n", total_eam_time, pcm_eam->wearout);
    dprintf(fd, "PAM\t%lf\t%zu\n", total_pam_time, pcm_pam->wearout);
    close(fd);

    /* Normalize time and wearout to PAM */
    snprintf(total_file_name, sizeof(total_file_name), "%s_total_norma.txt", file);
    fd = open(total_file_name, O_CREAT | O_TRUNC | O_RDWR | O_APPEND, 0644);
    dprintf(fd, "Type\tTime\tPCM Wear-out\n");
    dprintf(fd, "AM\t%Lf\t%Lf\n", (long double)total_am_time / (long double)total_pam_time, (long double)pcm_am->wearout / (long double)pcm_pam->wearout);
    dprintf(fd, "eAM\t%Lf\t%Lf\n", (long double)total_eam_time / (long double)total_pam_time, (long double)pcm_eam->wearout / (long double)pcm_pam->wearout);
    dprintf(fd, "PAM\t%Lf\t%Lf\n", (long double)total_pam_time / (long double)total_pam_time, (long double)pcm_pam->wearout / (long double)pcm_pam->wearout);
    close(fd);

    pcm_destroy(pcm_am);
    pcm_destroy(pcm_eam);
    pcm_destroy(pcm_pam);
}


void experiment_stress_step(const char* file, size_t key_size, size_t data_size, size_t entries, StressBatch* batch, size_t batches)
{
    char total_file_name[1024];
    char total_norma_file_name[1024];
    int fd_total;
    int fd_norma;

    snprintf(total_file_name, sizeof(total_file_name), "%s_total.txt", file);
    fd_total = open(total_file_name, O_CREAT | O_TRUNC | O_RDWR | O_APPEND, 0644);
    dprintf(fd_total, "Selectivity\tPAM\tAM\teAM\n");

    snprintf(total_norma_file_name, sizeof(total_norma_file_name), "%s_total_norma.txt", file);
    fd_norma = open(total_norma_file_name, O_CREAT | O_TRUNC | O_RDWR | O_APPEND, 0644);
    dprintf(fd_norma, "Selectivity\tPAM\tAM\teAM\n");

    for (double sel = batch->selectivity_min; sel <= batch->selectivity_max + 0.001; sel += batch->selectivity_step)
    {
        PCM *pcm_am;
        PCM *pcm_eam;
        PCM *pcm_pam;

        DB_PAM *pam;
        DB_AM *am;
        DB_AM *eam;

        double pam_time;
        double am_time;
        double eam_time;

        double total_am_time = 0.0;
        double total_eam_time = 0.0;
        double total_pam_time = 0.0;

        const size_t node_size = 512;
        const size_t buffer_size = (size_t)(0.01 * (double)entries * (double)data_size);

        TRACE();

        pcm_am = pcm_create_default_model();
        pcm_eam = pcm_create_default_model();
        pcm_pam = pcm_create_default_model();

        am = db_am_create(pcm_am, entries, key_size, data_size, buffer_size, node_size, INVALIDATION_OVERWRITE, BTREE_NORMAL);
        eam = db_am_create(pcm_eam, entries, key_size, data_size, buffer_size, node_size, INVALIDATION_BITMAP, BTREE_UNSORTED_LEAVES);
        pam = db_pam_create(pcm_pam, entries, key_size, data_size, buffer_size, node_size);

        db_pam_search(pam, QUERY_RANDOM, 1);
        db_am_search(am, QUERY_RANDOM, 1);
        db_am_search(eam, QUERY_RANDOM, 1);

        /* Lets skip cost of init */
        pcm_am->wearout = 0;
        pcm_eam->wearout = 0;
        pcm_pam->wearout = 0;
        db_stat_reset();

        for (size_t i = 0; i < batches; ++i)
        {
            printf("%s: SEL: %.4lf/%.4lf: BATCH %zu/%zu\n", file, sel, batch->selectivity_max,  (i + 1), batches);

            /* PAM BATCH */
            db_stat_start_query();
            for (size_t q = 0; q < batch->rsearches; ++q)
                db_pam_search(pam, QUERY_RANDOM, (size_t)((double)entries * sel));

            for (size_t q = 0; q < batch->inserts; ++q)
                db_pam_insert(pam, 1);

            for (size_t q = 0; q < batch->deletes; ++q)
                db_pam_delete(pam, 1);

            db_stat_finish_query();
            pam_time = db_stat_get_current_time();
            total_pam_time += pam_time;

            /* AM BATCH */
            db_stat_start_query();
            for (size_t q = 0; q < batch->rsearches; ++q)
                db_am_search(am, QUERY_RANDOM, (size_t)((double)entries * sel));

            for (size_t q = 0; q < batch->inserts; ++q)
                db_am_insert(am, 1);

            for (size_t q = 0; q < batch->deletes; ++q)
                db_am_delete(am, 1);

            db_stat_finish_query();
            am_time = db_stat_get_current_time();
            total_am_time += am_time;

            /* eAM BATCH */
            db_stat_start_query();
            for (size_t q = 0; q < batch->rsearches; ++q)
                db_am_search(eam, QUERY_RANDOM, (size_t)((double)entries * sel));

            for (size_t q = 0; q < batch->inserts; ++q)
                db_am_insert(eam, 1);

            for (size_t q = 0; q < batch->deletes; ++q)
                db_am_delete(eam, 1);

            db_stat_finish_query();
            eam_time = db_stat_get_current_time();
            total_eam_time += eam_time;
        }

        // db_stat_summary_print();

        db_pam_destroy(pam);
        db_am_destroy(am);
        db_am_destroy(eam);

        dprintf(fd_total, "%.4lf\t%lf\t%lf\t%lf\n", sel, total_pam_time, total_am_time, total_eam_time);

        /* Normalize time to PAM */
        dprintf(fd_norma, "%.4lf\t%Lf\t%Lf\t%Lf\n", sel, (long double)total_pam_time / (long double)total_pam_time, (long double)total_am_time / (long double)total_pam_time, (long double)total_eam_time / (long double)total_pam_time);

        pcm_destroy(pcm_am);
        pcm_destroy(pcm_eam);
        pcm_destroy(pcm_pam);
    }

    close(fd_total);
    close(fd_norma);
}