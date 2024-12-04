#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_FILE "input.txt" 
#define OUTPUT_FILE "output.huff" 

typedef struct Node {
    char data;
    int freq;
    struct Node* left;
    struct Node* right;
} Node;

Node* createNode(char data, int freq) {
    Node* temp = (Node*)malloc(sizeof(Node));
    temp->left = NULL;
    temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}
Node* createNode1(char data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->left = newNode->right = NULL;
    return newNode;
}

void printNodes(Node* node, char* code, int top) {
    if (node->left) {
        code[top] = '0';
        printNodes(node->left, code, top + 1);
    }

    if (node->right) {
        code[top] = '1';
        printNodes(node->right, code, top + 1);
    }

    if (!(node->left) && !(node->right)) {
        printf("%c: ", node->data);
        for (int i = 0; i < top; i++) {
            printf("%c", code[i]);
        }
        printf("\n");
    }
}

void buildHuffmanTree(Node* nodes[], int n) {
    while (n > 1) {
        Node* left = nodes[n - 1];
        Node* right = nodes[n - 2];

        Node* parent = createNode('$', left->freq + right->freq);
        parent->left = left;
        parent->right = right;

        int i = n - 3;
        while (i >= 0 && nodes[i]->freq < parent->freq) {
            nodes[i + 1] = nodes[i];
            i--;
        }

        nodes[i + 1] = parent;
        n--;
    }
}
Node* buildHuffmanTree1(FILE* file) {
    char ch;
    if (fscanf(file, "%c", &ch) != 1) return NULL;

    if (ch == 'I') { // Internal node
        Node* node = createNode1('\0'); // Internal nodes have no data
        node->left = buildHuffmanTree1(file);
        node->right = buildHuffmanTree1(file);
        return node;
    } else if (ch == 'L') { // Leaf node
        fscanf(file, "%c", &ch); // Read the character
        return createNode1(ch);
    }
    return NULL;
}

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


long getFileSize(FILE* file) {
    long size;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    return size;
}

void printFileSize(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    long size = getFileSize(file);
    printf("%s size: %ld bytes\n", filename, size);

    fclose(file);
}


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