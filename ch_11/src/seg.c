#include "seg.h"

#include <stdint.h>
#include <stdio.h>

int seg_merge_criterion(int a, int b, int diff) { return abs(a - b) <= diff; }

void save_as_ppm(const char *filename, uint32_t *val, uint32_t *cnt, int width,
                 int height) {
  FILE *fp = fopen(filename, "wb");
  if (!fp) return;

  fprintf(fp, "P6\n%d %d\n255\n", width, height);

  for (int i = 0; i < width * height; ++i) {
    if (val[i] > 0) {
      fputc(0, fp);                // R
      fputc(val[i] / cnt[i], fp);  // G
      fputc(0, fp);                // B
    } else {
      fputc(255, fp);
      fputc(255, fp);
      fputc(255, fp);
    }
  }
  fclose(fp);
}

void image_seg(VipsImage *img, disjoint_set *ds, merge_cri_fn_t cri_fn,
               int diff) {
  int m = vips_image_get_width(img);
  int h = vips_image_get_height(img);
  size_t img_sz;
  unsigned char *img_data = vips_image_write_to_memory(img, &img_sz);
  uint32_t *val_arr = calloc(m * h, sizeof(uint32_t));
  uint32_t *cnt_arr = malloc(m * h * sizeof(uint32_t));

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < m; x++) {
      int idx = y * m + x;
      val_arr[idx] = img_data[idx];
      cnt_arr[idx] = 1;
    }
  }

  for (int y = 0; y < h; y++) {
    for (int x = 0; x < m; x++) {
      int ij = find(ds, y * m + x);

      // check left and top of current pos are valid
      int neighbors[2] = {(x > 0) ? (y * m + x - 1) : -1,
                          (y > 0) ? ((y - 1) * m + x) : -1};

      for (int k = 0; k < 2; k++) {
        if (neighbors[k] == -1) continue;

        int target = find(ds, neighbors[k]);
        if (ij != target && !is_same_set(ds, ij, target)) {
          int avg_ij = val_arr[ij] / cnt_arr[ij];
          int avg_target = val_arr[target] / cnt_arr[target];

          if (cri_fn(avg_ij, avg_target, diff)) {
            int new_root = disjoint_set_union(ds, ij, target);
            int old_root = (new_root == ij) ? target : ij;

            val_arr[new_root] += val_arr[old_root];
            cnt_arr[new_root] += cnt_arr[old_root];
            val_arr[old_root] = 0;
            cnt_arr[old_root] = 0;
            ij = new_root;
          }
        }
      }
    }
  }
  int n_grps = 0;
  for (int i = 0; i < m; ++i) {
    for (int j = 0; j < h; ++j) {
      int ij = i * h + j;
      if (cnt_arr[ij]) {
        printf("(%d, %d), Total: %u, Avg: %u, Grp_nums: %d\n", i, j,
               val_arr[ij], val_arr[ij] / cnt_arr[ij], cnt_arr[ij]);
        n_grps++;
      }
    }
  }
  printf("N groups: %d\n", n_grps);
  save_as_ppm("processed_img.ppm", val_arr, cnt_arr, m, h);
  if (val_arr) free(val_arr);
  if (cnt_arr) free(cnt_arr);
  if (img_data) free(img_data);
}