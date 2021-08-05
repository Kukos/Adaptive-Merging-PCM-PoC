#include <stdlib.h>
#include <common.h>
#include <dbam.h>
#include <dbpam.h>
#include <log.h>

DB_PAM *db_pam_create(PCM *pcm, size_t num_entries, size_t key_size, size_t entry_size, size_t buffer_size, size_t index_node_size, btree_type_t index_type)
{
    DB_PAM *pam;

    TRACE();

    pam = malloc(sizeof(*pam));
    if (pam == NULL)
        ERROR("malloc error\n", NULL);

    pam->am = db_am_create(pcm, num_entries, key_size, entry_size, buffer_size, index_node_size, INVALIDATION_JOURNAL, index_type);
    if (pam->am == NULL)
    {
        FREE(pam);
        ERROR("db_am_create error\n", NULL);
    }

    return pam;
}

void db_pam_destroy(DB_PAM *pam)
{
    if (pam == NULL)
        return;

    db_am_destroy(pam->am);
    FREE(pam);
}

double db_pam_search(DB_PAM *pam, query_t type, size_t entries)
{
    return db_am_search(pam->am, type, entries);
}

double db_pam_insert(DB_PAM *pam, size_t entries)
{
    return db_am_insert(pam->am, entries);
}

double db_pam_delete(DB_PAM* pam, size_t entries)
{
    return db_am_delete(pam->am, entries);
}