#ifndef __SEG_H_
#define __SEG_H_

#include <math.h>
#include <vips/vips.h>

#include "disjoint_set.h"
#include "utils.h"

typedef int (*merge_cri_fn_t)(int, int, int);
int seg_merge_criterion(int a, int b, int diff);

void image_seg(VipsImage *img, disjoint_set *ds, merge_cri_fn_t cri_fn,
               int diff);

#endif