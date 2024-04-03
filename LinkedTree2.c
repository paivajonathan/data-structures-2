#include <stdio.h>
#include <stdlib.h>

typedef struct TreeNode 
{
  int value;
  struct TreeNode *parent;
  struct TreeNode *leftNode;
  struct TreeNode *rightNode;
} TreeNode;

TreeNode *
create_node(int value)
{
  TreeNode *node = calloc(sizeof(TreeNode), 1);
  if (node == NULL)
  {
    fprintf(stderr, "An error happened while allocating a TreeNode");
    exit(1);
  }
  node->value = value;
  node->parent = NULL;
  node->leftNode = NULL;
  node->rightNode = NULL;
  return node;
}

void
print_spaces(int quantity)
{
  for (int i = 0; i < quantity; i++)
    printf(" ");
}

void print_tree_aux(TreeNode *root, int level) {
  if (!root) return;
  print_tree_aux(root->rightNode, level + 4);
  print_spaces(level);
  printf("%d\n", root->value);
  print_tree_aux(root->leftNode, level + 4);
}

void print_tree(TreeNode *root) { print_tree_aux(root, 0); puts(""); }

TreeNode *
generate_tree(int array[], int startIndex, int endIndex, TreeNode *parent)
{
  if (startIndex > endIndex) return NULL;
  int midIndex = (endIndex + startIndex + 1) / 2;
  TreeNode *root = create_node(array[midIndex]);
  root->parent = parent;
  root->leftNode = generate_tree(array, startIndex, midIndex - 1, root);
  root->rightNode = generate_tree(array, midIndex + 1, endIndex, root);
  return root;
}

void
insert_value(TreeNode *root, int value)
{
  TreeNode *cursor = root;
  TreeNode *parent = NULL;
  while (1)
  {
    if (!cursor)
    {
      if (value < parent->value)
        parent->leftNode = create_node(value);
      else
        parent->rightNode = create_node(value);
      break;
    }
  
    if (value < cursor->value)
    {
      parent = cursor;
      cursor = cursor->leftNode;
    }
    else if (value > cursor->value)
    {
      parent = cursor;
      cursor = cursor->rightNode;
    } else
      break;
  }
}

TreeNode *
search_value(TreeNode *root, int value)
{
  TreeNode *node = root;
  while (node)
  {
    if (value == node->value)
      return node;
    else if (value < node->value)
      node = node->leftNode;
    else if (value > node->value)
      node = node->rightNode;
  }
  return NULL;
}

TreeNode *
find_minimum(TreeNode *node)
{
  while (node->leftNode != NULL)
    node = node->leftNode;
  return node;
}

int
is_right_child(TreeNode *node) {
  return node->parent->rightNode == node;
}

void
delete_value(TreeNode *root, int value)
{
  TreeNode *node = search_value(root, value);
  
  if (!node) return;

  if (!node->leftNode && !node->rightNode)
  {
    if (node != root)
    {
      if (is_right_child(node))
        node->parent->rightNode = NULL;
      else
        node->parent->leftNode = NULL;
    } else
      root = NULL;
    node->parent = NULL;
    free(node);
    node = NULL;
  }
  else if (node->leftNode && node->rightNode)
  {
    TreeNode *successor = find_minimum(node->rightNode);
    node->value = successor->value;
    
    if (is_right_child(successor))
      successor->parent->rightNode = NULL;
    else 
      successor->parent->leftNode = NULL;
    
    successor->parent = NULL;
    free(successor);
    successor = NULL;    
  }
  else
  {
    TreeNode *childNode = node->rightNode != NULL
      ? node->rightNode
      : node->leftNode; 
    
    if (node != root)
    {
      if (is_right_child(node))
        node->parent->rightNode = childNode;
      else
        node->parent->leftNode = childNode;
    } else
      root = childNode;

    childNode->parent = node->parent;
    free(node);
    node = NULL;
  }
}

void
free_tree(TreeNode *root) {
  if (!root) return;
  free_tree(root->leftNode);
  root->leftNode = NULL;
  free_tree(root->rightNode);
  root->rightNode = NULL;
  free(root);
  root = NULL;
}

int main(void) {
  int values[] = {1, 2, 3, 4, 5, 6, 7};
  int length = sizeof(values) / sizeof(values[0]);
  TreeNode *root = NULL;
  root = generate_tree(values, 0, length - 1, NULL);
  puts("\nAntes:\n");
  print_tree(root);
  insert_value(root, 10);
  insert_value(root, 9);
  insert_value(root, 10);
  puts("\nDepois:\n");
  print_tree(root);
  free_tree(root);
  return 0;
}
