#include <png.h>
#include <chrono>
#include "pngio.h"

int main(int argc, char *argv[]) {
   if(argc != 3) abort();

   const int conv[3][3] = {{-2,-1,0},{-1,1,1},{0,1,2}};
   const int DIVISOR = 1;

   /* Lectura de la imagen en una matriz bytep */
   dimension dim = read_png_file(argv[1]);

   /*
      Paso de la matriz original a 3 matrices para
      cada Canal
   */

   int * R = new int[(dim.height+2)*(dim.width+2)],
       * G = new int[(dim.height+2)*(dim.width+2)],
       * B = new int[(dim.height+2)*(dim.width+2)];

   conversion_a_RGBAMatrices(R, G, B);

   /*
      Creación de una matriz copia para no sobrescribir
      la original.
   */

   int * R_cop = new int[(height+2)*(width+2)],
       * G_cop = new int[(height+2)*(width+2)],
       * B_cop = new int[(height+2)*(width+2)];

   std::chrono::high_resolution_clock::time_point t_inicio = std::chrono::high_resolution_clock::now();

   /* Realización del proceso de convolucion */

   for(int i = 2; i < height; ++i){
      for(int j = 2; j < width; ++j){
         R_cop[(i*dim.width)+j] += R[((i-1)*dim.width)+(j-1)] * conv[0][0] + R[((i)*dim.width)+(j-1)] * conv[1][0] + R[((i+1)*dim.width)+(j-1)] * conv[2][0];
         G_cop[(i*dim.width)+j] += G[((i-1)*dim.width)+(j-1)] * conv[0][0] + G[((i)*dim.width)+(j-1)] * conv[1][0] + G[((i+1)*dim.width)+(j-1)] * conv[2][0];
         B_cop[(i*dim.width)+j] += B[((i-1)*dim.width)+(j-1)] * conv[0][0] + B[((i)*dim.width)+(j-1)] * conv[1][0] + B[((i+1)*dim.width)+(j-1)] * conv[2][0];

         R_cop[(i*dim.width)+j] += R[((i-1)*dim.width)+(j)] * conv[0][1] + R[((i)*dim.width)+(j)] * conv[1][1] + R[((i+1)*dim.width)+(j)] * conv[2][1];
         G_cop[(i*dim.width)+j] += G[((i-1)*dim.width)+(j)] * conv[0][1] + G[((i)*dim.width)+(j)] * conv[1][1] + G[((i+1)*dim.width)+(j)] * conv[2][1];
         B_cop[(i*dim.width)+j] += B[((i-1)*dim.width)+(j)] * conv[0][1] + B[((i)*dim.width)+(j)] * conv[1][1] + B[((i+1)*dim.width)+(j)] * conv[2][1];

         R_cop[(i*dim.width)+j] += R[((i-1)*dim.width)+(j+1)] * conv[0][2] + R[((i)*dim.width)+(j+1)] * conv[1][2] + R[((i+1)*dim.width)+(j+1)] * conv[2][2];
         G_cop[(i*dim.width)+j] += G[((i-1)*dim.width)+(j+1)] * conv[0][2] + G[((i)*dim.width)+(j+1)] * conv[1][2] + G[((i+1)*dim.width)+(j+1)] * conv[2][2];
         B_cop[(i*dim.width)+j] += B[((i-1)*dim.width)+(j+1)] * conv[0][2] + B[((i)*dim.width)+(j+1)] * conv[1][2] + B[((i+1)*dim.width)+(j+1)] * conv[2][2];


         R_cop[(i*width)+j] /= DIVISOR;
         G_cop[(i*width)+j] /= DIVISOR;
         B_cop[(i*width)+j] /= DIVISOR;
      }
   }

   std::chrono::high_resolution_clock::time_point t_fin = std::chrono::high_resolution_clock::now();

   std::chrono::duration<double> total_time = std::chrono::duration_cast<std::chrono::duration<double> >(t_fin - t_inicio);

   /*
      Conversión de las 3 matrices de vuelta a una
      matriz de representación de imagenes
   */

   conversion_a_Imagen(R_cop, G_cop, B_cop);

   /* Escritura de la matriz en un fichero .png */

   write_png_file(argv[2]);

   printf("Problem size: %i x %i\tElapsed time: %f\n", dim.width, dim.height, total_time);

   return 0;
}
