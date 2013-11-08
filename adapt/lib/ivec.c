/**
 *  Copyright (C) 2011, 2012, 2013 CentecNetworks, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file
 * @brief This file implements a vector database (with integer key) and its operations
 */

#include <config.h>
#include "ivec.h"
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "dynamic-string.h"
#include "util.h"
#include "ofp_types.h"

extern int
ctc_cli_out_ofp(const char *fmt, ...);

void
ivec_init(struct ivec *ivec)
{
    ivec->values = NULL;
    ivec->n = 0;
    ivec->allocated = 0;
}

void
ivec_destroy(struct ivec *ivec)
{
    ivec_clear(ivec);
    free(ivec->values);
}

void
ivec_clear(struct ivec *ivec)
{
    ivec->n = 0;
}

bool
ivec_is_empty(const struct ivec *ivec)
{
    return ivec->n == 0;
}

void
ivec_add(struct ivec *ivec, const uint32_t value)
{
    ivec_add_nocopy(ivec, value);
}

void
ivec_del(struct ivec *ivec, const uint32_t value)
{
    size_t offset;

    offset = ivec_find(ivec, value);
    if (offset != SIZE_MAX)
    {
        /* free(ivec->values); */
        memmove(&ivec->values[offset], &ivec->values[offset + 1], sizeof *ivec->values * (ivec->n - offset - 1));
        ivec->n--;
    }
}

static void
ivec_expand(struct ivec *ivec)
{
    if (ivec->n >= ivec->allocated)
    {
        ivec->values = x2nrealloc(ivec->values, &ivec->allocated, sizeof *ivec->values);
    }
}

void
ivec_add_nocopy(struct ivec *ivec, uint32_t value)
{
    ivec_expand(ivec);
    ivec->values[ivec->n++] = value;
}

static int
compare_ints(const void *a_, const void *b_)
{
    /*
      int const *a = a_;
      int const *b = b_;
      return (*a >= *b);
    */
    return *((uint32_t*) a_) - *((uint32_t*) b_);
}

void
ivec_sort(struct ivec *ivec)
{
    qsort(ivec->values, ivec->n, sizeof *ivec->values, compare_ints);
}

bool
ivec_contains(const struct ivec *ivec, const uint32_t value)
{
    return ivec_find(ivec, value) != SIZE_MAX;
}

size_t
ivec_find(const struct ivec *ivec, const uint32_t value)
{
    int i = 0;
    for (i = 0; i < ivec->n; i++)
    {
        if (ivec->values[i] == value)
        {
            return i;
        }
    }
    return SIZE_MAX;
    /*
     uint32_t *p;

     assert(ivec_is_sorted(ivec));
     p = bsearch(&value, ivec->values, ivec->n, sizeof(uint32_t),
     compare_ints);
     return p ? (p - ivec->values) : SIZE_MAX;
     */
}

/**
 * Find spec value
 * @param ivec ivec
 * @param value value need to match
 * @param prev_value value before the value need to be matched
 * @param after_value value after the value need to matched
 * @return 0: exact match 1: not matched, return prev_value and after_value
 */
int
ivec_find_value(const struct ivec *ivec, const uint32_t value, uint32_t * prev_value, uint32_t * after_value)
{
    int i = 0;
    uint32_t ret = 1;
    uint32_t temp_value = 0;
    for (i = 0; i < ivec->n; i++)
    {
        temp_value = ivec->values[i];
        if (temp_value < value)
        {
            *prev_value = temp_value;
        }
        else if (temp_value > value)
        {
            *after_value = temp_value;
            break;
        }
        else
        {
            ret = 0;
        }
    }
    return ret;
}

bool
ivec_is_sorted(const struct ivec *ivec)
{
    size_t i;

    for (i = 1; i < ivec->n; i++)
    {
        if (ivec->values[i - 1] > ivec->values[i])
        {
            return false;
        }
    }
    return true;
}

/* Get the lower number (smaller number) */
int
ivec_get_lower(const struct ivec *ivec, const uint32_t value, uint32_t * lower_value_p)
{
    uint32_t lower_value = SIZE_MAX;
    uint32_t higher_value = SIZE_MAX;
    int ret = 0;
    ret = ivec_find_value(ivec, value, &lower_value, &higher_value);
    /* OFP_DEBUG_PRINT("find lower_value %u\n",lower_value); */
    *lower_value_p = lower_value;
    if (lower_value == SIZE_MAX)
    {
        return -1;
    }
    return 0;
}

/* Get the higher number (larger number) */
int
ivec_get_higher(const struct ivec *ivec, const uint32_t value, uint32_t * higher_value_p)
{
    uint32_t lower_value = SIZE_MAX;
    uint32_t higher_value = SIZE_MAX;
    int ret = 0;
    ret = ivec_find_value(ivec, value, &lower_value, &higher_value);
    /* OFP_DEBUG_PRINT("find higher_value %u\n",higher_value); */
    *higher_value_p = higher_value;
    if (higher_value == SIZE_MAX)
    {
        return -1;
    }
    return 0;
}

uint32_t
ivec_get_last(const struct ivec *ivec)
{
    assert(ivec->n);
    return ivec->values[ivec->n - 1];
}

void
ivec_print(const struct ivec *ivec)
{
    size_t i;

    for (i = 0; i < ivec->n; i++)
    {
        ctc_cli_out_ofp("%u ", ivec->values[i]);
        if ((ivec->n + 1) % 10 == 0)
        {
            ctc_cli_out_ofp("\n%13s", "");
        }
    }
    ctc_cli_out_ofp("\n");
}
