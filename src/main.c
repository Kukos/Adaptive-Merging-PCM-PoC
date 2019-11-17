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

    db_raw_experiment_workload(1000);
    db_index_experiment_workload(1000);
    db_am_experiment_workload(1000000);

    return 0;
}