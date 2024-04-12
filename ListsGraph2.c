#include <stdio.h>
#include <stdlib.h>

typedef struct Node
{
  int data;
  int num_neighbors;
  struct Node **neighbors;
} Node;

typedef struct Graph
{
  int num_elements;
  Node **elements;
} Graph;

Graph *create_graph(void)
{
  Graph *graph = calloc(sizeof(Graph), 1);
  if (!graph) exit(1);
  graph->num_elements = 0;
  graph->elements = NULL;
  return graph;
}

Node *create_node(int value)
{
  Node *node = calloc(sizeof(Node), 1);
  if (!node) exit(1);
  node->data = value;
  node->num_neighbors = 0;
  node->neighbors = NULL;
  return node;
}

void append(Node * **list, int * size, Node *element) {
  if (!(*size))
    (*list) = calloc(sizeof(Node *), 1);
  else
    (*list) = realloc((*list), sizeof(Node *) * ((*size) + 1));

  if (!(*list))
    exit(1);

  (*list)[(*size)++] = element;
}

Node *search(Node **list, int size, int value)
{
  for (int i = 0; i < size; i++)
  {
    if (list[i]->data == value)
      return list[i];
  }
  return NULL;
}

void insert_node(Graph *graph, int value)
{
  Node *node = create_node(value);
  append(&(graph->elements), &(graph->num_elements), node);
}

void insert_edge(Graph *graph, int from, int to)
{
  Node *from_element = search(graph->elements, graph->num_elements, from);
  Node *to_element = search(graph->elements, graph->num_elements, to);
  
  if (!from_element || !to_element)
  {
    puts("Um dos elementos não existe!");
    return;
  }

  if (search(from_element->neighbors, from_element->num_neighbors, to_element->data))
  {
    puts("Vizinho já existe!");
    return;
  }
  
  append(&(from_element->neighbors), &(from_element->num_neighbors), to_element);
  if (from_element != to_element)
    append(&(to_element->neighbors), &(to_element->num_neighbors), from_element);
}

void print_graph(Graph *graph)
{
  Node *current_element = NULL;

  for (int i = 0; i < graph->num_elements; i++)
  {
    current_element = graph->elements[i];
    printf("%d:", current_element->data);

    printf(" (");
    for (int j = 0; j < current_element->num_neighbors; j++)
      printf("%d,", current_element->neighbors[j]->data);
    printf(")\n");
  }
}

int main(void)
{
  Graph *graph = create_graph();
  
  insert_node(graph, 1);
  insert_node(graph, 2);
  insert_node(graph, 3);

  insert_edge(graph, 1, 2);
  
  print_graph(graph);
  
  return 0;
}
