#include <dfsch/lib/collections.h>

#include <limits.h>

/*
 * Priority queues
 */

typedef struct pq_entry_t pq_entry_t;

struct pq_entry_t {
  dfsch_object_t* object;
  pq_entry_t* next;
};

typedef struct pqueue_t {
  dfsch_type_t* type;
  pq_entry_t* head;
  dfsch_object_t* lt;
} pqueue_t;

dfsch_type_t dfsch_collections_priority_queue_type = {
  DFSCH_STANDARD_TYPE,
  NULL,
  sizeof(pqueue_t),
  "collections:priority-queue",
};

/* TODO: effective implementation */

dfsch_object_t* dfsch_collections_make_priority_queue(dfsch_object_t* lt){
  pqueue_t* pq = dfsch_make_object(DFSCH_COLLECTIONS_PRIORITY_QUEUE_TYPE);

  pq->head = NULL;
  pq->lt = lt;
  
  return (dfsch_object_t*)pq;
}
void dfsch_collections_priority_queue_push(dfsch_object_t* q,
                                           dfsch_object_t* o){
  pqueue_t* pq;
  pq_entry_t* e;
  pq_entry_t* i;
  if (!DFSCH_INSTANCE_P(q, DFSCH_COLLECTIONS_PRIORITY_QUEUE_TYPE)){
    dfsch_error("Not a priority queue", q);
  }
  pq = (pqueue_t*)q;

  e = GC_NEW(pq_entry_t);
  e->object = o;

  if (!pq->head){
    pq->head = e;
  } else if (dfsch_apply(pq->lt, dfsch_list(2,
                                            e->object,
                                            pq->head->object)) != NULL){
    e->next = pq->head;
    pq->head = e;
  } else {
    i = pq->head;
    while (i->next){
      if (dfsch_apply(pq->lt, dfsch_list(2, 
                                         e->object,
                                         i->next->object))){
        e->next = i->next;
        break;
      }
    }
    i->next = e;
  }
}
dfsch_object_t* dfsch_collections_priority_queue_pop(dfsch_object_t* q){
  pqueue_t* pq;
  pq_entry_t* e;
  if (!DFSCH_INSTANCE_P(q, DFSCH_COLLECTIONS_PRIORITY_QUEUE_TYPE)){
    dfsch_error("Not a priority queue", q);
  }
  pq = q;
  if (!pq->head){
    dfsch_error("Priority queue is empty", q);
  }
  e = pq->head;
  pq->head = e->next;
  return e->object;
}
int dfsch_collections_priority_queue_empty_p(dfsch_object_t* q){
  if (!DFSCH_INSTANCE_P(q, DFSCH_COLLECTIONS_PRIORITY_QUEUE_TYPE)){
    dfsch_error("Not a priority queue", q);
  }
  return ((pqueue_t*)q)->head == NULL;
}

/*
 * bit vectors
 */

typedef struct bitvector_t {
  dfsch_type_t* type;
  size_t length;
  size_t num_words;
  unsigned int words[];
} bitvector_t;

dfsch_type_t dfsch_collections_bitvector_type = {
  .type = DFSCH_STANDARD_TYPE,
  .superclass = NULL,
  .name = "collections:bit-vector",
  .size = sizeof(bitvector_t)
};

#define WORD_BITS (CHAR_BIT * sizeof(unsigned int))

static bitvector_t* alloc_bitvector(size_t length){
  size_t num_words = length / WORD_BITS;
  bitvector_t* bv;
  if (num_words % WORD_BITS != 0){
    num_words++;
  }
  
  bv = GC_MALLOC_ATOMIC(sizeof(bitvector_t) + num_words * sizeof(unsigned int));
    
  bv->type = DFSCH_COLLECTIONS_BITVECTOR_TYPE;
  bv->length = length;
  bv->num_words = num_words;
  return bv;
}

static void mask_unused_bits(bitvector_t* bv){
  bv->words[bv->num_words - 1] &= (1 << (bv->length % WORD_BITS)) - 1;
}

dfsch_object_t* dfsch_collections_make_bitvector(size_t length){
  bitvector_t* bv;
  bv = alloc_bitvector(length);

  memset(bv->words, 0, bv->num_words * sizeof(unsigned int));
  return (dfsch_object_t*)bv;
}
dfsch_object_t* dfsch_collections_list_2_bitvector(dfsch_object_t* values){
  size_t length;
  size_t j;
  dfsch_object_t* i;
  bitvector_t* bv;

  length = dfsch_list_length_check(values);
  bv = alloc_bitvector(length);
  i = values;

  for (j = 0; j < length && DFSCH_PAIR_P(i); j++, i = DFSCH_FAST_CDR(i)){
    if (DFSCH_FAST_CAR(i)){
      bv->words[j / WORD_BITS] |= (1 << (j % WORD_BITS));  
    } else {
      bv->words[j / WORD_BITS] &= ~(1 << (j % WORD_BITS));
    }    
  }

  return (dfsch_object_t*) bv;
}
dfsch_object_t* dfsch_collections_bitvector_2_list(dfsch_object_t* bv){
  bitvector_t* b = DFSCH_ASSERT_TYPE(bv, DFSCH_COLLECTIONS_BITVECTOR_TYPE);
  size_t i;
  dfsch_object_t* head;
  dfsch_object_t* tail;

  if (b->length == 0){
    return NULL;
  }
  
  head = tail = dfsch_cons(dfsch_bool(b->words[0] & 0x01 != 0), NULL);

  for (i = 1; i < b->length; i++){
    dfsch_object_t* tmp = dfsch_cons(dfsch_bool((b->words[i / WORD_BITS] 
                                                 & 1 << (i % WORD_BITS)) 
                                                != 0),
                                     NULL);
    DFSCH_FAST_CDR_MUT(tail) = tmp;
    tail = tmp;
  }

  return head;
}
int dfsch_collections_bitvector_ref(dfsch_object_t* bv, size_t n){
  bitvector_t* b = DFSCH_ASSERT_TYPE(bv, DFSCH_COLLECTIONS_BITVECTOR_TYPE);
  if (b->length <= n){
    dfsch_error("Index out of range", dfsch_make_number_from_long(n));
  }

  return (b->words[n / WORD_BITS] & 1 << (n % WORD_BITS)) != 0;
}
void dfsch_collections_bitvector_set(dfsch_object_t* bv, size_t n, int v){
  bitvector_t* b = DFSCH_ASSERT_TYPE(bv, DFSCH_COLLECTIONS_BITVECTOR_TYPE);
  if (b->length <= n){
    dfsch_error("Index out of range", dfsch_make_number_from_long(n));
  }

  if (v){
    b->words[n / WORD_BITS] |= (1 << (n % WORD_BITS));  
  } else {
    b->words[n / WORD_BITS] &= ~(1 << (n % WORD_BITS));
  }
}
size_t dfsch_collections_bitvector_length(dfsch_object_t* bv){
  bitvector_t* b = DFSCH_ASSERT_TYPE(bv, DFSCH_COLLECTIONS_BITVECTOR_TYPE);
  return b->length;
}
dfsch_object_t* dfsch_collections_bitvector_not(dfsch_object_t* bv){
  bitvector_t* b = DFSCH_ASSERT_TYPE(bv, DFSCH_COLLECTIONS_BITVECTOR_TYPE);
  bitvector_t* r = alloc_bitvector(b->length);
  size_t i;

  for (i = 0; i < b->num_words; i++){
    r->words[i] = ~b->words[i];
  }

  mask_unused_bits(r);

  return (dfsch_object_t*)r;
}
dfsch_object_t* dfsch_collections_bitvector_or(dfsch_object_t* bva,
                                               dfsch_object_t* bvb){
  bitvector_t* a = DFSCH_ASSERT_TYPE(bva, DFSCH_COLLECTIONS_BITVECTOR_TYPE);
  bitvector_t* b = DFSCH_ASSERT_TYPE(bvb, DFSCH_COLLECTIONS_BITVECTOR_TYPE);
  bitvector_t* r;
  int i;

  if (a->length < b->length){
    bitvector_t* tmp = a;
    a = b;
    b = tmp;
  }

  r = alloc_bitvector(a->length);
  
  for (i = 0; i < b->num_words; i++){
    r->words[i] = a->words[i] | b->words[i]; 
  }

  for (; i < a->num_words; i++){
    r->words[i] = a->words[i]; 
  }

  return (dfsch_object_t*)r;
}
dfsch_object_t* dfsch_collections_bitvector_and(dfsch_object_t* bva,
                                               dfsch_object_t* bvb){
  bitvector_t* a = DFSCH_ASSERT_TYPE(bva, DFSCH_COLLECTIONS_BITVECTOR_TYPE);
  bitvector_t* b = DFSCH_ASSERT_TYPE(bvb, DFSCH_COLLECTIONS_BITVECTOR_TYPE);
  bitvector_t* r;
  int i;

  if (a->length < b->length){
    bitvector_t* tmp = a;
    a = b;
    b = tmp;
  }

  r = alloc_bitvector(a->length);
  
  for (i = 0; i < b->num_words; i++){
    r->words[i] = a->words[i] | b->words[i]; 
  }

  for (; i < a->num_words; i++){
    r->words[i] = 0; 
  }

  return (dfsch_object_t*)r;
}
dfsch_object_t* dfsch_collections_bitvector_xor(dfsch_object_t* bva,
                                                dfsch_object_t* bvb){
  bitvector_t* a = DFSCH_ASSERT_TYPE(bva, DFSCH_COLLECTIONS_BITVECTOR_TYPE);
  bitvector_t* b = DFSCH_ASSERT_TYPE(bvb, DFSCH_COLLECTIONS_BITVECTOR_TYPE);
  bitvector_t* r;
  int i;

  if (a->length < b->length){
    bitvector_t* tmp = a;
    a = b;
    b = tmp;
  }

  r = alloc_bitvector(a->length);
  
  for (i = 0; i < b->num_words; i++){
    r->words[i] = a->words[i] ^ b->words[i]; 
  }

  for (; i < a->num_words; i++){
    r->words[i] = a->words[i]; 
  }

  return (dfsch_object_t*)r;
}
dfsch_object_t* dfsch_collections_bitvector_2_integer(dfsch_object_t* bv){
}
dfsch_object_t* dfsch_collections_integer_2_bitvector(dfsch_object_t* bv){
}
dfsch_object_t* dfsch_collections_bitvector_increment(dfsch_object_t* bv){
}

int dfsch_collections_bitvector_all_zeros_p(dfsch_object_t* bv){
  bitvector_t* b = DFSCH_ASSERT_TYPE(bv, DFSCH_COLLECTIONS_BITVECTOR_TYPE);
  size_t i;

  for (i = 0; i < b->num_words; i++){
    if (b->words[i]){
      return 0;
    }
  }
  
  return 1;
}
int dfsch_collections_bitvector_all_ones_p(dfsch_object_t* bv){
  bitvector_t* b = DFSCH_ASSERT_TYPE(bv, DFSCH_COLLECTIONS_BITVECTOR_TYPE);
  size_t i;

  for (i = 0; i < (b->num_words - 1); i++){
    if (b->words[i] != UINT_MAX){
      return 0;
    }
  }
  
  if (b->words[b->num_words -1] != ((1 << (b->length % WORD_BITS)) - 1)){
    return 0;
  }

  return 1;
}
