#ifndef PARTITIONS_H
#define PARTITIONS_H

/*
    Adaptive Merging Partitions (set of sorted entries)

    Author: Michal Kukowski
    email: michalkukowski10@gmail.com

    LICENCE GPL 3.0
*/

typedef enum
{
    INVALIDATION_FLAG,
    INVALIDATION_BITMAP,
    INVALIDATION_JOURNAL,
    INVALIDATION_OVERWRITE,
    INVALIDATION_SKIP,
} invalidation_type_t;

#endif