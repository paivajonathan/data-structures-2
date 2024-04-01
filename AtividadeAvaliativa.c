#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct Flight
{
  int number;
  char origin[50];
  char destiny[50];
  int dispon_seats;
  int time;
  time_t date;
  struct Flight *right;
  struct Flight *left;
} Flight;

Flight **flight_list = NULL;
int flight_list_length = 0;

void append_on_list(Flight *flight)
{
  if (!flight_list)
  {
    flight_list = calloc(sizeof(Flight), 1);
  }
  else
  {
    flight_list = realloc(flight_list, sizeof(Flight) * 1);
  }
  flight_list[flight_list_length++] = flight;
}

void append_flights_on_list(Flight *root)
{
  if (!root)
    return;
  append_flights_on_list(root->left);
  append_on_list(root);
  append_flights_on_list(root->right);
}

void print_list(void)
{
  for (int i = 0; i < flight_list_length; i++)
    printf("Number: %d", flight_list[i]->number);
}

void destroy_list(void)
{
  free(flight_list);
  flight_list = NULL;
}

Flight *generate_flights_tree(Flight **values, int start, int end)
{
  if (start > end)
    return NULL;
  int mid = (start + end + 1) / 2;
  Flight *root = values[mid];
  root->left = generate_flights_tree(values, start, mid - 1);
  root->right = generate_flights_tree(values, mid + 1, end);
  return root;
}

Flight *create_flight(int number)
{
  Flight *new_flight = calloc(sizeof(Flight), 1);
  if (!new_flight)
    exit(1);
  new_flight->number = number;
  strcpy(new_flight->origin, "");
  strcpy(new_flight->destiny, "");
  new_flight->dispon_seats = 0;
  new_flight->time = 0;
  new_flight->date = 0;
  new_flight->left = NULL;
  new_flight->right = NULL;
  return new_flight;
}

/* Verificar se tá balanceada */
int max_value(int a, int b)
{
  return (a >= b) ? a : b;
}

int height(Flight *node)
{
  if (node == NULL)
    return 0;

  return 1 + max_value(height(node->left), height(node->right));
}

bool is_balanced(Flight *root)
{
  int left_height;
  int right_height;

  if (root == NULL)
    return true;

  left_height = height(root->left);
  right_height = height(root->right);

  if (abs(left_height - right_height) <= 1 && is_balanced(root->left) &&
      is_balanced(root->right))
    return true;

  return false;
}
/* Verificar se tá balanceada */

Flight *insert(Flight *root, Flight *new_flight)
{
  if (!root)
  {
    root = new_flight;
    return root;
  }

  if (new_flight->number < root->number)
    root->left = insert(root->left, new_flight);

  else if (new_flight->number > root->number)
    root->right = insert(root->right, new_flight);

  return root;
}

Flight *register_flight(Flight *root)
{
  Flight *new_flight = calloc(sizeof(Flight), 1);
  if (!new_flight)
    exit(1);
  printf("Digite um número para o voo:\n");
  scanf("%d", &(new_flight->number));

  printf("Digite uma origem para o voo:\n");
  scanf("%s", new_flight->origin);

  printf("Digite um destino para o voo:\n");
  scanf("%s", new_flight->destiny);

  printf("Digite a quantidade de assentos disponíveis:\n");
  scanf("%d", &(new_flight->dispon_seats));

  printf("Digite o horário do voo:\n");
  scanf("%d", &(new_flight->time));

  printf("Digite a data do voo:\n");
  scanf("%ld", &(new_flight->date));

  new_flight->left = NULL;
  new_flight->right = NULL;

  root = insert(root, new_flight);

  if (!is_balanced(root))
  {
    append_flights_on_list(root);
    root = generate_flights_tree(flight_list, 0, flight_list_length - 1);
  }

  return root;
}

Flight *delete_flight(Flight *root, int number)
{
  if (!root)
    return root;

  if (number < root->number)
    root->left = delete_flight(root->left, number);
  else if (number > root->number)
    root->right = delete_flight(root->right, number);
  else
  {
    // If it doesn't have any children, free its memory and return NULL to who
    // called.
    if (!root->left && !root->right)
    {
      free(root);
      return NULL;
    }

    // If it doesn't have only the left child, return the right child to who
    // called, after freeing its memory
    if (!root->left)
    {
      Flight *temp = root->right;
      free(root);
      return temp;
    }

    // If it doesn't have only the right child, return the left child to who
    // called
    if (!root->right)
    {
      Flight *temp = root->left;
      free(root);
      return temp;
    }

    // And if it has the two possible children, find the minimum successor,
    // that the current root will receive its number, and the node that had that
    // number is deleted. That node will always be a leaf one. Ex.: Root = 4,
    // Minimum Successor = 5.
    Flight *minimum_successor = root->right;
    while (minimum_successor->left)
      minimum_successor = minimum_successor->left;
    root->number = minimum_successor->number;
    root->right = delete_flight(root->right, minimum_successor->number);
  }

  return root;
}

void print_flight(Flight *root)
{
  if (!root)
    return;
  printf("Número: %d\n", root->number);
  printf("Origem: %s\n", root->origin);
  printf("Destino: %s\n", root->destiny);
  printf("Assentos disponíveis: %d\n", root->dispon_seats);
  printf("Hora: %d\n", root->time);
  printf("Data %ld\n\n", root->date);
}

void search_flights(Flight *root, char *origin, char *destiny, time_t date)
{
  if (!root)
    return;
  search_flights(root->right, origin, destiny, date);
  if (strcmp(root->origin, origin) == 0 &&
      strcmp(root->destiny, destiny) == 0 && root->date == date)
    print_flight(root);
  search_flights(root->left, origin, destiny, date);
}

void print_flight_tree(Flight *root, int level)
{
  if (!root)
    return;
  print_flight_tree(root->right, level + 4);
  // printf("%*c%d\n", level, ' ', root->number);
  print_flight(root);
  print_flight_tree(root->left, level + 4);
}

void destroy_flight_tree(Flight *root)
{
  if (!root)
    return;
  destroy_flight_tree(root->left);
  destroy_flight_tree(root->right);
  free(root);
}

int main(void)
{
  Flight *root = NULL;
  root = register_flight(root);
  root = register_flight(root);
  print_flight_tree(root, 0);
  destroy_list();
  destroy_flight_tree(root);
  return 0;
}