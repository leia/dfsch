/*
 * dfsch - dfox's quick and dirty scheme implementation
 *   Internal identity hash tables
 * Copyright (C) 2009 Ales Hakl
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef H__dfsch__eqhash__
#define H__dfsch__eqhash__

#include <dfsch/dfsch.h>

#define DFSCH_EQHASH_SMALL_SIZE 4
#define DFSCH_EQHASH_CACHE_SIZE 8

#define DFSCH_EQHASH_LARGE 1

typedef struct dfsch_eqhash_entry_t dfsch_eqhash_entry_t;

struct dfsch_eqhash_entry_t {
  dfsch_object_t* key;
  dfsch_eqhash_entry_t* next;
  dfsch_object_t* value;
  short flags;
};

typedef struct dfsch__eqhash_small_t {
  dfsch_object_t* keys[DFSCH_EQHASH_SMALL_SIZE];
  dfsch_object_t* values[DFSCH_EQHASH_SMALL_SIZE];
  short flags[DFSCH_EQHASH_SMALL_SIZE];
} dfsch__eqhash_small_t;

typedef struct dfsch__eqhash_large_t {
  dfsch_eqhash_entry_t* cache[DFSCH_EQHASH_CACHE_SIZE];
  dfsch_eqhash_entry_t* vector;
  size_t mask;
  size_t count;
} dfsch__eqhash_large_t;

#undef small
#undef large

typedef union dfsch__eqhash_contents_t {
    dfsch__eqhash_small_t small;
    dfsch__eqhash_large_t large;
} dfsch__eqhash_contents_t;

typedef struct dfsch_eqhash_t {
  int is_large; 
  /*this can be changed to char or bitfield or whatever when necessary*/
  dfsch__eqhash_contents_t contents;
} dfsch_eqhash_t; 

void dfsch_eqhash_init(dfsch_eqhash_t* hash, int start_large);
void dfsch_eqhash_put(dfsch_eqhash_t* hash,
                      dfsch_object_t* key, dfsch_object_t* value);
void dfsch_eqhash_set(dfsch_eqhash_t* hash,
                      dfsch_object_t* key, dfsch_object_t* value);
int dfsch_eqhash_unset(dfsch_eqhash_t* hash, dfsch_object_t* key);
void dfsch_eqhash_set_flags(dfsch_eqhash_t* hash,
                            dfsch_object_t* key, short flags);
int dfsch_eqhash_set_if_exists(dfsch_eqhash_t* hash,
                               dfsch_object_t* key, dfsch_object_t* value,
                               short* flags);
dfsch_object_t* dfsch_eqhash_ref(dfsch_eqhash_t* hash,
                                 dfsch_object_t* key);
int dfsch_eqhash_ref_ex(dfsch_eqhash_t* hash,
                        dfsch_object_t* key, 
                        dfsch_object_t** value, short *flags, 
                        dfsch_eqhash_entry_t** entry);

dfsch_object_t* dfsch_eqhash_2_alist(dfsch_eqhash_t* hash);
#endif
