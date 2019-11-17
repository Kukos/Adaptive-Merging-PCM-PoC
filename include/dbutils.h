#ifndef DBUTILS_H
#define DBUTILS_H

/*
    Common functions for DB Simulation

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE GPL 3.0
*/


#include <common.h>
#include <compiler.h>

typedef enum query_t
{
    QUERY_RANDOM,
    QUERY_ALWAYS_NEW,
    QUERY_SEQUENTIAL_PATTERN,
} query_t;

#define INT_CEIL_DIV(n, k) (((n) + (k) - 1) / (k))

#endif