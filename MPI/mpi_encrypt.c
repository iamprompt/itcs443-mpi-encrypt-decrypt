#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Returns file size in bytes
int fileSize(FILE *file) {
  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  fseek(file, 0, SEEK_SET);
  return size;
}

// Returns destination file name with prefix
char *destFileName(char *fileName) {
  char *oPrefix = "C_";
  char *oFileName = malloc(sizeof(char) * (strlen(fileName) + strlen(oPrefix) + 1));
  strcpy(oFileName, oPrefix);
  strcat(oFileName, fileName);
  return oFileName;
}

// Operates XOR
void xorOperation(char *destBuffer, char *srcBuffer, int size, int key) {
  for (int i = 0; i < size; i++) {
    destBuffer[i] = srcBuffer[i] ^ key;
    // printf("%d : %d %d\n", i, srcBuffer[i], destBuffer[i]);
  }
}

int main(int argc, char **argv) {
  int processId;
  int numOfProcesses;
  double start, end;
  int size;
  char *oFileName;
  FILE *inputFile;

  int key = atoi(argv[1]);
  char *fileName = argv[2];

  char *buffer;
  char *operatedBuffer;

  MPI_Status status;

  MPI_Init(&argc, &argv);                          // Initialize MPI Program
  MPI_Comm_rank(MPI_COMM_WORLD, &processId);       // Get Current Process ID
  MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);  // Get a size of processes requested by the command -np

  printf("%d\n", processId);
  printf("%d\n", numOfProcesses);

  if (processId == 0) {
    oFileName = destFileName(fileName);

    inputFile = fopen(fileName, "rb");
    if (!inputFile) {
      fprintf(stderr, "Unable to open file\n");
      return -1;
    }

    size = fileSize(inputFile);
    printf("Size: %d\n", size);

    // buffer = malloc(sizeof(char) * size);
    operatedBuffer = malloc(sizeof(char) * size);

    fread(&buffer, size, 1, inputFile);

    // for (int i = 0; i < size; i++) {
    //   printf("%d : %d\n", i, buffer[i]);
    // }

    fclose(inputFile);
  }

  MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

  start = MPI_Wtime();

  int localMaxArraySize = size % numOfProcesses == 0 ? size / numOfProcesses : (size / numOfProcesses) + 1;
  int lastLocalSize = size - (localMaxArraySize * (numOfProcesses - 1));
  char *localBuffer = malloc(sizeof(char) * localMaxArraySize);

  MPI_Scatter(buffer, localMaxArraySize, MPI_CHAR, localBuffer, localMaxArraySize, MPI_CHAR, 0, MPI_COMM_WORLD);

  // xorOperation(operatedBuffer, buffer, localMaxArraySize, key);
  for (int i = 0; i < localMaxArraySize; i++) {
    // localBuffer[i] = localBuffer[i];
    printf("%d : %d\n", i, localBuffer[i]);
  }

  MPI_Gather(localBuffer, localMaxArraySize, MPI_CHAR, operatedBuffer, localMaxArraySize, MPI_CHAR, 0, MPI_COMM_WORLD);

  if (processId == 0) {
    FILE *outputFile = fopen(oFileName, "wb");
    if (!outputFile) {
      fprintf(stderr, "Unable to create file\n");
      return -1;
    }
    fwrite(&operatedBuffer, 1, size, outputFile);
    fclose(outputFile);

    free(buffer);
    free(operatedBuffer);
  }

  end = MPI_Wtime();

  printf("[Process %d] Execution time: %f seconds with %d processes and %d random integers.\n", processId, end - start, numOfProcesses, localMaxArraySize);

  MPI_Finalize();  // Finalize MPI Program
  return 0;
}