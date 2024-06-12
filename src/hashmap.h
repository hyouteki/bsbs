#ifndef BSBS_HASHMAP_H_
#define BSBS_HASHMAP_H_

#include <stdlib.h>
#include "helper.h"

#define Bsbs_Hashmap_Size 2048

typedef struct Bsbs_Hashmap_Entry {
	char *query;
	char *response;
	struct Bsbs_Hashmap_Entry *next;
} Bsbs_Hashmap_Entry;

typedef struct Bsbs_Hashmap {
	Bsbs_Hashmap_Entry *table[Bsbs_Hashmap_Size];
} Bsbs_Hashmap;

void Bsbs_Hashmap_Insert(Bsbs_Hashmap *, char *, char *);
int Bsbs_Hashmap_Find(Bsbs_Hashmap *, char *);
char *Bsbs_Hashmap_Lookup(Bsbs_Hashmap *, char *);

void Bsbs_Hashmap_Insert(Bsbs_Hashmap *hashmap, char *query, char *response) {
	unsigned int hash = Bsbs_Hash(query)%Bsbs_Hashmap_Size;
	Bsbs_Hashmap_Entry *entry = (Bsbs_Hashmap_Entry *)malloc(sizeof(Bsbs_Hashmap_Entry));
    if (!entry) Error("hashmap entry allocation failed in function '%s'\n", __func__);
    entry->query = strdup(query);
    entry->response = strdup(response);
	entry->next = hashmap->table[hash];
    hashmap->table[hash] = entry;
}

int Bsbs_Hashmap_Find(Bsbs_Hashmap *hashmap, char *query) {
	unsigned int hash = Bsbs_Hash(query)%Bsbs_Hashmap_Size;
	Bsbs_Hashmap_Entry *itr = hashmap->table[hash];
	while (itr) {
		if (strcmp(itr->query, query) == 0) return 1;
		itr = itr->next;
	}
	return 0;
}

// unsafe lookup assuming Bsbs_Hashmap_Find returned 1
char *Bsbs_Hashmap_Lookup(Bsbs_Hashmap *hashmap, char *query) {
	unsigned int hash = Bsbs_Hash(query)%Bsbs_Hashmap_Size;
	Bsbs_Hashmap_Entry *itr = hashmap->table[hash];
	while (itr) {
		if (strcmp(itr->query, query) == 0) return strdup(itr->response);
		itr = itr->next;
	}
	return NULL;
}

#endif // BSBS_HASHMAP_H_
