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

int main(int argc, char **argv) {
  int processId;
  int numOfProcesses;
  double start, end;
  int size;
  int i;

  int key = atoi(argv[1]);
  char *fileName = argv[2];

  char *oFileName = destFileName(fileName);
  FILE *inputFile = fopen(fileName, "rb");

  if (!inputFile) {
    fprintf(stderr, "Unable to open file\n");
    return -1;
  }

  size = fileSize(inputFile);

  char buffer[size];
  char operatedBuffer[size];

  fread(&buffer, size, 1, inputFile);
  fclose(inputFile);

  MPI_Status status;

  MPI_Init(&argc, &argv);                          // Initialize MPI Program
  MPI_Comm_rank(MPI_COMM_WORLD, &processId);       // Get Current Process ID
  MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);  // Get a size of processes requested by the command -np

  start = MPI_Wtime();

  int localMaxArraySize = size % numOfProcesses == 0 ? size / numOfProcesses : (size / numOfProcesses) + 1;
  char localBuffer[localMaxArraySize];

  MPI_Scatter(buffer, localMaxArraySize, MPI_CHAR, &localBuffer, localMaxArraySize, MPI_CHAR, 0, MPI_COMM_WORLD);
  printf("[Process %d] Starting Encryption/Decryption with Xor Operation\n", processId);
  for (i = 0; i < size; i++) {
    localBuffer[i] = localBuffer[i] ^ key;
  }
  printf("[Process %d] Finishing Encryption/Decryption with Xor Operation\n", processId);
  MPI_Gather(&localBuffer, localMaxArraySize, MPI_CHAR, operatedBuffer, localMaxArraySize, MPI_CHAR, 0, MPI_COMM_WORLD);

  if (processId == 0) {
    FILE *outputFile = fopen(oFileName, "wb");
    if (!outputFile) {
      fprintf(stderr, "Unable to create file\n");
      return -1;
    }
    fwrite(&operatedBuffer, 1, size, outputFile);
    fclose(outputFile);
  }

  end = MPI_Wtime();

  printf("[Process %d] Execution time: %f seconds with %d processes.\n", processId, end - start, numOfProcesses);

  MPI_Finalize();  // Finalize MPI Program
  return 0;
}