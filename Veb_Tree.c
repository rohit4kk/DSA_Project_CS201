#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

#define NULL_VAL -1

typedef struct vEBTree {
    int universe_size;
    int min_val;
    int max_val;
    struct vEBTree *summary;
    struct vEBTree **clusters;
} vEBTree;

int high(int x, int size) {
    int sqrt_size = (int)sqrt(size);
    return x / sqrt_size;
}

int low(int x, int size) {
    int sqrt_size = (int)sqrt(size);
    return x % sqrt_size;
}

int combine(int high_val, int low_val, int size) {
    int sqrt_size = (int)sqrt(size);
    return high_val * sqrt_size + low_val;
}

vEBTree *createVEBTree(int size) {
    vEBTree *tree = (vEBTree *)malloc(sizeof(vEBTree));
    tree->universe_size = size;
    tree->min_val = NULL_VAL;
    tree->max_val = NULL_VAL;

    if (size > 2) {
        int sqrt_size = (int)sqrt(size);
        tree->summary = createVEBTree(sqrt_size);
        tree->clusters = (vEBTree **)malloc(sqrt_size * sizeof(vEBTree *));
        for (int i = 0; i < sqrt_size; i++) {
            tree->clusters[i] = createVEBTree(sqrt_size);
        }
    } else {
        tree->summary = NULL;
        tree->clusters = NULL;
    }

    return tree;
}

void insert(vEBTree *tree, int val) {
    if (tree->min_val == NULL_VAL) {
        tree->min_val = tree->max_val = val;
    } else {
        if (val < tree->min_val) {
            int temp = tree->min_val;
            tree->min_val = val;
            val = temp;
        }

        if (tree->universe_size > 2) {
            int high_val = high(val, tree->universe_size);
            int low_val = low(val, tree->universe_size);

            if (tree->clusters[high_val]->min_val == NULL_VAL) {
                insert(tree->summary, high_val);
                tree->clusters[high_val]->min_val = tree->clusters[high_val]->max_val = low_val;
            } else {
                insert(tree->clusters[high_val], low_val);
            }
        }

        if (val > tree->max_val) {
            tree->max_val = val;
        }
    }
}

