#include "RTE_Array.h"
#define ARRAY_INIT_SIZE (4) // Size of one GC block.
void array_alloc(array_t **a, RTE_MEM_Name_e membank)
{
    array_t *array = RTE_MEM_Alloc0(membank,sizeof(array_t));
    array->index  = 0;
    array->length = ARRAY_INIT_SIZE;
    array->membank  = membank;
    array->data   = RTE_MEM_Alloc0(membank,ARRAY_INIT_SIZE * sizeof(void*));
    *a = array;
}

void array_alloc_init(array_t **a, RTE_MEM_Name_e membank , int size)
{
    array_t *array = RTE_MEM_Alloc0(membank,sizeof(array_t));
    array->index  = 0;
    array->length = size;
    array->membank  = membank;
    array->data   = RTE_MEM_Alloc0(membank,size * sizeof(void*));
    *a = array;
}

void array_clear(array_t *array)
{
    if (array->membank != MEM_NULL) {
        for (int i=0, j=array->index; i<j; i++) {
            RTE_MEM_Free(array->membank, array->data[i]);
        }
    }
    RTE_MEM_Free(array->membank, array->data);
    array->index = 0;
    array->length = 0;
    array->data = NULL;
    array->membank = MEM_NULL;
    // Note: realloc with null pointer and (size != 0) returns valid pointer.
    // Note: realloc with valid pointer and (size == 0) returns null pointer.
}

void array_free(array_t *array)
{
    RTE_MEM_Name_e mem = (RTE_MEM_Name_e)array->membank;
    array_clear(array);
    RTE_MEM_Free(mem, array);
}

int array_length(array_t *array)
{
    return array->index; // index is the actual length, length is the max length
}

void *array_at(array_t *array, int idx)
{
    return array->data[idx];
}

void array_push_back(array_t *array, void *element)
{
    if (array->index == array->length) {
        array->length += ARRAY_INIT_SIZE;
        array->data    = RTE_MEM_Realloc(array->membank, array->data, array->length * sizeof(void*));
    }
    array->data[array->index++] = element;
}

void *array_pop_back(array_t *array)
{
    void *el=NULL;
    if (array->index) {
        el = array->data[--array->index];
    }
    return el;
}

void *array_take(array_t *array, int idx)
{
    void *el=array->data[idx];
    if ((1 < array->index) && (idx < (array->index - 1))) {
        /* Since dst is always < src we can just use memcpy */
        memcpy(array->data+idx, array->data+idx+1, (array->index-idx-1) * sizeof(void*));
    }
    array->index--;
    return el;
}

void array_erase(array_t *array, int idx)
{
    if (array->membank != MEM_NULL) {
        RTE_MEM_Free(array->membank, array->data[idx]);
    }
    array_take(array, idx);
}

void array_resize(array_t *array, int num)
{
    if (array->index != num) {
        if (!num) {
            array_clear(array);
        } else {
            if (array->index > num) {
                if (array->membank != MEM_NULL) {
                    for (int i=num, j=array->index; i<j; i++) {
                        RTE_MEM_Free(array->membank, array->data[i]);
                    }
                }
                array->index = num;
            }
            // resize array
            array->length = num;
            array->data = RTE_MEM_Realloc(array->membank, array->data, array->length * sizeof(void*));
        }
    }
}

// see micropython quicksort (objlist.c -> mp_quicksort)
static void quicksort(void **head, void **tail, array_comp_t comp)
{
//    MP_STACK_CHECK();
    while (head < tail) {
        void **h = head - 1;
        void **t = tail;
        void *v = tail[0];
        for (;;) {
            do ++h; while(h < t && comp(h[0], v) < 0);
            do --t; while(h < t && comp(v, t[0]) < 0);
            if (h >= t) break;
            void *x = h[0];
            h[0] = t[0];
            t[0] = x;
        }
        void *x = h[0];
        h[0] = tail[0];
        tail[0] = x;
        // do the smaller recursive call first, to keep stack within O(log(N))
        if (t - head < tail - h - 1) {
            quicksort(head, t, comp);
            head = h + 1;
        } else {
            quicksort(h + 1, tail, comp);
            tail = t;
        }
    }
}

// TODO Python defines sort to be stable but ours is not
void array_sort(array_t *array, array_comp_t comp)
{
    if (array->index > 1) {
        quicksort(array->data, array->data + array->index - 1, comp);
    }
}

void array_isort(array_t *array, array_comp_t comp)
{
    if (array->index > 1) {
        for (int i = 1; i < array->index; i++) {
            int j = i-1;
            void *t = array->data[i];
            while (j >= 0 && comp(array->data[j], t)) {
                array->data[j+1] = array->data[j];
                j--;
            }
            array->data[j+1] = t;
        }
    }
}
