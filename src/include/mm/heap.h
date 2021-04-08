#ifndef __HEAP_H__
#define __HEAP_H__

#include <stddef.h>
#include <stdint.h>

#define CONTAINER(C, l, v) ((C *)(((char *)v) - (intptr_t) & (((C *)0)->l)))
#define OFFSETOF(TYPE, MEMBER) __builtin_offsetof(TYPE, MEMBER)

#define DLIST_INIT(v, l) dlist_init(&v->l)

#define DLIST_REMOVE_FROM(h, d, l)                                             \
  {                                                                            \
    typeof(**h) **h_ = h, *d_ = d;                                             \
    dlist *head = &(*h_)->l;                                                   \
    dlist_remove_from(&head, &d_->l);                                          \
    if (head == NULL) {                                                        \
      *h_ = NULL;                                                              \
    } else {                                                                   \
      *h_ = CONTAINER(typeof(**h), l, head);                                   \
    }                                                                          \
  }

#define DLIST_PUSH(h, v, l)                                                    \
  {                                                                            \
    typeof(*v) **h_ = h, *v_ = v;                                              \
    dlist *head = &(*h_)->l;                                                   \
    if (*h_ == NULL)                                                           \
      head = NULL;                                                             \
    dlist_push(&head, &v_->l);                                                 \
    *h_ = CONTAINER(typeof(*v), l, head);                                      \
  }

#define DLIST_POP(h, l)                                                        \
  ({                                                                           \
    typeof(**h) **h_ = h;                                                      \
    dlist *head = &(*h_)->l;                                                   \
    dlist *res = dlist_pop(&head);                                             \
    if (head == NULL) {                                                        \
      *h_ = NULL;                                                              \
    } else {                                                                   \
      *h_ = CONTAINER(typeof(**h), l, head);                                   \
    }                                                                          \
    CONTAINER(typeof(**h), l, res);                                            \
  })

#define DLIST_ITERATOR_BEGIN(h, l, it)                                         \
  {                                                                            \
    typeof(*h) *h_ = h;                                                        \
    dlist *last_##it = h_->l.prev, *iter_##it = &h_->l, *next_##it;            \
    do {                                                                       \
      if (iter_##it == last_##it) {                                            \
        next_##it = NULL;                                                      \
      } else {                                                                 \
        next_##it = iter_##it->next;                                           \
      }                                                                        \
      typeof(*h) *it = CONTAINER(typeof(*h), l, iter_##it);

#define DLIST_ITERATOR_END(it)                                                 \
  }                                                                            \
  while ((iter_##it = next_##it))                                              \
    ;                                                                          \
  }

#define DLIST_ITERATOR_REMOVE_FROM(h, it, l) DLIST_REMOVE_FROM(h, iter_##it, l)

typedef struct dlist dlist;

struct dlist {
  dlist *next;
  dlist *prev;
};

typedef struct chunk chunk;
struct chunk {
  dlist all;
  int used;
  union {
    char data[0];
    dlist free;
  };
};

enum {
  NUM_SIZES = 32,
  ALIGN = 4,
  MIN_SIZE = sizeof(dlist),
  HEADER_SIZE = OFFSETOF(chunk, data),
};

int init_heap(void *mem, size_t size);
void *kmalloc(size_t size);
void kfree(void *mem);

#endif