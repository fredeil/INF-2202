/*
 *  bpt:  B+ Tree Implementation
 *  Copyright (C) 2010  Amittai Aviram  http://www.amittai.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *  Author:  Amittai Aviram
 *    http://www.amittai.com
 *    amittai.aviram@gmail.edu or afa13@columbia.edu
 *    Senior Software Engineer
 *    MathWorks, Inc.
 *    3 Apple Hill Drive
 *    Natick, MA 01760
 *  Original Date:  26 June 2010
 *  Last modified: 15 April 2014
 *
 *  This implementation demonstrates the B+ tree data structure
 *  for educational purposes, includin insertion, deletion, search, and display
 *  of the search path, the leaves, or the whole tree.
 *
 */

// Uncomment the line below if you are compiling on Windows.
// #define WINDOWS
#define _GNU_SOURCE
#include <sched.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#ifdef WINDOWS
#define bool char
#define false 0
#define true 1
#endif

#define DEFAULT_ORDER 336

#define MIN_ORDER 3
#define MAX_ORDER 400

// TYPES.
typedef struct record
{
  int value;
} record;

typedef struct node
{
  void **pointers;
  int *keys;
  struct node *parent;
  bool is_leaf;
  int num_keys;
  struct node *next; // Used for queue.
} node;

// GLOBALS.
int order = DEFAULT_ORDER;
node *queue = NULL;
bool verbose_output = true;
pthread_rwlock_t rwlock;

// Output and utility.
void usage(void);
void enqueue(node *new_node);
node *dequeue(void);
int height(node *root);
int path_to_root(node *root, node *child);
void print_leaves(node *root);
void print_tree(node *root);
void find_and_print(node *root, int key, bool verbose);
void find_and_print_range(node *root, int range1, int range2, bool verbose);
int find_range(node *root, int key_start, int key_end, bool verbose, int returned_keys[], void *returned_pointers[]);
node *find_leaf(node *root, int key, bool verbose);
record *find(node *root, int key, bool verbose);
int cut(int length);

// Insertion.
record *make_record(int value);
node *make_node(void);
node *make_leaf(void);
int get_left_index(node *parent, node *left);
node *insert_into_leaf(node *leaf, int key, record *pointer);
node *insert_into_leaf_after_splitting(node *root, node *leaf, int key, record *pointer);
node *insert_into_node(node *root, node *parent, int left_index, int key, node *right);
node *insert_into_node_after_splitting(node *root, node *parent, int left_index, int key, node *right);
node *insert_into_parent(node *root, node *left, int key, node *right);
node *insert_into_new_root(node *left, int key, node *right);
node *start_new_tree(int key, record *pointer);
node *insert(node **root, int key, int value);

// Deletion.
int get_neighbor_index(node *n);
node *adjust_root(node *root);
node *coalesce_nodes(node *root, node *n, node *neighbor, int neighbor_index, int k_prime);
node *redistribute_nodes(node *root, node *n, node *neighbor, int neighbor_index, int k_prime_index, int k_prime);
node *delete_entry(node *root, node *n, int key, void *pointer);
node *delete (node *root, int key);

// OUTPUT AND UTILITIES
void usage()
{
  fprintf(stderr, "Accepted parameters\n");
  fprintf(stderr, "-r <NUM>    : Range size\n");
  fprintf(stderr, "-u <0..100> : Update ratio. 0 = Only search; 100 = Only updates\n");
  fprintf(stderr, "-i <NUM>    : Initial tree size (inital pre-filled element count)\n");
  fprintf(stderr, "-t <0 / 1>  : Test mode (for correctness). 0: NO / 1: YES.\n");
  fprintf(stderr, "-n <NUM>    : Number of threads\n");
  fprintf(stderr, "-s <NUM>    : Random seed. 0 = using time as seed\n");
  fprintf(stderr, "-h          : This help\n\n");
  fprintf(stderr, "Benchmark output format: \n\"0: range, insert ratio, delete ratio, #threads, attempted insert, attempted delete, attempted search, effective insert, effective delete, effective search, time (in msec)\"\n\n");
  exit(EXIT_SUCCESS);
}

/* Helper function for printing the
 * tree out.  See print_tree.
 */
void enqueue(node *new_node)
{
  node *c;
  if (queue == NULL)
  {
    queue = new_node;
    queue->next = NULL;
  }
  else
  {
    c = queue;
    while (c->next != NULL)
    {
      c = c->next;
    }
    c->next = new_node;
    new_node->next = NULL;
  }
}

/* Helper function for printing the
 * tree out.  See print_tree.
 */
node *dequeue(void)
{
  node *n = queue;
  queue = queue->next;
  n->next = NULL;
  return n;
}

/* Prints the bottom row of keys
 * of the tree (with their respective
 * pointers, if the verbose_output flag is set.
 */
void print_leaves(node *root)
{
  int i;
  node *c = root;
  if (root == NULL)
  {
    printf("Empty tree.\n");
    return;
  }

  while (!c->is_leaf)
    c = c->pointers[0];

  while (true)
  {
    for (i = 0; i < c->num_keys; i++)
    {
      if (verbose_output)
        printf("%lx ", (unsigned long)c->pointers[i]);
      printf("%d ", c->keys[i]);
    }

    if (c->pointers[order - 1] == NULL)
      break;

    if (verbose_output)
      printf("%lx ", (unsigned long)c->pointers[order - 1]);

    printf(" | ");
    c = c->pointers[order - 1];
  }

  printf("\n");
}

/* Utility function to give the height
 * of the tree, which length in number of edges
 * of the path from the root to any leaf.
 */
int height(node *root)
{
  int h = 0;
  node *c = root;
  while (!c->is_leaf)
  {
    c = c->pointers[0];
    h++;
  }
  return h;
}

/* Utility function to give the length in edges
 * of the path from any node to the root.
 */
int path_to_root(node *root, node *child)
{
  int length = 0;
  node *c = child;
  while (c != root)
  {
    c = c->parent;
    length++;
  }
  return length;
}

/* Prints the B+ tree in the command
 * line in level (rank) order, with the
 * keys in each node and the '|' symbol
 * to separate nodes.
 * With the verbose_output flag set.
 * the values of the pointers corresponding
 * to the keys also appear next to their respective
 * keys, in hexadecimal notation.
 */
void print_tree(node *root)
{
  if (root == NULL)
  {
    printf("Empty tree.\n");
    return;
  }

  node *n = NULL;
  int i = 0;
  int rank = 0;
  int new_rank = 0;

  queue = NULL;
  enqueue(root);
  while (queue != NULL)
  {
    n = dequeue();
    if (n->parent != NULL && n == n->parent->pointers[0])
    {
      new_rank = path_to_root(root, n);
      if (new_rank != rank)
      {
        rank = new_rank;
        printf("\n");
      }
    }

    if (verbose_output)
      printf("(%lx)", (unsigned long)n);

    for (i = 0; i < n->num_keys; i++)
    {
      if (verbose_output)
        printf("%lx ", (unsigned long)n->pointers[i]);
      printf("%d ", n->keys[i]);
    }

    if (!n->is_leaf)
    {
      for (i = 0; i <= n->num_keys; i++)
        enqueue(n->pointers[i]);
    }

    if (verbose_output)
    {
      if (n->is_leaf)
        printf("%lx ", (unsigned long)n->pointers[order - 1]);
      else
        printf("%lx ", (unsigned long)n->pointers[n->num_keys]);
    }
    printf("| ");
  }
  printf("\n");
}

/* Finds the record under a given key and prints an
 * appropriate message to stdout.
 */
void find_and_print(node *root, int key, bool verbose)
{
  record *r = find(root, key, verbose);
  if (r == NULL)
    printf("Record not found under key %d.\n", key);
  else
    printf("Record at %lx -- key %d, value %d.\n",
           (unsigned long)r, key, r->value);
}

/* Finds and prints the keys, pointers, and values within a range
 * of keys between key_start and key_end, including both bounds.
 */
void find_and_print_range(node *root, int key_start, int key_end, bool verbose)
{
  int i;
  int array_size = key_end - key_start + 1;
  int returned_keys[array_size];
  void *returned_pointers[array_size];
  int num_found = find_range(root, key_start, key_end, verbose,
                             returned_keys, returned_pointers);
  if (num_found)
  {
    for (i = 0; i < num_found; i++)
      printf("Key: %d   Location: %lx  Value: %d\n",
             returned_keys[i],
             (unsigned long)returned_pointers[i],
             ((record *)
                  returned_pointers[i])
                 ->value);
  }
  else
    printf("None found.\n");
}

/* Finds keys and their pointers, if present, in the range specified
 * by key_start and key_end, inclusive.  Places these in the arrays
 * returned_keys and returned_pointers, and returns the number of
 * entries found.
 */
int find_range(node *root, int key_start, int key_end, bool verbose, int returned_keys[], void *returned_pointers[])
{
  int i, num_found;
  num_found = 0;
  node *n = find_leaf(root, key_start, verbose);
  if (n == NULL)
    return 0;

  // for-loop intentionally left empty
  for (i = 0; i < n->num_keys && n->keys[i] < key_start; i++)
    ;
  if (i == n->num_keys)
    return 0;

  while (n != NULL)
  {
    while (i < n->num_keys && n->keys[i] <= key_end)
    {
      returned_keys[num_found] = n->keys[i];
      returned_pointers[num_found] = n->pointers[i];
      num_found++;
      i++;
    }

    n = n->pointers[order - 1];
    i = 0;
  }

  return num_found;
}

/* Traces the path from the root to a leaf, searching
 * by key.  Displays information about the path
 * if the verbose flag is set.
 * Returns the leaf containing the given key.
 */
node *find_leaf(node *root, int key, bool verbose)
{
  int i = 0;
  node *c = root;
  if (c == NULL)
  {
    if (verbose)
      printf("Empty tree.\n");

    return c;
  }

  while (!c->is_leaf)
  {
    if (verbose)
    {
      printf("[");
      for (i = 0; i < c->num_keys - 1; i++)
        printf("%d ", c->keys[i]);
      printf("%d] ", c->keys[i]);
    }

    i = 0;
    while (i < c->num_keys)
    {
      if (key >= c->keys[i])
        i++;
      else
        break;
    }
    if (verbose)
      printf("%d ->\n", i);
    c = (node *)c->pointers[i];
  }

  if (verbose)
  {
    printf("Leaf [");
    for (i = 0; i < c->num_keys - 1; i++)
      printf("%d ", c->keys[i]);
    printf("%d] ->\n", c->keys[i]);
  }

  return c;
}

/* Finds and returns the record to which
 * a key refers.
 */
record *find(node *root, int key, bool verbose)
{
  int i = 0;
  node *c = find_leaf(root, key, verbose);
  if (c == NULL)
    return NULL;

  for (i = 0; i < c->num_keys; i++)
  {
    if (c->keys[i] == key)
    {
      break;
    }
  }

  if (i == c->num_keys)
    return NULL;
  else
    return (record *)c->pointers[i];
}

/* Finds the appropriate place to
 * split a node that is too big into two.
 */
int cut(int length)
{
  if (length % 2 == 0)
    return length / 2;
  else
    return length / 2 + 1;
}

// INSERTION
/* Creates a new record to hold the value
 * to which a key refers.
 */
record *make_record(int value)
{
  record *new_record = malloc(sizeof(record));

  if (new_record == NULL)
  {
    perror("Record creation.");
    exit(EXIT_FAILURE);
  }

  new_record->value = value;
  return new_record;
}

/* Creates a new general node, which can be adapted
 * to serve as either a leaf or an internal node.
 */
node *make_node(void)
{
  node *new_node = malloc(sizeof(node));
  if (new_node == NULL)
  {
    perror("Node creation.");
    exit(EXIT_FAILURE);
  }

  new_node->keys = malloc((order - 1) * sizeof(int));
  if (new_node->keys == NULL)
  {
    perror("New node keys array.");
    exit(EXIT_FAILURE);
  }

  new_node->pointers = malloc(order * sizeof(void *));
  if (new_node->pointers == NULL)
  {
    perror("New node pointers array.");
    exit(EXIT_FAILURE);
  }

  new_node->is_leaf = false;
  new_node->num_keys = 0;
  new_node->parent = NULL;
  new_node->next = NULL;
  return new_node;
}

/* Creates a new leaf by creating a node
 * and then adapting it appropriately.
 */
node *make_leaf(void)
{
  node *leaf = make_node();
  leaf->is_leaf = true;
  return leaf;
}

/* Helper function used in insert_into_parent
 * to find the index of the parent's pointer to
 * the node to the left of the key to be inserted.
 */
int get_left_index(node *parent, node *left)
{
  int left_index = 0;

  while (left_index <= parent->num_keys && parent->pointers[left_index] != left)
    left_index++;

  return left_index;
}

/* Inserts a new pointer to a record and its corresponding
 * key into a leaf.
 * Returns the altered leaf.
 */
node *insert_into_leaf(node *leaf, int key, record *pointer)
{
  int insertion_point = 0;
  while (insertion_point < leaf->num_keys && leaf->keys[insertion_point] < key)
    insertion_point++;

  int i;
  for (i = leaf->num_keys; i > insertion_point; i--)
  {
    leaf->keys[i] = leaf->keys[i - 1];
    leaf->pointers[i] = leaf->pointers[i - 1];
  }

  leaf->keys[insertion_point] = key;
  leaf->pointers[insertion_point] = pointer;
  leaf->num_keys++;

  return leaf;
}

/* Inserts a new key and pointer
 * to a new record into a leaf so as to exceed
 * the tree's order, causing the leaf to be split
 * in half.
 */
node *insert_into_leaf_after_splitting(node *root, node *leaf, int key, record *pointer)
{
  node *new_leaf = make_leaf();

  int *temp_keys = malloc(order * sizeof(int));
  if (temp_keys == NULL)
  {
    perror("Temporary keys array.");
    exit(EXIT_FAILURE);
  }

  void **temp_pointers = malloc(order * sizeof(void *));
  if (temp_pointers == NULL)
  {
    perror("Temporary pointers array.");
    exit(EXIT_FAILURE);
  }

  int insertion_index, split, new_key, i, j;

  insertion_index = 0;
  while (insertion_index < order - 1 && leaf->keys[insertion_index] < key)
    insertion_index++;

  for (i = 0, j = 0; i < leaf->num_keys; i++, j++)
  {
    if (j == insertion_index)
      j++;
    temp_keys[j] = leaf->keys[i];
    temp_pointers[j] = leaf->pointers[i];
  }

  temp_keys[insertion_index] = key;
  temp_pointers[insertion_index] = pointer;

  leaf->num_keys = 0;

  split = cut(order - 1);

  for (i = 0; i < split; i++)
  {
    leaf->pointers[i] = temp_pointers[i];
    leaf->keys[i] = temp_keys[i];
    leaf->num_keys++;
  }

  for (i = split, j = 0; i < order; i++, j++)
  {
    new_leaf->pointers[j] = temp_pointers[i];
    new_leaf->keys[j] = temp_keys[i];
    new_leaf->num_keys++;
  }

  free(temp_pointers);
  free(temp_keys);

  new_leaf->pointers[order - 1] = leaf->pointers[order - 1];
  leaf->pointers[order - 1] = new_leaf;

  for (i = leaf->num_keys; i < order - 1; i++)
    leaf->pointers[i] = NULL;

  for (i = new_leaf->num_keys; i < order - 1; i++)
    new_leaf->pointers[i] = NULL;

  new_leaf->parent = leaf->parent;
  new_key = new_leaf->keys[0];

  return insert_into_parent(root, leaf, new_key, new_leaf);
}

/* Inserts a new key and pointer to a node
 * into a node into which these can fit
 * without violating the B+ tree properties.
 */
node *insert_into_node(node *root, node *n, int left_index, int key, node *right)
{
  int i;

  for (i = n->num_keys; i > left_index; i--)
  {
    n->pointers[i + 1] = n->pointers[i];
    n->keys[i] = n->keys[i - 1];
  }

  n->pointers[left_index + 1] = right;
  n->keys[left_index] = key;
  n->num_keys++;
  return root;
}

/* Inserts a new key and pointer to a node
 * into a node, causing the node's size to exceed
 * the order, and causing the node to split into two.
 */
node *insert_into_node_after_splitting(node *root, node *old_node, int left_index, int key, node *right)
{
  /* First create a temporary set of keys and pointers
  * to hold everything in order, including
  * the new key and pointer, inserted in their
  * correct places.
  * Then create a new node and copy half of the
  * keys and pointers to the old node and
  * the other half to the new.
  */

  node **temp_pointers = malloc((order + 1) * sizeof(node *));
  if (temp_pointers == NULL)
  {
    perror("Temporary pointers array for splitting nodes.");
    exit(EXIT_FAILURE);
  }

  int *temp_keys = malloc(order * sizeof(int));
  if (temp_keys == NULL)
  {
    perror("Temporary keys array for splitting nodes.");
    exit(EXIT_FAILURE);
  }

  int i, j, split, k_prime;
  for (i = 0, j = 0; i < old_node->num_keys + 1; i++, j++)
  {
    if (j == left_index + 1)
      j++;
    temp_pointers[j] = old_node->pointers[i];
  }

  for (i = 0, j = 0; i < old_node->num_keys; i++, j++)
  {
    if (j == left_index)
      j++;
    temp_keys[j] = old_node->keys[i];
  }

  temp_pointers[left_index + 1] = right;
  temp_keys[left_index] = key;

  /* Create the new node and copy
  * half the keys and pointers to the
  * old and half to the new.
  */
  split = cut(order);

  node *new_node = make_node();
  old_node->num_keys = 0;

  for (i = 0; i < split - 1; i++)
  {
    old_node->pointers[i] = temp_pointers[i];
    old_node->keys[i] = temp_keys[i];
    old_node->num_keys++;
  }

  old_node->pointers[i] = temp_pointers[i];
  k_prime = temp_keys[split - 1];
  for (++i, j = 0; i < order; i++, j++)
  {
    new_node->pointers[j] = temp_pointers[i];
    new_node->keys[j] = temp_keys[i];
    new_node->num_keys++;
  }

  new_node->pointers[j] = temp_pointers[i];
  free(temp_pointers);
  free(temp_keys);

  node *child;
  new_node->parent = old_node->parent;
  for (i = 0; i <= new_node->num_keys; i++)
  {
    child = new_node->pointers[i];
    child->parent = new_node;
  }

  /* Insert a new key into the parent of the two
  * nodes resulting from the split, with
  * the old node to the left and the new to the right.
  */

  return insert_into_parent(root, old_node, k_prime, new_node);
}

/* Inserts a new node (leaf or internal node) into the B+ tree.
 * Returns the root of the tree after insertion.
 */
node *insert_into_parent(node *root, node *left, int key, node *right)
{
  node *parent = left->parent;

  // Case: new root
  if (parent == NULL)
    return insert_into_new_root(left, key, right);

  // Case: leaf or node

  // Find the parent's pointer to the left node.
  int left_index = get_left_index(parent, left);

  // Simple case: the new key fits into the node.
  if (parent->num_keys < order - 1)
    return insert_into_node(root, parent, left_index, key, right);

  // Harder case:  split a node in order to preserve the B+ tree properties.
  return insert_into_node_after_splitting(root, parent, left_index, key, right);
}

/* Creates a new root for two subtrees
 * and inserts the appropriate key into
 * the new root.
 */
node *insert_into_new_root(node *left, int key, node *right)
{
  node *root = make_node();
  root->keys[0] = key;
  root->pointers[0] = left;
  root->pointers[1] = right;
  root->num_keys++;
  root->parent = NULL;
  left->parent = root;
  right->parent = root;
  return root;
}

/* First insertion:
 * start a new tree.
 */
node *start_new_tree(int key, record *pointer)
{
  node *root = make_leaf();
  root->keys[0] = key;
  root->pointers[0] = pointer;
  root->pointers[order - 1] = NULL;
  root->parent = NULL;
  root->num_keys++;
  return root;
}

/* Master insertion function.
 * Inserts a key and an associated value into
 * the B+ tree, causing the tree to be adjusted
 * however necessary to maintain the B+ tree
 * properties.
 */
node *insert(node **root, int key, int value)
{
  pthread_rwlock_wrlock(&rwlock);

  // The current implementation ignores duplicates.
  if (find(*root, key, false) != NULL)
  {
    pthread_rwlock_unlock(&rwlock);
    return *root;
  }

  // Create a new record for the value.
  record *pointer = make_record(value);

  // Case: the tree does not exist yet.
  if (*root == NULL)
  {
    node *mordi = start_new_tree(key, pointer);
    pthread_rwlock_unlock(&rwlock);
    return mordi;
  }

  node *leaf = find_leaf(*root, key, false);

  // Case: leaf has room for key and pointer.
  if (leaf->num_keys < order - 1)
  {
    leaf = insert_into_leaf(leaf, key, pointer);
    pthread_rwlock_unlock(&rwlock);
    return *root;
  }

  // Case: leaf must be split.
  node *nd = insert_into_leaf_after_splitting(*root, leaf, key, pointer);

  pthread_rwlock_unlock(&rwlock);

  return nd;
}

// DELETION.

/* Utility function for deletion.  Retrieves
 * the index of a node's nearest neighbor (sibling)
 * to the left if one exists.  If not (the node
 * is the leftmost child), returns -1 to signify
 * this special case.
 */
int get_neighbor_index(node *n)
{
  /* Return the index of the key to the left
  * of the pointer in the parent pointing
  * to n.
  * If n is the leftmost child, this means
  * return -1.
  */
  int i;
  for (i = 0; i <= n->parent->num_keys; i++)
    if (n->parent->pointers[i] == n)
      return i - 1;

  // Error state.
  printf("Search for nonexistent pointer to node in parent.\n");
  printf("Node:  %#lx\n", (unsigned long)n);
  exit(EXIT_FAILURE);
}

node *remove_entry_from_node(node *n, int key, node *pointer)
{
  int i = 0;

  // Remove the key and shift other keys accordingly.
  while (n->keys[i] != key)
    i++;
  for (++i; i < n->num_keys; i++)
    n->keys[i - 1] = n->keys[i];

  // Remove the pointer and shift other pointers accordingly.
  // First determine number of pointers.
  int num_pointers = n->is_leaf ? n->num_keys : n->num_keys + 1;
  i = 0;
  while (n->pointers[i] != pointer)
    i++;
  for (++i; i < num_pointers; i++)
    n->pointers[i - 1] = n->pointers[i];

  // One key fewer.
  n->num_keys--;

  // Set the other pointers to NULL for tidiness.
  // A leaf uses the last pointer to point to the next leaf.
  if (n->is_leaf)
    for (i = n->num_keys; i < order - 1; i++)
      n->pointers[i] = NULL;
  else
    for (i = n->num_keys + 1; i < order; i++)
      n->pointers[i] = NULL;

  return n;
}

node *adjust_root(node *root)
{
  node *new_root;

  // Case: nonempty root.
  // Key and pointer have already been deleted, nothing to be done.
  if (root->num_keys > 0)
    return root;

  // If it has a child, promote the first (only) child as the new root.
  if (!root->is_leaf)
  {
    new_root = root->pointers[0];
    new_root->parent = NULL;
  }
  // If it is a leaf (has no children) then the whole tree is empty.
  else
    new_root = NULL;

  free(root->keys);
  free(root->pointers);
  free(root);

  return new_root;
}

/* Coalesces a node that has become
 * too small after deletion
 * with a neighboring node that
 * can accept the additional entries
 * without exceeding the maximum.
 */
node *coalesce_nodes(node *root, node *n, node *neighbor, int neighbor_index, int k_prime)
{
  int i, j, neighbor_insertion_index, n_end;
  node *tmp;

  /* Swap neighbor with node if node is on the
  * extreme left and neighbor is to its right.
  */
  if (neighbor_index == -1)
  {
    tmp = n;
    n = neighbor;
    neighbor = tmp;
  }

  /* Starting point in the neighbor for copying
  * keys and pointers from n.
  * Recall that n and neighbor have swapped places
  * in the special case of n being a leftmost child.
  */
  neighbor_insertion_index = neighbor->num_keys;

  /* Case:  nonleaf node.
  * Append k_prime and the following pointer.
  * Append all pointers and keys from the neighbor.
  */
  if (!n->is_leaf)
  {
    // Append k_prime.
    neighbor->keys[neighbor_insertion_index] = k_prime;
    neighbor->num_keys++;

    n_end = n->num_keys;

    for (i = neighbor_insertion_index + 1, j = 0; j < n_end; i++, j++)
    {
      neighbor->keys[i] = n->keys[j];
      neighbor->pointers[i] = n->pointers[j];
      neighbor->num_keys++;
      n->num_keys--;
    }

    /* The number of pointers is always
    * one more than the number of keys.
    */
    neighbor->pointers[i] = n->pointers[j];

    // All children must now point up to the same parent.
    for (i = 0; i < neighbor->num_keys + 1; i++)
    {
      tmp = (node *)neighbor->pointers[i];
      tmp->parent = neighbor;
    }
  }
  /* In a leaf, append the keys and pointers of
  * n to the neighbor.
  * Set the neighbor's last pointer to point to
  * what had been n's right neighbor.
  */
  else
  {
    for (i = neighbor_insertion_index, j = 0; j < n->num_keys; i++, j++)
    {
      neighbor->keys[i] = n->keys[j];
      neighbor->pointers[i] = n->pointers[j];
      neighbor->num_keys++;
    }

    neighbor->pointers[order - 1] = n->pointers[order - 1];
  }

  root = delete_entry(root, n->parent, k_prime, n);
  free(n->keys);
  free(n->pointers);
  free(n);
  return root;
}

/* Redistributes entries between two nodes when
 * one has become too small after deletion
 * but its neighbor is too big to append the
 * small node's entries without exceeding the
 * maximum
 */
node *redistribute_nodes(node *root, node *n, node *neighbor, int neighbor_index, int k_prime_index, int k_prime)
{
  /* Case: n has a neighbor to the left.
    * Pull the neighbor's last key-pointer pair over
    * from the neighbor's right end to n's left end.
    */
  if (neighbor_index != -1)
  {
    if (!n->is_leaf)
      n->pointers[n->num_keys + 1] = n->pointers[n->num_keys];

    int i;
    for (i = n->num_keys; i > 0; i--)
    {
      n->keys[i] = n->keys[i - 1];
      n->pointers[i] = n->pointers[i - 1];
    }

    if (!n->is_leaf)
    {
      n->pointers[0] = neighbor->pointers[neighbor->num_keys];
      node *tmp = (node *)n->pointers[0];
      tmp->parent = n;
      neighbor->pointers[neighbor->num_keys] = NULL;
      n->keys[0] = k_prime;
      n->parent->keys[k_prime_index] = neighbor->keys[neighbor->num_keys - 1];
    }
    else
    {
      n->pointers[0] = neighbor->pointers[neighbor->num_keys - 1];
      neighbor->pointers[neighbor->num_keys - 1] = NULL;
      n->keys[0] = neighbor->keys[neighbor->num_keys - 1];
      n->parent->keys[k_prime_index] = n->keys[0];
    }
  }
  /* Case: n is the leftmost child.
    * Take a key-pointer pair from the neighbor to the right.
    * Move the neighbor's leftmost key-pointer pair
    * to n's rightmost position.
    */
  else
  {
    if (n->is_leaf)
    {
      n->keys[n->num_keys] = neighbor->keys[0];
      n->pointers[n->num_keys] = neighbor->pointers[0];
      n->parent->keys[k_prime_index] = neighbor->keys[1];
    }
    else
    {
      n->keys[n->num_keys] = k_prime;
      n->pointers[n->num_keys + 1] = neighbor->pointers[0];
      node *tmp = (node *)n->pointers[n->num_keys + 1];
      tmp->parent = n;
      n->parent->keys[k_prime_index] = neighbor->keys[0];
    }

    int i;
    for (i = 0; i < neighbor->num_keys - 1; i++)
    {
      neighbor->keys[i] = neighbor->keys[i + 1];
      neighbor->pointers[i] = neighbor->pointers[i + 1];
    }
    if (!n->is_leaf)
      neighbor->pointers[i] = neighbor->pointers[i + 1];
  }

  /* n now has one more key and one more pointer;
  * the neighbor has one fewer of each.
  */

  n->num_keys++;
  neighbor->num_keys--;

  return root;
}

/* Deletes an entry from the B+ tree.
 * Removes the record and its key and pointer
 * from the leaf, and then makes all appropriate
 * changes to preserve the B+ tree properties.
 */
node *delete_entry(node *root, node *n, int key, void *pointer)
{
  int min_keys;
  node *neighbor;
  int neighbor_index;
  int k_prime_index, k_prime;
  int capacity;

  // Remove key and pointer from node.
  n = remove_entry_from_node(n, key, pointer);

  // Case: deletion from the root.
  if (n == root)
    return adjust_root(root);

  /* Determine minimum allowable size of node,
  * to be preserved after deletion.
  */
  min_keys = n->is_leaf ? cut(order - 1) : cut(order) - 1;

  /* Case: node stays at or above minimum.
  * (The simple case.)
  */
  if (n->num_keys >= min_keys)
    return root;

  /* Case: node falls below minimum.
  * Either coalescence or redistribution is needed.
  */

  /* Find the appropriate neighbor node with which
  * to coalesce.
  * Also find the key (k_prime) in the parent
  * between the pointer to node n and the pointer
  * to the neighbor.
  */
  neighbor_index = get_neighbor_index(n);
  k_prime_index = neighbor_index == -1 ? 0 : neighbor_index;
  k_prime = n->parent->keys[k_prime_index];
  neighbor = neighbor_index == -1 ? n->parent->pointers[1] : n->parent->pointers[neighbor_index];

  capacity = n->is_leaf ? order : order - 1;

  if (neighbor->num_keys + n->num_keys < capacity)
    return coalesce_nodes(root, n, neighbor, neighbor_index, k_prime);

  return redistribute_nodes(root, n, neighbor, neighbor_index, k_prime_index, k_prime);
}

// Master deletion function.
node *delete (node *root, int key)
{
  pthread_rwlock_wrlock(&rwlock);

  record *key_record = find(root, key, false);
  node *key_leaf = find_leaf(root, key, false);

  if (key_record != NULL && key_leaf != NULL)
  {
    root = delete_entry(root, key_leaf, key, key_record);
    free(key_record);
  }

  pthread_rwlock_unlock(&rwlock);

  return root;
}

void destroy_tree_nodes(node *root)
{
  int i;
  if (root->is_leaf)
    for (i = 0; i < root->num_keys; i++)
      free(root->pointers[i]);
  else
    for (i = 0; i < root->num_keys + 1; i++)
      destroy_tree_nodes(root->pointers[i]);

  free(root->pointers);
  free(root->keys);
  free(root);
}

void destroy_tree(node *root)
{
  destroy_tree_nodes(root);
}

/*---------------START BENCHMARK------------------*/

//Emulated pthread spinlock and barrier for MAC OS X (SLOW!!!)
#if defined(__APPLE__) && defined(__MACH__)

#include <libkern/OSAtomic.h>

typedef volatile OSSpinLock pthread_spinlock_t;

#ifndef PTHREAD_PROCESS_SHARED
#define PTHREAD_PROCESS_SHARED 1
#endif
#ifndef PTHREAD_PROCESS_PRIVATE
#define PTHREAD_PROCESS_PRIVATE 2
#endif

static inline int pthread_spin_init(pthread_spinlock_t *lock, int pshared)
{
  *lock = OS_SPINLOCK_INIT;
  return 0;
}

static inline int pthread_spin_destroy(pthread_spinlock_t *lock)
{
  return 0;
}

static inline int pthread_spin_lock(pthread_spinlock_t *lock)
{
  OSSpinLockLock(lock);
  return 0;
}

static inline int pthread_spin_trylock(pthread_spinlock_t *lock)
{
  return !OSSpinLockTry(lock);
}

static inline int pthread_spin_unlock(pthread_spinlock_t *lock)
{
  OSSpinLockUnlock(lock);
  return 0;
}

#ifndef PTHREAD_BARRIER_H_
#define PTHREAD_BARRIER_H_

#include <pthread.h>
#include <errno.h>

typedef int pthread_barrierattr_t;
typedef struct
{
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  int count;
  int tripCount;
} pthread_barrier_t;

int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count)
{
  if (count == 0)
  {
    errno = EINVAL;
    return -1;
  }
  if (pthread_mutex_init(&barrier->mutex, 0) < 0)
  {
    return -1;
  }
  if (pthread_cond_init(&barrier->cond, 0) < 0)
  {
    pthread_mutex_destroy(&barrier->mutex);
    return -1;
  }
  barrier->tripCount = count;
  barrier->count = 0;

  return 0;
}

int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
  pthread_cond_destroy(&barrier->cond);
  pthread_mutex_destroy(&barrier->mutex);
  return 0;
}

int pthread_barrier_wait(pthread_barrier_t *barrier)
{
  pthread_mutex_lock(&barrier->mutex);
  ++(barrier->count);
  if (barrier->count >= barrier->tripCount)
  {
    barrier->count = 0;
    pthread_cond_broadcast(&barrier->cond);
    pthread_mutex_unlock(&barrier->mutex);
    return 1;
  }
  else
  {
    pthread_cond_wait(&barrier->cond, &(barrier->mutex));
    pthread_mutex_unlock(&barrier->mutex);
    return 0;
  }
}

#endif // PTHREAD_BARRIER_H_
#endif
// END: Helper pthread spinlock function for MAC OS X

// Better suited searching
int search(node *root, int val)
{
  pthread_rwlock_rdlock(&rwlock);
  record *ret = find(root, val, 0);
  pthread_rwlock_unlock(&rwlock);

  if (ret != NULL && ret->value == val)
    return 1;
  else
    return 0;
}

pthread_barrier_t bench_barrier;

#define __THREAD_PINNING 0

#define MAXITER 5000000

node *root;

/* RANDOM GENERATOR */
// RANGE: (1 - r)
int rand_range_re(unsigned int *seed, long r)
{
  return (rand_r(seed) % r) + 1;
}

/* simple function for generating random integer for probability, only works on value of integer 1-100% */
#define MAX_POOL 1000

int p_pool[MAX_POOL];

void prepare_randintp(float ins, float del)
{
  int i, j = 0;

  // Put insert
  for (i = 0; i < ins * MAX_POOL / 100; i++)
    p_pool[j++] = 1;

  // Put delete
  for (i = 0; i < del * MAX_POOL / 100; i++)
    p_pool[j++] = 2;

  // Put search
  for (i = j; i < MAX_POOL; i++)
    p_pool[j++] = 3;
}

/* Struct for data input/output per-thread */
struct arg_bench
{
  unsigned rank;
  int size;
  unsigned seed;
  unsigned seed2;
  unsigned update;
  unsigned *pool;
  unsigned long max_iter;
  long counter_ins;
  long counter_del;
  long counter_search;
  long counter_ins_s;
  long counter_del_s;
  long counter_search_s;
  long timer;
  long *inputs;
  int *ops;
};

void *do_bench(void *arguments)
{
  long counter[3] = {0},
       success[3] = {0};
  int val = 0;
  unsigned *pool;
  int ops, ret = 0;
  int b_size;
  long cont = 0;
  long max_iter = 0;

  struct timeval start, end;
  struct arg_bench *args = arguments;

  max_iter = args->max_iter;
  b_size = args->size;
  pool = args->pool;

  pthread_barrier_wait(&bench_barrier);

  gettimeofday(&start, NULL);

  /* Check the flag once in a while to see when to quit. */
  while (cont < max_iter)
  {
    //--For a completely random values (original)
    ops = pool[rand_range_re(&args->seed, MAX_POOL) - 1];
    val = rand_range_re(&args->seed2, b_size);

    //DEBUG_PRINT("ops:%d, val:%ld\n", ops, val);

    switch (ops)
    {
    case 1:
      root = insert(&root, val, val);
      break;
    case 2:
      root = delete (root, val);
      break;
    case 3:
      ret = search(root, val);
      break;
    default:
      exit(EXIT_SUCCESS);
      break;
    }
    cont++;
    counter[ops - 1]++;

    if (ret)
      success[ops - 1]++;
  }

  gettimeofday(&end, NULL);

  args->counter_ins = counter[0];
  args->counter_del = counter[1];
  args->counter_search = counter[2];

  args->counter_ins_s = success[0];
  args->counter_del_s = success[1];
  args->counter_search_s = success[2];

  args->timer = (end.tv_sec * 1000 + end.tv_usec / 1000) - (start.tv_sec * 1000 + start.tv_usec / 1000);

  //DEBUG_PRINT("\ncount:%ld, ins:%ld, del:%ld, search:%ld", cont, args->counter_ins, args->counter_del, args->counter_search);

  pthread_exit(arguments);
}

int benchmark(int threads, int size, float ins, float del)
{
  pthread_t *pid;
  long *inputs;
  int *ops;

  int i, k;

  struct arg_bench *args, *arg;

  args = calloc(threads, sizeof(struct arg_bench));

  inputs = calloc(size, sizeof(long));
  ops = calloc(size, sizeof(int));

  prepare_randintp(ins, del);

  long ncores = sysconf(_SC_NPROCESSORS_ONLN);
  int midcores = (int)ncores / 2;

  for (i = 0; i < threads; i++)
  {
    arg = &args[i];

    if (threads > midcores && threads < ncores && i >= (threads / 2))
      arg->rank = i - (threads / 2) + midcores;
    else
      arg->rank = i;

    arg->size = size;

    arg->update = ins + del;
    arg->seed = rand();
    arg->seed2 = rand();
    arg->pool = calloc(MAX_POOL, sizeof(unsigned));

    for (k = 0; k < MAX_POOL; k++)
      arg->pool[k] = p_pool[k];

    arg->counter_ins = 0;
    arg->counter_del = 0;
    arg->counter_search = 0;

    arg->counter_ins_s = 0;
    arg->counter_del_s = 0;
    arg->counter_search_s = 0;

    arg->timer = 0;

    arg->inputs = inputs;
    arg->ops = ops;

    arg->max_iter = ceil(MAXITER / threads);
  }

  pid = calloc(threads, sizeof(pthread_t));

  pthread_barrier_init(&bench_barrier, NULL, threads);

  fprintf(stderr, "\nStarting benchmark...");
  fprintf(stderr, "\n0: %d, %0.2f, %0.2f, %d, ", size, ins, del, threads);

  for (i = 0; i < threads; i++)
    pthread_create(&pid[i], NULL, &do_bench, &args[i]);

  for (i = 0; i < threads; i++)
    pthread_join(pid[i], NULL);

#ifdef __USEPCM
  pcm_bench_end();
  pcm_bench_print();
#endif

  struct arg_bench result = {
      .counter_del = 0,
      .counter_del_s = 0,
      .counter_ins = 0,
      .counter_ins_s = 0,
      .counter_search = 0,
      .counter_search_s = 0,
      .timer = 0};

  for (i = 0; i < threads; i++)
  {
    arg = &args[i];

    result.counter_del += arg->counter_del;
    result.counter_del_s += arg->counter_del_s;
    result.counter_ins += arg->counter_ins;
    result.counter_ins_s += arg->counter_ins_s;
    result.counter_search += arg->counter_search;
    result.counter_search_s += arg->counter_search_s;

    if (arg->timer > result.timer)
      result.timer = arg->timer;
  }

  fprintf(stderr, " %ld, %ld, %ld,", result.counter_ins, result.counter_del, result.counter_search);
  fprintf(stderr, " %ld, %ld, %ld, %ld\n", result.counter_ins_s, result.counter_del_s, result.counter_search_s, result.timer);

  free(pid);
  free(inputs);
  free(ops);
  free(args);

  return 0;
}

void initial_add(int num, int range)
{
  int i = 0, j = 0;

  while (i < num)
  {
    j = (rand() % range) + 1;
    root = insert(&root, j, j);
    i++;
  }
}

void start_benchmark(int key_size, int updaterate, int num_thread)
{
  float update = (float)updaterate / 2;
  benchmark(num_thread, key_size, update, update);
}

int *bulk;
int nr;

void *do_test(void *args)
{
  int myid = *((int *)args);
  int i;

  int start = (MAXITER / nr) * (myid);
  int range = (MAXITER / nr);
  int end;

  if (myid == nr - 1)
    end = MAXITER;
  else
    end = start + range;

  fprintf(stdout, "id:%d, s:%d, r:%d, e:%d\n", myid, start, range, end);

  pthread_barrier_wait(&bench_barrier);

  for (i = start; i < end; i++)
    root = insert(&root, bulk[i], bulk[i]);

  pthread_exit((void *)args);
}

void test(int initial, int updaterate, int num_thread, bool random)
{
  int i;
  pthread_t pid[num_thread];
  int arg[num_thread];

  struct timeval start, end;
  pthread_barrier_init(&bench_barrier, NULL, num_thread + 1);

  int allkey = MAXITER;

  nr = num_thread;
  bulk = calloc(MAXITER, sizeof(int));

  for (i = 0; i < allkey; i++)
  {
    if (random)
      bulk[i] = 1 + (rand() % MAXITER) + initial;
    else
      bulk[i] = 1 + i + initial;
  }

  for (i = 0; i < num_thread; i++)
  {
    arg[i] = i;
    pthread_create(&pid[i], NULL, &do_test, &arg[i]);
  }

  pthread_barrier_wait(&bench_barrier);

  gettimeofday(&start, NULL);

  for (i = 0; i < num_thread; i++)
    pthread_join(pid[i], NULL);

  gettimeofday(&end, NULL);

  printf("time : %lu usec\n", (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);

  for (i = 0; i < allkey; i++)
  {
    if (!search(root, bulk[i]))
    {
      fprintf(stderr, "Error found! Exiting.\n");
      exit(EXIT_FAILURE);
    }
  }

  fprintf(stderr, "PASSED!\n");
}

void testseq(bool random)
{
  int i, count = 0;
  struct timeval start, end;
  int *values;

  int seed_r = rand();
  srand(seed_r);

  values = calloc(MAXITER, sizeof(int));

  for (i = 0; i < MAXITER; i++)
  {
    if (random == true)
      values[i] = (rand() % MAXITER) + 1;
    else
      values[i] = i + 1;
  }

  printf("Inserting %d (%s) elements...\n", MAXITER, random ? "Random" : "Increasing");

  gettimeofday(&start, NULL);
  for (i = 0; i < MAXITER; i++)
  {
    root = insert(&root, values[i], values[i]);
  }
  gettimeofday(&end, NULL);
  printf("insert time : %lu usec\n", (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);

  srand(seed_r);

  gettimeofday(&start, NULL);
  for (i = 0; i < MAXITER; i++)
  {
    if (!search(root, values[i]))
    {
      count++;
    }
  }
  gettimeofday(&end, NULL);
  printf("search time : %lu usec\n", (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);

  free(values);

  if (count)
  {
    fprintf(stderr, "Error searching :%d!\n", count);
    exit(EXIT_FAILURE);
  }
  else
  {
    fprintf(stderr, "PASSED!\n");
  }
}
/*------------------- END BENCHMARK ---------------------*/

int main(int argc, char **argv)
{
  fprintf(stderr, "\nConcurrent BTree\n===============\n\n");
  if (argc < 2)
    fprintf(stderr, "NOTE: No parameters supplied, will continue with defaults\n");
  fprintf(stderr, "Use -h switch for help.\n\n");

  // Init lock
  if (pthread_rwlock_init(&rwlock, NULL) != 0)
  {
    // init failed
    return -1;
  }

  // Default values
  int initial_count = 1023;
  int range = 5000000;
  int update_rate = 10;
  int seed = 0;
  int num_threads = 1;
  int test_mode = false;

  int myopt = 0;
  while (EOF != myopt)
  {
    myopt = getopt(argc, argv, "r:t:n:i:u:s:hb:");
    switch (myopt)
    {
    case 'r':
      range = atoi(optarg);
      break;
    case 'n':
      num_threads = atoi(optarg);
      break;
    case 't':
      test_mode = atoi(optarg);
      break;
    case 'i':
      initial_count = atoi(optarg);
      break;
    case 'u':
      update_rate = atoi(optarg);
      break;
    case 's':
      seed = atoi(optarg);
      break;
    case 'h':
      usage();
    }
  }
  fprintf(stderr, "Parameters:\n");
  fprintf(stderr, "- Range size:\t\t %d\n", range);
  fprintf(stderr, "- Update rate:\t\t %d%% \n", update_rate);
  fprintf(stderr, "- Number of threads:\t %d\n", num_threads);
  fprintf(stderr, "- Initial tree size:\t %d\n", initial_count);
  fprintf(stderr, "- Random seed:\t\t %d\n", seed);
  fprintf(stderr, "- Test mode:\t\t %s\n", test_mode ? "true" : "false");

  fprintf(stderr, "Node size: %lu bytes\n", sizeof(node) + ((order - 1) * sizeof(int)) + (order * sizeof(void *)));

  if (seed == 0)
    srand((int)time(0));
  else
    srand(seed);

  root = NULL;
  if (test_mode == true)
  {
    fprintf(stderr, "Now doing correctness test\n");
    fprintf(stderr, "Sequential test\n");
    testseq(false);
    fprintf(stderr, "\n\n");

    fprintf(stderr, "Parallel test\n");
    test(range, update_rate, num_threads, false);
    fprintf(stderr, "\n\n");
  }
  else
  {
    if (initial_count > 0)
    {
      fprintf(stderr, "Now pre-filling %d random elements...\n", initial_count);
      initial_add(initial_count, range);
      fprintf(stderr, "...Done!\n\n");
    }

    start_benchmark(range, update_rate, num_threads);
  }

  pthread_rwlock_destroy(&rwlock);

  return 0;
}
