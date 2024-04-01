#include <stdio.h>
#include <stdlib.h>

typedef struct TreeNode {
  int value;
  struct TreeNode *left;
  struct TreeNode *right;
} TreeNode;

/*
* Allocates the memory needed for a new node.
* @param value the value of the node
*/
TreeNode *
create_node(int value)
{
  TreeNode *new_node = calloc(sizeof(TreeNode), 1);
  if (!new_node) exit(1);
  new_node->value = value;
  new_node->right = NULL;
  new_node->left = NULL;
  return new_node;
}

/*
* Generates tree from an array, finding always the median from the set of values.
* @param values[] the array of values to generate the tree
* @param start the start of the subarray
* @param end the end of the subarray
*/
TreeNode *
generate_tree(int values[], int start, int end)
{
  if (start > end)
    return NULL;
  int mid = (start + end + 1) / 2;
  TreeNode *root = create_node(values[mid]);
  root->left = generate_tree(values, start, mid - 1);
  root->right = generate_tree(values, mid + 1, end);
  return root;
}

/*
* Searches for a node that holds a value equal to the given value as argument.
*/
TreeNode *
search_node(TreeNode *root, int value)
{
  if (!root)
    return root;

  if (value < root->value)
    return search_node(root->left, value);
  else if (value > root->value)
    return search_node(root->right, value);

  return root;
}

/*
* Searches for the certain node that doesn't exist yet, to create it in the tree.
*/
TreeNode *
insert_node(TreeNode *root, int value)
{
  if (!root)
  {
    root = create_node(value);
    return root;
  }
  
  if (value < root->value)
    root->left = insert_node(root->left, value);
  else if (value > root->value)
    root->right = insert_node(root->right, value);

  return root;
}

/*
* Searches for a node that has value equal to the value given,
* to delete it from the tree.
*/
TreeNode *
delete_node(TreeNode *root, int value)
{
  if (!root)
    return root;

  if (value < root->value)
    root->left = delete_node(root->left, value);
  else if (value > root->value)
    root->right = delete_node(root->right, value);
  else
  {
    // If it doesn't have any children, free its memory and return NULL to who called.
    if (!root->left && !root->right)
    {
      free(root);
      return NULL;
    } 
    
    // If it doesn't have only the left child, return the right child to who called, after freeing its memory
    if (!root->left)
    {
      TreeNode *temp = root->right;
      free(root);
      return temp;
    }
    
    // If it doesn't have only the right child, return the left child to who called
    if (!root->right)
    {
      TreeNode *temp = root->left;
      free(root);
      return temp;
    }
 
    // And if it has the two possible children, find the minimum successor,
    // that the current root will receive its value, and the node that had that value
    // is deleted. That node will always be a leaf one.
    // Ex.: Root = 4, Minimum Successor = 5.
    TreeNode *minimum_successor = root->right;
    while (minimum_successor->left)
      minimum_successor = minimum_successor->left;
    root->value = minimum_successor->value;
    root->right = delete_node(root->right, minimum_successor->value);
  }

  // Return current root after operations
  return root;
}

TreeNode *
update_node(TreeNode *root, int target_value, int new_value) {
  TreeNode *search = search_node(root, target_value);
  if (!search) {
    puts("Value not found.");
    return root;
  }
  root = delete_node(root, target_value);
  root = insert_node(root, new_value);
  return root;
}

/*
* Prints each element from the tree, adding spaces to make it better observable.
*/
void
print_tree(TreeNode *root, int level)
{
  if (!root) return;
  print_tree(root->right, level + 4);
  printf("%*c%d\n", level, ' ', root->value);
  print_tree(root->left, level + 4);
}

void
destroy_tree(TreeNode *node)
{
  if (!node) return;
  destroy_tree(node->left);
  destroy_tree(node->right);
  free(node);
}

int
main(void)
{
  int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
  int length = sizeof(values) / sizeof(values[0]);
  TreeNode *root = generate_tree(values, 0, length - 1);
  root = update_node(root, 9, 10);
  print_tree(root, 0);
  destroy_tree(root);
  return 0;
}