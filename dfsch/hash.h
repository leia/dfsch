/*
 * dfsch - dfox's quick and dirty scheme implementation
 *   Hash tables
 * Copyright (C) 2005-2008 Ales Hakl
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

/** \file dfsch/hash.h
 *
 * Hash table datatype.
 */


#ifndef H__dfsch__hash__
#define H__dfsch__hash__

#include <dfsch/dfsch.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DFSCH_HASH_EQ 0
#define DFSCH_HASH_EQV 1
#define DFSCH_HASH_EQUAL 2

  /**
   * Create new hash table object
   */
  extern dfsch_object_t* dfsch_hash_make(int mode);

  /**
   * Check whenever given object is hashtable
   */
  extern int dfsch_hash_p(dfsch_object_t* obj);

  extern int dfsch_hash_ref_fast(dfsch_object_t* hash_obj,
                                 dfsch_object_t* key,
                                 dfsch_object_t** res);

  /**
   * Get given entry in hashtable.
   *
   * Return value is pair, whose CAR is value associated with key
   * or empty list in case of non-existant key.
   */
  extern dfsch_object_t* dfsch_hash_ref(dfsch_object_t* hash, 
                                        dfsch_object_t* key);

  /**
   * Associate value with key
   */
  extern void dfsch_hash_set(dfsch_object_t* hash,
                             dfsch_object_t* key,
                             dfsch_object_t* value);

  /**
   * Associate value with key
   */
  extern void dfsch_hash_put(dfsch_object_t* hash,
                             dfsch_object_t* key,
                             dfsch_object_t* value);
  
  /**
   * Delete given key.
   */
  extern int dfsch_hash_unset(dfsch_object_t* hash,
                              dfsch_object_t* key);
  
  /**
   * Set value associated with given key only when there is already such 
   * key.
   */
  extern int dfsch_hash_set_if_exists(dfsch_object_t* hash,
                                      dfsch_object_t* key,
                                      dfsch_object_t* value);
  
  /**
   * Convert hash table to list of associations.
   */
  extern dfsch_object_t* dfsch_hash_2_alist(dfsch_object_t* hash_obj);

  /**
   * Convert list of associations to hash table.
   */
  extern dfsch_object_t* dfsch_alist_2_hash(dfsch_object_t* alist,
                                            int mode);
  
  typedef int (*dfsch_custom_hash_ref_t)(dfsch_object_t* hash, 
                                         dfsch_object_t* key,
                                         dfsch_object_t** res);
  typedef void (*dfsch_custom_hash_set_t)(dfsch_object_t* hash, 
                                          dfsch_object_t* key,
                                          dfsch_object_t* value);
  typedef int (*dfsch_custom_hash_unset_t)(dfsch_object_t* hash, 
                                           dfsch_object_t* key);
  typedef int (*dfsch_custom_hash_set_if_exists_t)(dfsch_object_t* hash, 
                                                   dfsch_object_t* key,
                                                   dfsch_object_t* value);
  typedef dfsch_object_t* (*dfsch_custom_hash_2_alist)(dfsch_object_t* hash);

  typedef struct dfsch_custom_hash_type_t {
    dfsch_type_t parent;
    
    dfsch_custom_hash_ref_t ref;
    dfsch_custom_hash_set_t set;
    dfsch_custom_hash_unset_t unset;
    dfsch_custom_hash_set_if_exists_t set_if_exists;
    dfsch_custom_hash_2_alist hash_2_alist;
  } dfsch_custom_hash_type_t;

  extern dfsch_type_t dfsch_hash_basetype;
#define DFSCH_HASH_BASETYPE (&dfsch_hash_basetype)
  extern dfsch_type_t dfsch_standard_hash_type;
#define DFSCH_STANDARD_HASH_TYPE (&dfsch_standard_hash_type)
  extern dfsch_type_t dfsch_custom_hash_type_type;
#define DFSCH_CUSTOM_HASH_TYPE_TYPE (&dfsch_custom_hash_type_type)



#ifdef __cplusplus
}
#endif


#endif
