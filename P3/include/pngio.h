#include <stdlib.h>
#include <stdio.h>
#include <png.h>

struct dimension{
   int width, height;
};

int width, height;
png_byte color_type;
png_byte bit_depth;
png_bytep *row_pointers;

dimension read_png_file(char *filename) {
  FILE *fp = fopen(filename, "rb");

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png) abort();

  png_infop info = png_create_info_struct(png);
  if(!info) abort();

  if(setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);

  png_read_info(png, info);

  width      = png_get_image_width(png, info);
  height     = png_get_image_height(png, info);
  color_type = png_get_color_type(png, info);
  bit_depth  = png_get_bit_depth(png, info);

  // Read any color_type into 8bit depth, RGBA format.
  // See http://www.libpng.org/pub/png/libpng-manual.txt

  if(bit_depth == 16)
    png_set_strip_16(png);

  if(color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png);

  // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
  if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand_gray_1_2_4_to_8(png);

  if(png_get_valid(png, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png);

  // These color_type don't have an alpha channel then fill it with 0xff.
  if(color_type == PNG_COLOR_TYPE_RGB ||
     color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

  if(color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png);

  png_read_update_info(png, info);

  row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
  for(int y = 0; y < height; y++) {
    row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
  }

  png_read_image(png, row_pointers);

  fclose(fp);

  dimension dim;

  dim.height = height;
  dim.width = width;

  return dim;
}

void write_png_file(char *filename) {
  int y;

  FILE *fp = fopen(filename, "wb");
  if(!fp) abort();

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) abort();

  png_infop info = png_create_info_struct(png);
  if (!info) abort();

  if (setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);

  // Output is 8bit depth, RGBA format.
  png_set_IHDR(
    png,
    info,
    width, height,
    8,
    PNG_COLOR_TYPE_RGBA,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );
  png_write_info(png, info);

  // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
  // Use png_set_filler().
  //png_set_filler(png, 0, PNG_FILLER_AFTER);

  png_write_image(png, row_pointers);
  png_write_end(png, NULL);

  for(int y = 0; y < height; y++) {
    free(row_pointers[y]);
  }
  free(row_pointers);

  fclose(fp);
}

/* Traspaso de matriz original a tres matrices */

void conversion_a_RGBAMatrices(int * R, int *G, int *B){
   for(int i = 0; i <= height+1; ++i){
      for(int j = 0; j <= width+1; ++j){
         if(i == 0 || j == 0 || i == height+1 || j == width+1){
            R[(i*width)+j] = G[(i*width)+j] = B[(i*width)+j] = 0;
         }
         else{
            png_bytep row = row_pointers[i-1];
            png_bytep px = &(row[(j-1)*4]);

            R[(i*width)+j] = px[0];
            G[(i*width)+j] = px[1];
            B[(i*width)+j] = px[2];
         }
      }
   }

}

void conversion_a_Imagen(int * R, int *G, int *B){
   for(int i = 0; i < height; ++i){
      png_bytep row = row_pointers[i];

      for(int j = 0; j < width; ++j){
            png_bytep px = &(row[j*4]);

            px[0] = abs(R[((i+1)*width)+(j+1)]);
            px[1] = abs(G[((i+1)*width)+(j+1)]);
            px[2] = abs(B[((i+1)*width)+(j+1)]);
      }
   }
}

void conversion_a_RGBAMatrices(char * R, char *G, char *B){
   for(int i = 0; i <= height+1; ++i){
      for(int j = 0; j <= width+1; ++j){
         if(i == 0 || j == 0 || i == height+1 || j == width+1){
            R[(i*width)+j] = G[(i*width)+j] = B[(i*width)+j] = 0;
         }
         else{
            png_bytep row = row_pointers[i-1];
            png_bytep px = &(row[(j-1)*4]);

            R[(i*width)+j] = px[0];
            G[(i*width)+j] = px[1];
            B[(i*width)+j] = px[2];
         }
      }
   }

}

void conversion_a_Imagen(char * R, char *G, char *B){
   for(int i = 0; i < height; ++i){
      png_bytep row = row_pointers[i];

      for(int j = 0; j < width; ++j){
            png_bytep px = &(row[j*4]);

            px[0] = abs(R[((i+1)*width)+(j+1)]);
            px[1] = abs(G[((i+1)*width)+(j+1)]);
            px[2] = abs(B[((i+1)*width)+(j+1)]);
      }
   }
}
