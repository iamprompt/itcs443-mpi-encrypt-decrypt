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
    printf("%d : %d %d\n", i, srcBuffer[i], destBuffer[i]);
  }
}

void operated(char *iFileName, int key) {
  char *oFileName = destFileName(iFileName);
  FILE *inputFile = fopen(iFileName, "rb");
  if (!inputFile) {
    fprintf(stderr, "Unable to open file\n");
    return;
  }

  FILE *outputFile = fopen(oFileName, "wb");
  if (!outputFile) {
    fprintf(stderr, "Unable to create file\n");
    return;
  }

  int size = fileSize(inputFile);

  char buffer[size];
  char operatedBuffer[size];

  printf("Size: %d\n", size);

  fread(&buffer, size, 1, inputFile);
  fclose(inputFile);

  xorOperation(operatedBuffer, buffer, size, key);

  fwrite(&operatedBuffer, 1, size, outputFile);
  fclose(outputFile);
}

int main(int argc, char **argv) {
  // Ask for key to encrypt/decrypt
  int key;
  printf("Enter key: ");
  scanf("%d", &key);

  // Ask for file name
  char fileName[100];
  printf("Enter file name: ");
  scanf("%s", fileName);

  // Operate on file
  operated(fileName, key);
  return 0;
}