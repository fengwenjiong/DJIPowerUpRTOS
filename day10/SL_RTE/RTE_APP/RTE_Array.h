#ifndef __RTE_ARRAY_H
#define __RTE_ARRAY_H
#include "RTE_MEM.h"
typedef int  (*array_comp_t)(const void*, const void*);
// (left < right) == negative
// (left == right) == zero
// (left > right) == positive
typedef struct {
    int index;
    int length;
    RTE_MEM_Name_e membank;
    void **data;
} array_t;
void array_alloc(array_t **a, RTE_MEM_Name_e membank);
void array_alloc_init(array_t **a, RTE_MEM_Name_e membank, int size);
void array_clear(array_t *array);
void array_free(array_t *array);
int array_length(array_t *array);
void *array_at(array_t *array, int idx);
void array_push_back(array_t *array, void *element);
void *array_pop_back(array_t *array);
void *array_take(array_t *array, int idx);
void array_erase(array_t *array, int idx);
void array_resize(array_t *array, int num);
void array_sort(array_t *array, array_comp_t comp);
void array_isort(array_t *array, array_comp_t comp);
#endif // __RTE_ARRAY_H
