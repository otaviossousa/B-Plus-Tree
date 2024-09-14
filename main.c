// Source code: https://www.geeksforgeeks.org/implementation-of-b-plus-tree-in-c/
// C Program to Implement B+ Tree
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN_DEGREE 3 // Minimum degree (defines the range for number of keys)

typedef struct Node {
    // Array of keys
    int* keys;
    // Minimum degree (defines the range for number of keys)
    int t;
    // Array of child pointers
    struct Node** children;
    // Current number of keys
    int n;
    // To determine whether the node is leaf or not
    bool leaf;
    // Pointer to next leaf node
    struct Node* next;
} Node;

typedef struct BTree {
    // Pointer to root node
    Node* root;
    // Minimum degree
    int t;
} BTree;

/* Pseudo code for create node 

CREATE-NODE(t, leaf)
1. x = ALLOCATE-NODE()
2. x.t = t
3. x.leaf = leaf
4. x.keys = ALLOCATE-ARRAY(2 * t - 1)
5. x.children = ALLOCATE-ARRAY(2 * t)
6. x.n = 0
7. x.next = NULL
8. RETURN x

*/

// Function to create a new B+ tree node
Node* createNode(int t, bool leaf)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->t = t;
    newNode->leaf = leaf;
    newNode->keys = (int*)malloc((2 * t - 1) * sizeof(int));
    newNode->children
        = (Node**)malloc((2 * t) * sizeof(Node*));
    newNode->n = 0;
    newNode->next = NULL;
    return newNode;
}

/* Pseudo code for create tree

CREATE-BTREE(t)
1. T = ALLOCATE-BTREE()
2. T.t = t
3. T.root = CREATE-NODE(t, TRUE)
4. DISK-WRITE(T.root)
5. RETURN T

*/

// Function to create a new B+ tree
BTree* createBTree(int t)
{
    BTree* btree = (BTree*)malloc(sizeof(BTree));
    btree->t = t;
    btree->root = createNode(t, true);
    return btree;
}

// Function to display the B+ tree and print its keys
void display(Node* node)
{
    if (node == NULL)
        return;
    int i;
    for (i = 0; i < node->n; i++) {
        if (!node->leaf) {
            display(node->children[i]);
        }
        printf("%d ", node->keys[i]);
    }
    if (!node->leaf) {
        display(node->children[i]);
    }
}

/* Pseudocode for search

SEARCH(node, key)
1. i = 0
2. WHILE i < node.n AND key > node.keys[i]
3.    i = i + 1
4. IF i < node.n AND key = node.keys[i]
5.    RETURN TRUE
6. IF node.leaf
7.    RETURN FALSE
8. RETURN SEARCH(node.children[i], key)

*/

// Function to search a key in the B+ tree
bool search(Node* node, int key)
{
    int i = 0;
    while (i < node->n && key > node->keys[i]) {
        i++;
    }
    if (i < node->n && key == node->keys[i]) {
        return true;
    }
    if (node->leaf) {
        return false;
    }
    return search(node->children[i], key);
}

/* Pseudocode to splitchild 

SPLIT-CHILD(parent, i, child)
1. t = child.t
2. newChild = CREATE-NODE(t, child.leaf)
3. newChild.n = t - 1
4. FOR j = 0 TO t - 2
5.    newChild.keys[j] = child.keys[j + t]
6.
7. IF NOT child.leaf
8.    FOR j = 0 TO t - 1
9.        newChild.children[j] = child.children[j + t]
10.
11. child.n = t - 1
12.
13. FOR j = parent.n DOWNTO i + 1
14.    parent.children[j + 1] = parent.children[j]
15.
16. parent.children[i + 1] = newChild
17.
18. FOR j = parent.n - 1 DOWNTO i
19.    parent.keys[j + 1] = parent.keys[j]
20.
21. parent.keys[i] = child.keys[t - 1]
22. parent.n = parent.n + 1

*/

// Function to split the child of a node during insertion
void splitChild(Node* parent, int i, Node* child)
{
    int t = child->t;
    Node* newChild = createNode(t, child->leaf);
    newChild->n = t - 1;

    for (int j = 0; j < t - 1; j++) {
        newChild->keys[j] = child->keys[j + t];
    }

    if (!child->leaf) {
        for (int j = 0; j < t; j++) {
            newChild->children[j] = child->children[j + t];
        }
    }

    child->n = t - 1;

    for (int j = parent->n; j >= i + 1; j--) {
        parent->children[j + 1] = parent->children[j];
    }
    parent->children[i + 1] = newChild;

    for (int j = parent->n - 1; j >= i; j--) {
        parent->keys[j + 1] = parent->keys[j];
    }
    parent->keys[i] = child->keys[t - 1];
    parent->n += 1;
}


/*Pseudocode for insertNonFull

INSERT-NONFULL(node, key)
1. i = node.n - 1
2. IF node.leaf
3.    WHILE i >= 0 AND node.keys[i] > key
4.        node.keys[i + 1] = node.keys[i]
5.        i = i - 1
6.    node.keys[i + 1] = key
7.    node.n = node.n + 1
8. ELSE
9.    WHILE i >= 0 AND node.keys[i] > key
10.        i = i - 1
11.    i = i + 1
12.    IF node.children[i].n = 2 * node.t - 1
13.        SPLIT-CHILD(node, i, node.children[i])
14.        IF node.keys[i] < key
15.            i = i + 1
16.    INSERT-NONFULL(node.children[i], key)


*/

// Function to insert a non-full node
void insertNonFull(Node* node, int key)
{
    int i = node->n - 1;

    if (node->leaf) {
        while (i >= 0 && node->keys[i] > key) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->n += 1;
    }
    else {
        while (i >= 0 && node->keys[i] > key) {
            i--;
        }
        i++;
        if (node->children[i]->n == 2 * node->t - 1) {
            splitChild(node, i, node->children[i]);
            if (node->keys[i] < key) {
                i++;
            }
        }
        insertNonFull(node->children[i], key);
    }
}

/* Pseudo code for insert

INSERT(btree, key)
1. root = btree.root
2.
3. IF root.n = 2 * btree.t - 1
4.    newRoot = CREATE-NODE(btree.t, FALSE)
5.    newRoot.children[0] = root
6.    SPLIT-CHILD(newRoot, 0, root)
7.    INSERT-NON-FULL(newRoot, key)
8.    btree.root = newRoot
9. ELSE
10.   INSERT-NON-FULL(root, key)

*/

// Function to insert a key into the B+ tree
void insert(BTree* btree, int key)
{
    Node* root = btree->root;
    if (root->n == 2 * btree->t - 1) {
        Node* newRoot = createNode(btree->t, false);
        newRoot->children[0] = root;
        splitChild(newRoot, 0, root);
        insertNonFull(newRoot, key);
        btree->root = newRoot;
    }
    else {
        insertNonFull(root, key);
    }
}

// Function prototypes for helper functions used in deleteKey
void deleteKeyHelper(Node* node, int key);
int findKey(Node* node, int key);
void removeFromLeaf(Node* node, int idx);
int getPredecessor(Node* node, int idx);
void fill(Node* node, int idx);
void borrowFromPrev(Node* node, int idx);
void borrowFromNext(Node* node, int idx);
void merge(Node* node, int idx);


/* Pseudo code for delete

DELETE-KEY(btree, key)
1. root = btree.root
2. DELETE-KEY-HELPER(root, key)
3. IF root.n = 0 AND NOT root.leaf
4.    btree.root = root.children[0]
5.    FREE(root)

*/

// Function for deleting a key from the B+ tree
void deleteKey(BTree* btree, int key)
{
    Node* root = btree->root;

    // Call a helper function to delete the key recursively
    deleteKeyHelper(root, key);

    // If root has no keys left and it has a child, make its
    // first child the new root
    if (root->n == 0 && !root->leaf) {
        btree->root = root->children[0];
        free(root);
    }
}


/* Pseudo code for delete helper

DELETE-KEY-HELPER(node, key)
1. i = 0
2. WHILE i < node.n AND key > node.keys[i]
3.    2.1 i = i + 1
4.
5. IF i < node.n AND key = node.keys[i]
6.    IF node.leaf
7.        FOR j = i TO node.n - 2
8.            node.keys[j] = node.keys[j + 1]
9.        node.n = node.n - 1
10.   ELSE
11.        IF node.children[i].n >= node.t
12.            DELETE-KEY-HELPER(node.children[i], key)
13.        ELSE
14.            IF i < node.n AND node.children[i + 1].n >= node.t
15.                DELETE-KEY-HELPER(node.children[i + 1], key)
16.            ELSE
17.                MERGE(node, i)
18.                DELETE-KEY-HELPER(node.children[i], key)

*/

// Helper function to recursively delete a key from the B+ tree
void deleteKeyHelper(Node* node, int key)
{
    int idx = findKey(
        node, key); // Find the index of the key in the node

    // If key is present in this node
    if (idx < node->n && node->keys[idx] == key) {
        if (node->leaf) {
            // If the node is a leaf, simply remove the key
            removeFromLeaf(node, idx);
        }
        else {
            // If the node is not a leaf, replace the key
            // with its predecessor/successor
            int predecessor = getPredecessor(node, idx);
            node->keys[idx] = predecessor;
            // Recursively delete the predecessor
            deleteKeyHelper(node->children[idx],
                            predecessor);
        }
    }
    else {
        // If the key is not present in this node, go down
        // the appropriate child
        if (node->leaf) {
            // Key not found in the tree
            printf("Key %d not found in the B+ tree.\n",
                   key);
            return;
        }

        bool isLastChild = (idx == node->n);

        // If the child where the key is supposed to be lies
        // has less than t keys, fill that child
        if (node->children[idx]->n < node->t) {
            fill(node, idx);
        }

        // If the last child has been merged, it must have
        // merged with the previous child

        // So, we need to recursively delete the key from
        // the previous child
        if (isLastChild && idx > node->n) {
            deleteKeyHelper(node->children[idx - 1], key);
        }
        else {
            deleteKeyHelper(node->children[idx], key);
        }
    }
}
// Function to find the index of a key in a node
int findKey(Node* node, int key)
{
    int idx = 0;
    while (idx < node->n && key > node->keys[idx]) {
        idx++;
    }
    return idx;
}

// Function to remove a key from a leaf node
void removeFromLeaf(Node* node, int idx)
{
    for (int i = idx + 1; i < node->n; ++i) {
        node->keys[i - 1] = node->keys[i];
    }
    node->n--;
}

// Function to get the predecessor of a key in a non-leaf
// node
int getPredecessor(Node* node, int idx)
{
    Node* curr = node->children[idx];
    while (!curr->leaf) {
        curr = curr->children[curr->n];
    }
    return curr->keys[curr->n - 1];
}

// Function to fill up the child node present at the idx-th
// position in the node node
void fill(Node* node, int idx)
{
    if (idx != 0 && node->children[idx - 1]->n >= node->t) {
        borrowFromPrev(node, idx);
    }
    else if (idx != node->n
             && node->children[idx + 1]->n >= node->t) {
        borrowFromNext(node, idx);
    }
    else {
        if (idx != node->n) {
            merge(node, idx);
        }
        else {
            merge(node, idx - 1);
        }
    }
}

// Function to borrow a key from the previous child and move
// it to the idx-th child
void borrowFromPrev(Node* node, int idx)
{
    Node* child = node->children[idx];
    Node* sibling = node->children[idx - 1];

    // Move all keys in child one step ahead
    for (int i = child->n - 1; i >= 0; --i) {
        child->keys[i + 1] = child->keys[i];
    }

    // If child is not a leaf, move its child pointers one
    // step ahead
    if (!child->leaf) {
        for (int i = child->n; i >= 0; --i) {
            child->children[i + 1] = child->children[i];
        }
    }

    // Setting child's first key equal to node's key[idx -
    // 1]
    child->keys[0] = node->keys[idx - 1];

    // Moving sibling's last child as child's first child
    if (!child->leaf) {
        child->children[0] = sibling->children[sibling->n];
    }

    // Moving the key from the sibling to the parent
    node->keys[idx - 1] = sibling->keys[sibling->n - 1];

    // Incrementing and decrementing the key counts of child
    // and sibling respectively
    child->n += 1;
    sibling->n -= 1;
}

// Function to borrow a key from the next child and move it
// to the idx-th child
void borrowFromNext(Node* node, int idx)
{
    Node* child = node->children[idx];
    Node* sibling = node->children[idx + 1];

    // Setting child's (t - 1)th key equal to node's
    // key[idx]
    child->keys[(child->n)] = node->keys[idx];

    // If child is not a leaf, move its child pointers one
    // step ahead
    if (!child->leaf) {
        child->children[(child->n) + 1]
            = sibling->children[0];
    }

    // Setting node's idx-th key equal to sibling's first
    // key
    node->keys[idx] = sibling->keys[0];

    // Moving all keys in sibling one step behind
    for (int i = 1; i < sibling->n; ++i) {
        sibling->keys[i - 1] = sibling->keys[i];
    }

    // If sibling is not a leaf, move its child pointers one
    // step behind
    if (!sibling->leaf) {
        for (int i = 1; i <= sibling->n; ++i) {
            sibling->children[i - 1] = sibling->children[i];
        }
    }

    // Incrementing and decrementing the key counts of child
    // and sibling respectively
    child->n += 1;
    sibling->n -= 1;
}


/* Pseudo code for merge

MERGE(parent, i)
1. t = parent.t
2. child = parent.children[i]
3. sibling = parent.children[i + 1]
4. child.keys[t - 1] = parent.keys[i]
5. FOR j = 0 TO sibling.n - 1
6.    child.keys[j + t] = sibling.keys[j]
7. IF NOT child.leaf
8.    FOR j = 0 TO sibling.n
9.        child.children[j + t] = sibling.children[j]
10.
11. FOR j = i + 1 TO parent.n - 1
12.    parent.keys[j - 1] = parent.keys[j]
13.    parent.children[j] = parent.children[j + 1]
14.
15. parent.n = parent.n - 1
16. child.n = child.n + sibling.n + 1
17.
18. FREE(sibling)

*/

// Function to merge idx-th child of node with (idx + 1)-th
// child of node
void merge(Node* node, int idx)
{
    Node* child = node->children[idx];
    Node* sibling = node->children[idx + 1];

    // Pulling a key from the current node and inserting it
    // into (t-1)th position of child
    child->keys[child->n] = node->keys[idx];

    // If child is not a leaf, move its child pointers one
    // step ahead
    if (!child->leaf) {
        child->children[child->n + 1]
            = sibling->children[0];
    }

    // Copying the keys from sibling to child
    for (int i = 0; i < sibling->n; ++i) {
        child->keys[i + child->n + 1] = sibling->keys[i];
    }

    // If child is not a leaf, copy the children pointers as
    // well
    if (!child->leaf) {
        for (int i = 0; i <= sibling->n; ++i) {
            child->children[i + child->n + 1]
                = sibling->children[i];
        }
    }

    // Move all keys after idx in the current node one step
    // before, so as to fill the gap created by moving
    // keys[idx] to child
    for (int i = idx + 1; i < node->n; ++i) {
        node->keys[i - 1] = node->keys[i];
    }

    // Move the child pointers after (idx + 1) in the
    // current node one step before
    for (int i = idx + 2; i <= node->n; ++i) {
        node->children[i - 1] = node->children[i];
    }

    // Update the key count of child and current node
    child->n += sibling->n + 1;
    node->n--;

    // Free the memory occupied by sibling
    free(sibling);
}

/* Pseudocode for printTree

PRINTTREE(node, level)
1. IF node = NULL
2.    RETURN
3. FOR i = 0 TO level - 1
4.    PRINT "  "
5. PRINT "["
6. FOR i = 0 TO node.n - 1
7.    PRINT node.keys[i] + " "
8. PRINT "]"
9. PRINT newline
10. IF NOT node.leaf
11.    FOR i = 0 TO node.n
12.        PRINTTREE(node.children[i], level + 1)

*/

// Function to visualize the B+ tree structure
void printTree(Node* node, int level) {
    if (node == NULL) {
        return;
    }

    // Print indentation based on the level
    for (int i = 0; i < level; i++) {
        printf("  ");
    }

    // Print the node's keys
    printf("[");
    for (int i = 0; i < node->n; i++) {
        printf("%d ", node->keys[i]);
    }
    printf("]\n");

    // Recursively visualize children
    if (!node->leaf) {
        for (int i = 0; i <= node->n; i++) {
            printTree(node->children[i], level + 1);
        }
    }
}

int main()
{
    BTree* btree = createBTree(MIN_DEGREE);

    int option = 0;
    int key = 0;
    bool found = false;

    while (option != 5)
    {
        printf("\n\nMenu\n");
        printf("1 - Insert\n");
        printf("2 - Search\n");
        printf("3 - Remove\n");
        printf("4 - Display\n");
        printf("5 - Exit\n");

        printf("Enter the desired option:");
        scanf("%d", &option);

        switch (option)
        {
            case 1:
                printf("Enter the key to be inserted:");
                scanf("%d", &key);
                insert(btree, key);
                break;

            case 2:
                printf("Enter the key to be searched:");
                scanf("%d", &key);
                found = search(btree->root, key);

                if (found) {
                    printf("Key %d found in the B+ tree.\n", key);
                }
                else {
                    printf("Key %d not found in the B+ tree.\n", key);
                }
                break;

            case 3:
                printf("Enter the key to be removed:");
                scanf("%d", &key);
                deleteKey(btree, key);
                break;

            case 4:
                printTree(btree->root, 0);
                break;

            case 5:
                break;

            default:
                printf("Invalid option!\n");
                break;
        }
    }

    return 0;
}
