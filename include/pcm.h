#ifndef PCM_H
#define PCM_H

/*
    Simulator of Phase Change Memory

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE: GPL 3.0
*/

#include <compiler.h>
#include <stddef.h>
#include <dbutils.h>

typedef struct PCM
{
    size_t mem_line; /* minimum unit of read and write, like page in flash */

    /* time in seconds per mem_line*/
    double read_time;
    double write_time;

    /* global wearout of memory (in bytes) */
    size_t wearout;
} PCM;

#define NANO(x)  ((double)x / (double)1000000000)
#define MICRO(x) ((double)x / (double)1000000)

/* from Rethinking Database Algorithms for PCM */
#define pcm_create_default_model() pcm_create(64, NANO(50), MICRO(1))

/*
    Create a PCM instance

    PARAMS
    @IN mem_line - memory line (minimum unit of read and write, like page)
    @IN rtime - read time in seconds per mem_line
    @IN wtime - write time in seconds per mem_line

    RETURN
    Pointer to new PCM instance iff success
    NULL iff failure
*/
PCM *pcm_create(size_t mem_line, double rtime, double wtime);

/*
    Destroy PCM instance

    PARAMS
    @IN pcm - pointer to PCM

    RETURN
    This is a void function
*/
void pcm_destroy(PCM *pcm);

/*
    Simulate READ from PCM instance

    PARAMS
    @IN pcm - pointer to PCM instance
    @IN bytes - number of bytes to read

    RETURN
    time consumed by read
*/
static ___inline___ double pcm_read(const PCM *pcm, size_t bytes);

static ___inline___ double pcm_read(const PCM *pcm, size_t bytes)
{
    const size_t lines = INT_CEIL_DIV(bytes, pcm->mem_line);

    return (double)lines * pcm->read_time;
}

/*
    Simulate WRITE to PCM instance

    PARAMS
    @IN pcm - pointer to PCM instance
    @IN bytes - number of bytes to write

    RETURN
    time consumed by write
*/
static ___inline___ double pcm_write(PCM *pcm, size_t bytes);

static ___inline___ double pcm_write(PCM *pcm, size_t bytes)
{
    const size_t lines = INT_CEIL_DIV(bytes, pcm->mem_line);

    pcm->wearout += bytes;
    return (double)lines * pcm->write_time;
}




#endif