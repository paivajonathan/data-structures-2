#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
  int value;
  struct Node *next;
} Node;

typedef struct Graph {
  int num_elements;
  Node **elements_list;
} Graph;

Graph *create_graph(void) {
  Graph *graph = calloc(sizeof(Graph), 1);
  if (!graph) exit(1);
  graph->num_elements = 0;
  graph->elements_list = NULL;
  return graph;
}

Node *create_node(int value) {
  Node *node = calloc(sizeof(Node), 1);
  if (!node) {
    puts("Ocorreu um erro de alocação!");
    exit(1);
  }
  node->value = value;
  node->next = NULL;
  return node;
}

void insert_element(Graph *graph, int value) {
  if (!graph->elements_list) {
    graph->elements_list = calloc(sizeof(Node *), 1);
  } else {
    graph->elements_list = realloc(
      graph->elements_list,
      sizeof(Node *) * (graph->num_elements + 1)
    );
  }
  if (!graph->elements_list) exit(1);

  Node *node = create_node(value);
  graph->elements_list[graph->num_elements++] = node;
}

Node *search_element(Graph *graph, int value) {
  if (!graph->elements_list) return NULL;
  Node *cursor = NULL;
  for (int i = 0; i < graph->num_elements; i++) {
    cursor = graph->elements_list[i];
    if (value == cursor->value)
      return cursor;
  }
  return NULL;
}

void create_link(Graph *graph, int source_value, int destination_value) {
  if (!graph->elements_list) return;

  Node *source_element = search_element(graph, source_value);
  Node *destination_element = search_element(graph, destination_value);

  if (!source_element) return;
  if (!destination_element) return;

  Node *cursor = source_element;
  while (cursor->next) {
    cursor = cursor->next;
    if (cursor->value == destination_value)
      return;
  }
  cursor->next = create_node(destination_value);

  cursor = destination_element;
  while (cursor->next) {
    cursor = cursor->next;
    if (cursor->value == source_value)
      return;
  }
  cursor->next = create_node(source_value);
  
  cursor = NULL;
}

void print_graph(Graph *graph) {
  if (!graph->elements_list) return;
  Node *cursor = NULL;
  for (int i = 0; i < graph->num_elements; i++) {
    cursor = graph->elements_list[i];
    printf("%d -> ", cursor->value);
    while (cursor->next) {
      printf("%d -> ", cursor->next->value);
      cursor = cursor->next;
    }
    printf("\n");
  }
}

void destroy_graph(Graph *graph) {
  if (!graph->elements_list) return;
  graph->num_elements = 0;
  free(graph->elements_list);
  graph->elements_list = NULL;
}

int main(void) {
  Graph *graph = create_graph();
  insert_element(graph, 1);
  insert_element(graph, 2);
  insert_element(graph, 3);
  insert_element(graph, 4);
  insert_element(graph, 5);

  create_link(graph, 1, 1);
  create_link(graph, 1, 1);

  print_graph(graph);
  destroy_graph(graph);
  return 0;
}