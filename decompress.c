
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "huffman.c"





void decodeHuffman(FILE* file, Node* root, FILE* output){
    Node* current = root;
    int bit;
    while ((bit = fgetc(file)) != EOF) {
        unsigned char mask = 0x80; // Start with the highest bit
        for (int i = 0; i < 8; i++) {
            if (current->left == NULL && current->right == NULL) {
                fputc(current->data, output); // Write the decoded character
                current = root; // Reset to the root for the next character
            }
            if (bit & mask) // Check the current bit
                current = current->right;
            else
                current = current->left;
            mask >>= 1; // Move to the next bit
        }
    }
    // Handle the last character if current is at a leaf node
    if (current->left == NULL && current->right == NULL) {
        fputc(current->data, output);
    }
}