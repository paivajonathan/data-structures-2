#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_VALUES 7

typedef struct TreeNode {
  int value;
  int left;
  int right;
} TreeNode;

TreeNode *binary_tree[MAX_VALUES] = {NULL};
int values_list[MAX_VALUES] = {0};

void insertion_sort(int arr[], int n)
{
  int i, key, j;
  for (i = 1; i < n; i++)
  {
    key = arr[i];
    j = i - 1;
    while (j >= 0 && arr[j] > key)
    {
      arr[j + 1] = arr[j];
      j = j - 1;
    }
    arr[j + 1] = key;
  }
}

void generate_values(int min, int max)
{
  int new_random;
  bool unique;

  for (int i = 0; i < MAX_VALUES; i++)
  {
    do
    {
      new_random = (rand() % (max - min + 1)) + min;
      unique = true;

      for (int j = 0; j < i; j++)
        if (values_list[j] == new_random)
          unique = false;
    } while (!unique);
    values_list[i] = new_random;
  }
  insertion_sort(values_list, MAX_VALUES);
}

TreeNode *create_node(int value) {
  TreeNode *node = calloc(sizeof(TreeNode), 1);
  if (node == NULL) exit(1);
  node->value = value;
  node->left = -1;
  node->right = -1;
  return node;
}

int generate_tree_aux(int start, int end, int next_index)
{
  if (start > end)
    return -1;
  int mid = (start + end) / 2;
  binary_tree[next_index] = create_node(values_list[mid]);
  binary_tree[next_index]->left = generate_tree_aux(start, mid - 1, 2 * next_index + 1);
  binary_tree[next_index]->right = generate_tree_aux(mid + 1, end, 2 * next_index + 2);
  return next_index;
}

void generate_tree(void) { generate_tree_aux(0, MAX_VALUES - 1, 0); }

void print_tree(void)
{
  for (int i = 0; i < MAX_VALUES; i++)
  {
    printf("[%d]<-[%d]->[%d]\n", binary_tree[i]->left, binary_tree[i]->value, binary_tree[i]->right);
  }
}

int main(void) {
  srand(time(NULL));
  generate_values(1, MAX_VALUES);
  generate_tree();
  print_tree();
  return 0;
}
