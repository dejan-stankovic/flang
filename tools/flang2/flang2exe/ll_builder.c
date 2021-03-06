/*
 * Copyright (c) 2017, NVIDIA CORPORATION.  All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "gbldefs.h"
#include "global.h"
#include "ll_builder.h"
#include "lldebug.h"

/*
 * LLMD_Builder -- Builder for metadata nodes.
 */
struct LLMD_Builder_ {
  LL_Module *module;
  unsigned nelems;
  unsigned capacity;
  LL_MDRef *elems;
  enum LL_MDClass mdclass;
  unsigned is_distinct : 1;
  LL_MDRef init_elems[32];
};

LLMD_Builder
llmd_init(LL_Module *module)
{
  LLMD_Builder mdb = calloc(1, sizeof(struct LLMD_Builder_));
  mdb->module = module;
  /* Start out with init_elems to save a malloc() call. */
  mdb->elems = mdb->init_elems;
  mdb->capacity = sizeof(mdb->init_elems) / sizeof(mdb->init_elems[0]);
  return mdb;
}

/* Double the capacity in mdb. */
static void
grow(LLMD_Builder mdb)
{
  mdb->capacity *= 2;
  if (mdb->elems == mdb->init_elems) {
    /* First time we grow, don't free init_elems. */
    mdb->elems = malloc(mdb->capacity * sizeof(mdb->elems[0]));
    memcpy(mdb->elems, mdb->init_elems, sizeof(mdb->init_elems));
  } else {
    /* We've grown before. Just realloc. */
    mdb->elems = realloc(mdb->elems, mdb->capacity * sizeof(mdb->elems[0]));
  }
}

void
llmd_add_md(LLMD_Builder mdb, LL_MDRef mdnode)
{
  if (mdb->nelems >= mdb->capacity)
    grow(mdb);
  mdb->elems[mdb->nelems++] = mdnode;
}

void
llmd_add_null(LLMD_Builder mdb)
{
  llmd_add_md(mdb, ll_get_md_null());
}

void
llmd_add_i1(LLMD_Builder mdb, int value)
{
  llmd_add_md(mdb, ll_get_md_i1(value));
}

void
llmd_add_i32(LLMD_Builder mdb, int value)
{
  llmd_add_md(mdb, ll_get_md_i32(mdb->module, value));
}

void
llmd_add_i64(LLMD_Builder mdb, long long value)
{
  llmd_add_md(mdb, ll_get_md_i64(mdb->module, value));
}

void
llmd_add_i64_lsb_msb(LLMD_Builder mdb, unsigned lsb, unsigned msb)
{
  long long value = msb;
  value <<= 32;
  value |= lsb;
  llmd_add_i64(mdb, value);
}

void
llmd_add_INT64(LLMD_Builder mdb, INT64 value)
{
  /* INT64 is big-endian (msb, lsb). */
  llmd_add_i64_lsb_msb(mdb, value[1], value[0]);
}

void
llmd_add_string(LLMD_Builder mdb, const char *value)
{
  llmd_add_md(mdb, ll_get_md_string(mdb->module, value));
}

void
llmd_add_value(LLMD_Builder mdb, LL_Value *value)
{
  llmd_add_md(mdb, ll_get_md_value(mdb->module, value));
}

void
llmd_reverse(LLMD_Builder mdb)
{
  unsigned i, j;
  LL_MDRef *e = mdb->elems;

  if (mdb->nelems <= 1)
    return;

  for (i = 0, j = mdb->nelems - 1; i < j; i++, j--) {
    LL_MDRef t = e[i];
    e[i] = e[j];
    e[j] = t;
  }
}

unsigned
llmd_get_nelems(LLMD_Builder mdb)
{
  return mdb->nelems;
}

void
llmd_set_distinct(LLMD_Builder mdb)
{
  mdb->is_distinct = TRUE;
}

void
llmd_set_class(LLMD_Builder mdb, enum LL_MDClass mdclass)
{
  mdb->mdclass = mdclass;
}

LL_MDRef
llmd_finish(LLMD_Builder mdb)
{
  LL_MDRef mdref;

  if (mdb->is_distinct)
    mdref = ll_create_distinct_md_node(mdb->module, mdb->mdclass, mdb->elems,
                                       mdb->nelems);
  else
    mdref = ll_get_md_node(mdb->module, mdb->mdclass, mdb->elems, mdb->nelems);

  if (mdb->elems != mdb->init_elems)
    free(mdb->elems);
  free(mdb);

  return mdref;
}
