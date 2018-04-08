#include <png.h>
#include "mpi.h"
#include "pngio.h"

int main(int argc, char *argv[]) {
   if(argc != 3) abort();

   const int conv[3][3] = {{-2,-1,0},{-1,1,1},{0,1,2}};
   const int DIVISOR = 1;
   int mi_rank, p_size, chunk_size, my_width, my_height;
   double t_inicio = 0.0,
          t_final = 0.0;

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &mi_rank);
   MPI_Comm_size(MPI_COMM_WORLD, &p_size);

   int *R, *G, *B;
   int *R_cop, *G_cop, *B_cop;

   if(mi_rank == 0){
      /* Lectura de la imagen en una matriz bytep */
      dimension dim = read_png_file(argv[1]);

      my_width = dim.width;
      my_height = dim.height;

      /*
         Paso de la matriz original a 3 matrices para
         cada Canal
      */

      R = new int[(dim.height+2)*(dim.width+2)],
      G = new int[(dim.height+2)*(dim.width+2)],
      B = new int[(dim.height+2)*(dim.width+2)];

      conversion_a_RGBAMatrices(R, G, B);

      chunk_size = ((dim.height+2)*(dim.width+2)) / p_size;

      for(int i = 1; i < p_size; ++i){
         MPI_Send(&dim.width,1,MPI_INT,i,0,MPI_COMM_WORLD);
         MPI_Send(&dim.height,1,MPI_INT,i,0,MPI_COMM_WORLD);

         MPI_Send(R+((i*chunk_size)-my_width),chunk_size+(my_width*2),MPI_INT,i,0,MPI_COMM_WORLD);
         MPI_Send(G+((i*chunk_size)-my_width),chunk_size+(my_width*2),MPI_INT,i,0,MPI_COMM_WORLD);
         MPI_Send(B+((i*chunk_size)-my_width),chunk_size+(my_width*2),MPI_INT,i,0,MPI_COMM_WORLD);

      }

      /*
         Creación de una matriz copia para no sobrescribir
         la original.
      */

      R_cop = new int[(my_height+2)*(my_width+2)];
      G_cop = new int[(my_height+2)*(my_width+2)];
      B_cop = new int[(my_height+2)*(my_width+2)];
   }

   /*
      Si no es el proceso 0
   */

   else {
      MPI_Status status;

      // Recibo de la altura y anchura de la imagen
      MPI_Recv(&my_width, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
      MPI_Recv(&my_height, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD,&status);

      chunk_size = ((my_height+2)*(my_width+2)) / p_size;

      R = new int[chunk_size+my_width*2];
      G = new int[chunk_size+my_width*2];
      B = new int[chunk_size+my_width*2];

      MPI_Recv(R,chunk_size+my_width*2,MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
      MPI_Recv(G,chunk_size+my_width*2,MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD,&status);
      MPI_Recv(B,chunk_size+my_width*2,MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD,&status);

      /*
         Creación de una matriz copia para no sobrescribir
         la original.
      */

      R_cop = new int[chunk_size+my_width*2];
      G_cop = new int[chunk_size+my_width*2];
      B_cop = new int[chunk_size+my_width*2];
   }

   if(mi_rank == 0)
      t_inicio = MPI_Wtime();

   /* Realización del proceso de convolucion */

   for(int i = 1; i <= (my_height/p_size)+1; ++i){
      for(int j = 1; j <= my_width; ++j){
         for(int k = -1; k <= 1; ++k){
            R_cop[(i*my_width)+j] += R[((i-1)*my_width)+(j+k)] * conv[0][k+1] + R[((i)*my_width)+(j+k)] * conv[1][k+1] + R[((i+1)*my_width)+(j+k)] * conv[2][k+1];
            G_cop[(i*my_width)+j] += G[((i-1)*my_width)+(j+k)] * conv[0][k+1] + G[((i)*my_width)+(j+k)] * conv[1][k+1] + G[((i+1)*my_width)+(j+k)] * conv[2][k+1];
            B_cop[(i*my_width)+j] += B[((i-1)*my_width)+(j+k)] * conv[0][k+1] + B[((i)*my_width)+(j+k)] * conv[1][k+1] + B[((i+1)*my_width)+(j+k)] * conv[2][k+1];
         }

         R_cop[(i*my_width)+j] /= DIVISOR;
         G_cop[(i*my_width)+j] /= DIVISOR;
         B_cop[(i*my_width)+j] /= DIVISOR;
      }
   }

   if(mi_rank == 0)
      t_final = MPI_Wtime();

   /*
      Recepción de los datos
   */

   if(mi_rank == 0){
      MPI_Status status;

      for(int i = 1; i < p_size; ++i){
         MPI_Recv(R_cop+((i*chunk_size)-my_width),chunk_size+my_width,MPI_INT,i,0,MPI_COMM_WORLD, &status);
         MPI_Recv(G_cop+((i*chunk_size)-my_width),chunk_size+my_width,MPI_INT,i,0,MPI_COMM_WORLD, &status);
         MPI_Recv(B_cop+((i*chunk_size)-my_width),chunk_size+my_width,MPI_INT,i,0,MPI_COMM_WORLD, &status);
      }
   }
   else {
      MPI_Send(R_cop+my_width,chunk_size,MPI_INT, 0, 0, MPI_COMM_WORLD);
      MPI_Send(G_cop+my_width,chunk_size,MPI_INT, 0, 0, MPI_COMM_WORLD);
      MPI_Send(B_cop+my_width,chunk_size,MPI_INT, 0, 0, MPI_COMM_WORLD);
   }

   if(mi_rank == 0){
      /*
         Conversión de las 3 matrices de vuelta a una
         matriz de representación de imagenes
      */

      conversion_a_Imagen(R_cop, G_cop, B_cop);


      /* Escritura de la matriz en un fichero .png */

      write_png_file(argv[2]);
   }

   if(mi_rank == 0)
      printf("Problem size: %i x %i\tElapsed time: %f\n", my_width, my_height, t_final - t_inicio);

   MPI_Finalize();

   return 0;
}
