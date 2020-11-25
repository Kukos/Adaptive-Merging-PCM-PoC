#include <log.h>
#include <common.h>
#include <experiments.h>

___before_main___(0) void init(void);
___after_main___(0) void deinit(void);

___before_main___(0) void init(void)
{
	(void)log_init(stdout, LOG_TO_FILE);
}

___after_main___(0) void deinit(void)
{
	log_deinit();
}

int main(void)
{
    // db_raw_experiment_workload(1000);
    // db_index_experiment_workload(1000);
    // db_am_experiment_workload(1000000);
    // db_pam_experiment_workload(1000000);

    struct
    {
        size_t key_size;
        size_t data_size;
        size_t entries;
    }
    table = {.key_size = sizeof(int), .data_size = sizeof(void*), .entries = 100000000};

    const double selectivity = 0.05;

    experiment1("ex1", table.key_size, table.data_size, table.entries, QUERY_RANDOM, selectivity);
    experiment2("ex2", table.key_size, table.data_size, table.entries, QUERY_RANDOM, selectivity);
    experiment3("ex3_random", table.key_size, table.data_size, table.entries, QUERY_RANDOM, selectivity);
    experiment3("ex3_newkeys", table.key_size, table.data_size, table.entries, QUERY_ALWAYS_NEW, selectivity);
    experiment3("ex3_seq", table.key_size, table.data_size, table.entries, QUERY_SEQUENTIAL_PATTERN, selectivity);

    experiment_stress("ex4_stress1",
                      table.key_size,
                      table.data_size,
                      table.entries,
                      &(StressBatch){.inserts = 500, .deletes = 500, .rsearches = 1000, .selectivity_min = 0.01},
                      20);

    experiment_stress("ex4_stress2",
                      table.key_size,
                      table.data_size,
                      table.entries,
                      &(StressBatch){.inserts = 5, .deletes = 5, .rsearches = 10, .selectivity_min = 0.01},
                      100);

    experiment_stress("ex4_stress3",
                      table.key_size,
                      table.data_size,
                      table.entries,
                      &(StressBatch){.inserts = 10, .deletes = 10, .rsearches = 80, .selectivity_min = 0.01},
                      40);

    experiment_stress("ex4_stress4",
                      table.key_size,
                      table.data_size,
                      table.entries,
                      &(StressBatch){.inserts = 100000, .deletes = 100000, .rsearches = 5, .selectivity_min = 0.01},
                      5);

    const double sel_min  = 0.01;
    const double sel_max  = 0.05;
    const double sel_step = 0.01;
    experiment_stress_step("ex4_stress_step1",
                           table.key_size,
                           table.data_size,
                           table.entries,
                           &(StressBatch){.inserts = 500, .deletes = 500, .rsearches = 1000, .selectivity_min = sel_min, .selectivity_max = sel_max, .selectivity_step = sel_step},
                           20);

    experiment_stress_step("ex4_stress_step2",
                           table.key_size,
                           table.data_size,
                           table.entries,
                           &(StressBatch){.inserts = 5, .deletes = 5, .rsearches = 10, .selectivity_min = sel_min, .selectivity_max = sel_max, .selectivity_step = sel_step},
                           100);

    experiment_stress_step("ex4_stress_step3",
                           table.key_size,
                           table.data_size,
                           table.entries,
                           &(StressBatch){.inserts = 10, .deletes = 10, .rsearches = 80, .selectivity_min = sel_min, .selectivity_max = sel_max, .selectivity_step = sel_step},
                           40);

    experiment_stress_step("ex4_stress_step4",
                           table.key_size,
                           table.data_size,
                           table.entries,
                           &(StressBatch){.inserts = 100000, .deletes = 100000, .rsearches = 5, .selectivity_min = sel_min, .selectivity_max = sel_max, .selectivity_step = sel_step},
                           5);

    return 0;
}