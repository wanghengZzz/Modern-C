#include <stdio.h>
#include <stdlib.h>
#include <vips/vips.h>

#include "src/disjoint_set.h"
#include "src/seg.h"

char *make_grey_filename(const char *input, char *output, size_t out_size) {
  const char *dot = strrchr(input, '.');

  if (dot != NULL) {
    size_t base_len = dot - input;
    snprintf(output, out_size, "%.*s_grey%s", (int)base_len, input, dot);
  } else {
    snprintf(output, out_size, "%s_grey", input);
  }
  return output;
}

void prt_img_vector(VipsImage *img) {
  int w = vips_image_get_width(img);
  int h = vips_image_get_height(img);
  int b = vips_image_get_bands(img);

  size_t out_size = w * h;
  //   unsigned char *buffer = vips_image_write_to_memory(ready, &out_size);
  // vips_image_write_to_memory(ready, &out_size);
  printf("Width: %d, Height: %d, Pixels: %zu, Band: %d\n", w, h, out_size, b);

  unsigned char const *buf = vips_image_get_data(img);
  for (int i = 0; i < w; ++i) {
    for (int j = 0; j < h; ++j) {
      printf("%d ", buf[i * h + j]);
    }
    printf("\n");
  }
  //   g_free(buf);
}

int main(int argc, char *argv[argc + 1]) {
  if (VIPS_INIT(argv[0])) {
    vips_error_exit(NULL);
  }
  if (argc < 2) {
    fprintf(stderr, "None of input files\n");
    exit(EXIT_FAILURE);
  }
  VipsImage *img = vips_image_new_from_file(argv[1], NULL);
  if (!img) vips_error_exit(NULL);

  VipsImage *ready = img;

  size_t new_file_len = strlen(argv[1]) + 20;
  char new_file_name[new_file_len];

  // transform img to gray ray if input img is colorful
  if (vips_image_get_bands(img) != VIPS_INTERPRETATION_B_W) {
    if (vips_colourspace(img, &ready, VIPS_INTERPRETATION_B_W, NULL) ||
        vips_image_write_to_file(
            ready, make_grey_filename(argv[1], new_file_name, new_file_len),
            NULL))
      vips_error_exit(NULL);
  }

  size_t const N = vips_image_get_width(ready) * vips_image_get_height(ready);
  disjoint_set *ds = ALLOC_DISJOINT_SET(N + 1);

  int pixels_diff = argc > 2 ? atoi(argv[2]) : 5;
  image_seg(ready, ds, seg_merge_criterion, pixels_diff);
  if (img != ready) g_object_unref(img);
  g_object_unref(ready);
  vips_shutdown();
  free_disjoint_set(ds);
  return 0;
}
