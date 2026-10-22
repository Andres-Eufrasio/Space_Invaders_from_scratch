#include <stdlib.h>
#include <string.h>

#include "hashmap.h"

#define HASHSIZE 101

static struct nlist *hashtab[HASHSIZE];

/* Duplicate a string using standard C. */
static char *string_duplicate(const char *s)
{
    size_t len = strlen(s) + 1;
    char *p = malloc(len);

    if (p != NULL) {
        memcpy(p, s, len);
    }

    return p;
}

/* Form a hash value for a string. */
static unsigned hash(const char *s)
{
    unsigned hashval = 0;

    while (*s != '\0') {
        hashval = (unsigned char)*s + 31u * hashval;
        s++;
    }

    return hashval % HASHSIZE;
}

/* Look for a name in the hash table. */
struct nlist *lookup(const char *s)
{
    for (struct nlist *np = hashtab[hash(s)];
         np != NULL;
         np = np->next) {
        if (strcmp(s, np->name) == 0) {
            return np;
        }
    }

    return NULL;
}

/* Install or update a name-definition pair. */
struct nlist *install(const char *name, const char *defn)
{
    struct nlist *np = lookup(name);
    char *new_defn = string_duplicate(defn);

    if (new_defn == NULL) {
        return NULL;
    }

    if (np != NULL) {
        free(np->defn);
        np->defn = new_defn;
        return np;
    }

    np = malloc(sizeof *np);
    if (np == NULL) {
        free(new_defn);
        return NULL;
    }

    np->name = string_duplicate(name);
    if (np->name == NULL) {
        free(new_defn);
        free(np);
        return NULL;
    }

    unsigned hashval = hash(name);

    np->defn = new_defn;
    np->next = hashtab[hashval];
    hashtab[hashval] = np;

    return np;
}

/* Free all entries in the hash table. */
void hashmap_cleanup(void)
{
    for (size_t i = 0; i < HASHSIZE; i++) {
        struct nlist *np = hashtab[i];

        while (np != NULL) {
            struct nlist *next = np->next;

            free(np->name);
            free(np->defn);
            free(np);

            np = next;
        }

        hashtab[i] = NULL;
    }
}

