#include <pcm.h>
#include <log.h>
#include <common.h>
#include <stdlib.h>

PCM *pcm_create(size_t mem_line, double rtime, double wtime)
{
    PCM *pcm;

    TRACE();

    pcm = malloc(sizeof(PCM));
    if (pcm == NULL)
        ERROR("Malloc error\n", NULL);

    pcm->mem_line = mem_line;
    pcm->read_time = rtime;
    pcm->write_time = wtime;
    pcm->wearout = 0;

    return pcm;
}

void pcm_destroy(PCM *pcm)
{
    TRACE();

    FREE(pcm);
}