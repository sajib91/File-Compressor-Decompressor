
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "huffman.c"
#include "compress.c"
#include "decompress.c"
#define INPUT_FILE "input.txt" 
#define OUTPUT_FILE "output.huff" 

int main() {

    const char* file_path = INPUT_FILE ;

    FILE* file = fopen(file_path, "r");

    if (file == NULL) {
        perror("Failed to open the file");
        return 1;
    }

    int charCount = 0;
    int charFrequency[256] = {0};

    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if (ferror(file)) {
            perror("Error reading from the file");
            fclose(file);
            return 1;
        }

        charCount++;
        charFrequency[ch]++;
    }

    if (fclose(file) == EOF) {
        perror("Error closing the file");
        return 1;
    }

    printf("Total characters: %d\n", charCount);
    printf("Character frequencies:\n");

    for (int i = 0; i < 256; i++) {
        if (charFrequency[i] > 0) {
            printf("Character '%c' (ASCII %d): %d\n", (char)i, i, charFrequency[i]);
        }
    }


    printf("Huffman Compression\n");

    compressHuffman(INPUT_FILE, OUTPUT_FILE);

    printf("The file has been compressed and saved as %s\n", OUTPUT_FILE);

    printFileSize(INPUT_FILE);
    printFileSize(OUTPUT_FILE);
    printf("\n\n");
 
    FILE* input = fopen(OUTPUT_FILE, "rb");
    FILE* output = fopen("decompressed.txt", "w");
    if (!input || !output) {
        perror("Error opening files");
        return EXIT_FAILURE;
    }

    // Step 1: Build the Huffman Tree
    Node* root = buildHuffmanTree1(input);

    // Step 2: Decode the compressed data
    decodeHuffman(input, root, output);

    fclose(input);
    fclose(output);

    printf("Decompression complete. Output saved to 'decompressed.txt'.\n");

    return 0;
}