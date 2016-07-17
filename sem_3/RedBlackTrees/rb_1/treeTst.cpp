// =====================================
// Red-Black Trees task number 1
// (C) rdmr, 2k6
// Unautorized Duplication Appreciated!
// =====================================


//
// Test of class RBTree representing Red-Black Tree
// We work with the set of integer values
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "RBTree.h"

static bool writeIntegerTree(const RBTreeNode* root, FILE* f, int level = 0);
static bool readIntegerTree(RBTree& tree, FILE* f);
static void printHelp();

class Integer: public RBTreeNodeValue {
public:
    int number; // The value of a node is an integer number

    Integer(): 
        RBTreeNodeValue(), 
        number(0) 
    {}

    Integer(int n):
        RBTreeNodeValue(), 
        number(n)
    {}

    Integer(const Integer& n):
        RBTreeNodeValue(), 
        number(n.number)
    {}

    virtual int compareTo(const RBTreeNodeValue& v) const {
        return (number - ((const Integer&) v).number);
    }

    virtual ~Integer() {}
};

///== /////////////////////////////////////////////
int GetDepth(RBTreeNode *node)
{
  
  RBTreeNode *l = node->left;
  RBTreeNode *r = node->right;
  int ld = 0;
  int rd = 0;
  if (l != 0) ld = 1 + GetDepth(l); 
  if (r != 0) rd = 1 + GetDepth(r); 
  if (ld >= rd) return ld; else return rd;
  return 0;
}

int GetBlackDepth(RBTreeNode *node)
{
  
  int bd = 0;
  while(node != 0)
  {
    if (!node->red) bd++; node = node->left;
  }

  return bd;
}

///== /////////////////////////////////////////////
int main() {
    printf("Test of Red-Black Tree class\n");
    printHelp();
    FILE* f = NULL;

    // Define a random tree with 20 nodes
    time_t t = time(0); // Current time in seconds since 1 Jan 1970
    srand(t);

    RBTree tree;
    char line[256];
    while (true)
    {
        printf("Command>");
        if (fgets(line, 254, stdin) == NULL) break;

        // Parse a command
        line[254] = 0;
        int len = strlen(line);
        // Remove "\r\n" at the end of line
        if (len > 0 && line[len-1] == '\n') { line[len-1] = 0; --len; }
        if (len > 0 && line[len-1] == '\r') { line[len-1] = 0; --len; }

        int i = 0;
        // Skip a space in beginning of line
        while (i < len && isspace(line[i])) ++i;
        int commandBeg = i;
        while (i < len && isalpha(line[i])) ++i;
        int commandEnd = i;
        int commandLen = commandEnd - commandBeg;

        if (strncmp("gentree", line+commandBeg, commandLen) == 0 || strncmp("gt", line+commandBeg, commandLen) == 0)
        {
            while (i < len && isspace(line[i])) ++i; // Skip a space
            if (i >= len || !isdigit(line[i])) { printf("Incorrect command.\n"); printHelp(); continue; }
            int n = atoi(line + i);

            // Generate a random tree
            tree.clear();
            int j = 0; if (n > 100)  n = 100;        // Maximum 100 nodes
            while (j < n)
            {
                Integer num(rand() % 100 + 1); RBTreeNode* node;
                if (!tree.find(&num, tree.root(), &node)) { Integer* v = new Integer(num); tree.insert(node, v); ++j; }
            }

            // Print a tree to stdout
            writeIntegerTree(tree.root(), stdout);

        }
        else if (strncmp("readtree", line+commandBeg, commandLen) == 0)
        {

            while (i < len && isspace(line[i])) ++i; // Skip a space
            if ((f = fopen(line+i, "r")) == NULL) { perror("Could not open a file for reading"); continue; }
            // Read a tree from a file
            if (readIntegerTree(tree, f)) writeIntegerTree(tree.root(), stdout); else  printf("Incorrect format.\n"); 
            fclose(f); f = NULL;

        }
        else if (strncmp("writetree", line+commandBeg, commandLen) == 0)
        {
            while (i < len && isspace(line[i])) ++i; // Skip a space
            if (i >= len) { printf("Incorrect command.\n"); printHelp(); continue; }
            if ((f = fopen(line+i, "w")) == NULL) { perror("Could not open a file for writing"); continue; }
            writeIntegerTree(tree.root(), f);
            fclose(f); f = NULL;

        }
        ///////////////////// ================================================
        else if (strncmp("md", line+commandBeg, commandLen) == 0)
        {
            RBTreeNode*node = tree.root();
            printf("Max depth = %d.\n", GetDepth(node));
            printf("Black depth = %d.\n", GetBlackDepth(node));
        }
        ///////////////////// ================================================
        else if (strncmp("quit", line+commandBeg, commandLen) == 0) break; // end if
    } // end while

    return 0;
}

// Recursive function
static bool writeIntegerTree(const RBTreeNode* root, FILE* f, int level /* = 0 */)
{
    int i;
    for (i = 0; i < level; ++i) {
        fprintf(f, "    ");
    }
    if (root == 0) {
        fprintf(f, "null\n");
    } else {
        Integer* v = (Integer*) root->value;
        fprintf(f, "%d ", v->number);
        if (root->red)
            fprintf(f, " red\n");
        else
            fprintf(f, " black\n");
        writeIntegerTree(root->left, f, level+1);
        writeIntegerTree(root->right, f, level+1);
    }

    return true;
}

// Read a tree from a stream
// Example:                               //
//                 10 black               //
//                /        \              //
//            5 red         15 black      //
//           /   \          /    \        //
//        null  7 black   null    null    //
//               /  \                     //
//            null  null                  //
// is represented as                      //
// 10 black
//     5 red
//         null
//         7 black
//             null
//             null
//     15 black
//         null
//         null
// This is a recursive function
//
static bool readIntegerTree(RBTree& tree, FILE* f) {
    tree.clear();

    char line[256];
    if (fgets(line, 254, f) == NULL)
        return false;
    line[254] = 0;
    int len = strlen(line);
    // Remove "\r\n" at the end of line
    if (len > 0 && line[len-1] == '\n') --len;
    if (len > 0 && line[len-1] == '\r') --len;
    int i = 0;
    while (i < len && isspace(line[i])) ++i;
    if (i < len && (isdigit(line[i]) || line[i] == '-'))
    {
        int n = atoi(line + i);
        bool red = false;
        while (i < len && (isdigit(line[i]) || line[i] == '-')) ++i;
        if (i >= len || !isspace(line[i])) return false;
        while (i < len && isspace(line[i])) ++i;
        if (i < len && line[i] == 'r') red = true;
        RBTree leftSubtree;
        if (!readIntegerTree(leftSubtree, f)) return false;
        RBTree rightSubtree;
        if (!readIntegerTree(rightSubtree, f)) return false;
        RBTreeNode* rootNode = new RBTreeNode();
        rootNode->red = red;
        rootNode->value = new Integer(n);
        tree.header.left = rootNode;
        rootNode->parent = &(tree.header);
        tree.numNodes = 1;

        if (leftSubtree.size() > 0)
        {
            rootNode->left = leftSubtree.root();
            leftSubtree.root()->parent = rootNode;
            tree.numNodes += leftSubtree.size();

            leftSubtree.header.left = 0;
            leftSubtree.numNodes = 0;
        }

        if (rightSubtree.size() > 0)
        {
            rootNode->right = rightSubtree.root();
            rightSubtree.root()->parent = rootNode;
            tree.numNodes += rightSubtree.size();

            rightSubtree.header.left = 0;
            rightSubtree.numNodes = 0;
        }
    }

    return true;
}

static void printHelp() {
    printf(
        "Commands:\n"
        "  gentree n [gt n]\t"
        "\tgenerate a random tree with n nodes\n"
        "  md\t"
        "get max depth & black depth"
        "  readtree fileName\t"
        "read a tree from the file \"fileName\"\n"
        "  writetree fileName\t"
        "write a tree into the file \"fileName\"\n"
        "  quit\t\t\tquit\n"
    );
}
