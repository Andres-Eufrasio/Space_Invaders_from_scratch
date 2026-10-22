#ifndef HASHMAP_H
#define HASHMAP_H

struct nlist {
    struct nlist *next;
    char *name;
    char *defn;
};

struct nlist *lookup(const char *s);
struct nlist *install(const char *name, const char *defn);
void hashmap_cleanup(void);

#endif
