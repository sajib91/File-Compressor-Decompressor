
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.c"




void encodeHuffman(Node* root, char* code, char data, int top) {
    if (root->data == data) {
        code[top] = '\0';
        return;
    }

    if (root->left) {
        code[top] = '0';
        encodeHuffman(root->left, code, data, top + 1);
    }

    if (root->right) {
        code[top] = '1';
        encodeHuffman(root->right, code, data, top + 1);
    }
}
void compressHuffman(const char* inputFilename, const char* outputFilename) {
    FILE* input = fopen(inputFilename, "r");
    if (input == NULL) {
        printf("Error opening the input file.\n");
        return;
    }

    int char_freq[256] = { 0 };
    char c;
    while ((c = fgetc(input)) != EOF) {
        char_freq[c]++;
    }

    int n = 0;
    Node* nodes[256];
    for (int i = 0; i < 256; i++) {
        if (char_freq[i] > 0) {
            nodes[n] = createNode((char)i, char_freq[i]);
            n++;
        }
    }

    buildHuffmanTree(nodes, n);

    Node* root = nodes[0];

    char code[256];
    char data;
    int top = 0;

    FILE* output = fopen(outputFilename, "wb");
    if (output == NULL) {
        printf("Error opening the output file.\n");
        fclose(input);
        return;
    }

    fseek(input, 0, SEEK_SET);

    while ((c = fgetc(input)) != EOF) {
        data = (char)c;
        encodeHuffman(root, code, data, top);
        fprintf(output, "%s", code);
    }

    fclose(input);
    fclose(output);
}