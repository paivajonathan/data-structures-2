#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ==================== UTILS ==================== */
void clear_buffer(void)
{
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

void print_menu(void)
{
  puts("\n1: Cadastrar voo");
  puts("2: Excluir voo");
  puts("3: Gerar voos aleatorios");
  puts("4: Visualizar voos");
  puts("5: Pesquisar voos com base na origem, destino e data");
  puts("6: Listar voos com menos de 10 assentos disponiveis");
  puts("7: Visualizar o numero total de voos disponiveis");
  puts("8: Listar voos com assentos disponiveis ordenados por data e hora");
  puts("0: Sair do programa\n");
}

int get_max_value(int a, int b)
{
  return (a >= b) ? a : b;
}

void generate_random_string(char name[], int length)
{
  for (int i = 0; i < length; i++)
    name[i] = 'a' + rand() % 26;
  name[length] = '\0';
}

int generate_random_integer(int min, int max)
{
  return min + rand() % (max - min + 1);
}
/* ==================== UTILS ==================== */

/* ==================== STRUCTURES ==================== */
typedef struct Date
{
  int year;
  int month;
  int day;
} Date;

typedef struct Time
{
  int hours;
  int minutes;
} Time;

typedef struct Flight
{
  int number;
  char origin[51];
  char destiny[51];
  int seats;
  Date date;
  Time time;
  struct Flight *right;
  struct Flight *left;
} Flight;

int current_flight_id = 1;

Flight **flight_list = NULL;
int flight_list_length = 0;
/* ==================== STRUCTURES ==================== */

/* ==================== LIST ==================== */

/**
 * Places flight structure pointer into list last position.
*/
void append_on_list(Flight *flight)
{
  // List isn't created yet
  if (!flight_list)
  {
    flight_list = calloc(sizeof(Flight *), 1);
  }
  // List already has values
  else
  {
    flight_list = realloc(flight_list, sizeof(Flight *) * (flight_list_length + 1));
  }

  // If it's still NULL
  if (!flight_list)
  {
    fprintf(stderr, "Ocorreu um erro de alocacao de memoria\n");
    exit(EXIT_FAILURE);
  }

  // Appends on last position
  flight_list[flight_list_length++] = flight;
}

void print_flight(Flight *root)
{
  if (!root)
    return;
  printf("\nNumero: %d\n", root->number);
  printf("Origem: %s\n", root->origin);
  printf("Destino: %s\n", root->destiny);
  printf("Assentos disponiveis: %d\n", root->seats);
  printf("Data: %02d/%02d/%04d\n", root->date.day, root->date.month, root->date.year);
  printf("Hora: %02d:%02d\n", root->time.hours, root->time.minutes);
}

void print_list(void)
{
  if (!flight_list)
    puts("\nNenhum voo encontrado.");

  for (int i = 0; i < flight_list_length; i++)
    print_flight(flight_list[i]);
}

void destroy_list(void)
{
  if (!flight_list)
    return;
  flight_list_length = 0;
  free(flight_list);
  flight_list = NULL;
}
/* ==================== LIST ==================== */

/* ==================== TREE ==================== */

/**
 * Places several flight structures into the list, following the tree ascending order. 
*/
void append_flights_on_list(Flight *root)
{
  if (!root)
    return;
  append_flights_on_list(root->left);
  append_on_list(root);
  append_flights_on_list(root->right);
}

/**
 * Creates the flights tree based on the flights placed into the list.
*/
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

/**
 * Searches flight based on its number.
*/
Flight *search_flight(Flight *root, int number)
{
  if (!root)
    return root;

  if (number < root->number)
    return search_flight(root->left, number);

  else if (number > root->number)
    return search_flight(root->right, number);

  return root;
}

/**
 * Puts flight pointer in the correct place, increasing current flight id.
*/
Flight *insert_flight(Flight *root, Flight *new_flight)
{
  if (!root)
  {
    root = new_flight;
    current_flight_id++;
    return root;
  }

  if (new_flight->number < root->number)
    root->left = insert_flight(root->left, new_flight);

  else if (new_flight->number > root->number)
    root->right = insert_flight(root->right, new_flight);

  return root;
}

/**
 * Deletes flight pointer from the tree.
*/
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

/**
 * Calculates the height of the tree.
 * 
 * The height of a tree is defined as the length of the longest path
 * from the root node to a leaf node.
 * 
 * The height of an empty tree (NULL pointer) is considered 0.
 */
int get_tree_height(Flight *flight)
{
  if (!flight)
    return 0;

  return 1 + get_max_value(get_tree_height(flight->left), get_tree_height(flight->right));
}

/**
 * Determines whether the binary tree is balanced or not.
 * 
 * A binary tree is considered balanced if the heights of its left and right subtrees
 * at every node differ by at most 1.
*/
bool is_tree_balanced(Flight *root)
{
  int left_height;
  int right_height;

  if (!root)
    return true;

  left_height = get_tree_height(root->left);
  right_height = get_tree_height(root->right);

  if (abs(left_height - right_height) <= 1 && is_tree_balanced(root->left) &&
      is_tree_balanced(root->right))
    return true;

  return false;
}

/**
 * Compares flights based on date and time values.
 * 
 * It returns an integer indicating the relative order between fields.
 * 
 * It's intended to use as a callback function in qsort algorithm.
*/
int compare_flights_by_datetime(const void *a, const void *b)
{
  Flight *flight_a = *(Flight **)a;
  Flight *flight_b = *(Flight **)b;

  if (flight_a->date.year != flight_b->date.year)
    return flight_a->date.year - flight_b->date.year;

  if (flight_a->date.month != flight_b->date.month)
    return flight_a->date.month - flight_b->date.month;

  if (flight_a->date.day != flight_b->date.day)
    return flight_a->date.day - flight_b->date.day;

  if (flight_a->time.hours != flight_b->time.hours)
    return flight_a->time.hours - flight_b->time.hours;

  return flight_a->time.minutes - flight_b->time.minutes;
}

/**
 * Shows the tree in a way it's easier to see its properties
*/
void print_flight_tree(Flight *root, int level)
{
  if (!root)
    return;
  print_flight_tree(root->right, level + 4);
  printf("%*c-%d-\n", level, ' ', root->number);
  print_flight_tree(root->left, level + 4);
}

/**
 * Frees tree memory.
*/
void destroy_flight_tree(Flight *root)
{
  if (!root)
    return;
  destroy_flight_tree(root->left);
  destroy_flight_tree(root->right);
  free(root);
}
/* ==================== TREE ==================== */

/* ==================== RECURSIVE OPERATION FUNCTIONS ==================== */

/**
 * Shows all flights that match the specified origin, destiny and date.
 * Also creates a counter to get the number of matches, to be used by the function that calls this.
*/
int search_flights_by_data(Flight *root, char *origin, char *destiny, Date date)
{
  int total_count = 0;

  if (!root)
    return total_count;

  total_count += search_flights_by_data(root->left, origin, destiny, date);

  if (
      strcmp(root->origin, origin) == 0 &&
      strcmp(root->destiny, destiny) == 0 &&
      root->date.year == date.year &&
      root->date.month == date.month &&
      root->date.day == date.day)
  {
    total_count++;
    print_flight(root);
  }

  total_count += search_flights_by_data(root->right, origin, destiny, date);

  return total_count;
}

/**
 * Shows all flights that has less than 10 disponible seats,
 * creating a counter of all matches to be used in the function that calls this.
*/
int list_flight_by_max_seats(Flight *root)
{
  int total_count = 0;

  if (!root)
    return total_count;

  total_count += list_flight_by_max_seats(root->left);

  if (root->seats < 10)
  {
    total_count++;
    print_flight(root);
  }

  total_count += list_flight_by_max_seats(root->right);

  return total_count;
}

/**
 * Counts all flights from the tree.
*/
int count_flights(Flight *root)
{
  int total_count = 0;

  if (!root)
    return total_count;

  total_count += count_flights(root->left);

  total_count++;

  total_count += count_flights(root->right);

  return total_count;
}

/**
 * Gets all the flights that have disponible seats and appends to the auxiliary flight list,
 * to sort by date and time and print the result.
*/
int list_flights_with_disponible_seats(Flight *root)
{
  int total_count = 0;

  if (!root)
    return total_count;

  total_count += list_flights_with_disponible_seats(root->left);

  if (root->seats)
  {
    total_count++;
    append_on_list(root);
  }

  total_count += list_flights_with_disponible_seats(root->right);

  return total_count;
}
/* ==================== RECURSIVE OPERATION FUNCTIONS ==================== */

/* ==================== OPERATION FUNCTIONS ==================== */

/**
 * Registers a new flight.
*/
Flight *insert_flight_helper(Flight *root)
{
  Flight *new_flight = calloc(sizeof(Flight), 1);

  if (!new_flight)
  {
    fprintf(stderr, "Ocorreu um erro de alocacao de memoria\n");
    exit(EXIT_FAILURE);
  }

  new_flight->number = current_flight_id;

  printf("\nDigite uma origem para o voo:\n");
  scanf(" %50[^\n]", new_flight->origin);
  clear_buffer();

  printf("\nDigite um destino para o voo:\n");
  scanf(" %50[^\n]", new_flight->destiny);
  clear_buffer();

  do
  {
    printf("\nDigite a quantidade de assentos disponiveis:\n");
    scanf("%d", &(new_flight->seats));
    clear_buffer();
    if (new_flight->seats >= 0)
      break;
    puts("Quantidade invalida!");
  } while (true);

  do
  {
    printf("\nDigite o ano do voo:\n");
    scanf("%d", &(new_flight->date.year));
    clear_buffer();
    if (new_flight->date.year > 1900)
      break;
    puts("Ano invalido!");
  } while (true);

  do
  {
    printf("\nDigite o mes do voo:\n");
    scanf("%d", &(new_flight->date.month));
    clear_buffer();
    if (new_flight->date.month >= 1 && new_flight->date.month <= 12)
      break;
    puts("Mes invalido!");
  } while (true);

  do
  {
    printf("\nDigite o dia do voo:\n");
    scanf("%d", &(new_flight->date.day));
    clear_buffer();
    if (new_flight->date.day >= 1 && new_flight->date.day <= 31)
      break;
    puts("Dia invalido!");
  } while (true);

  do
  {
    puts("\nDigite o horario do voo:");
    scanf("%d", &(new_flight->time.hours));
    clear_buffer();
    if (new_flight->time.hours >= 0 && new_flight->time.hours < 23)
      break;
    puts("Horario invalido");
  } while (true);

  do
  {
    puts("\nDigite os minutos:");
    scanf("%d", &(new_flight->time.minutes));
    clear_buffer();
    if (new_flight->time.minutes >= 0 && new_flight->time.minutes <= 59)
      break;
    puts("Minutos invalidos");
  } while (true);

  new_flight->left = NULL;
  new_flight->right = NULL;

  root = insert_flight(root, new_flight);

  if (!is_tree_balanced(root))
  {
    destroy_list();
    append_flights_on_list(root);
    root = generate_flights_tree(flight_list, 0, flight_list_length - 1);
  }

  puts("\nVoo cadastrado com sucesso!");

  return root;
}

/**
 * Removes a flight based on its number.
*/
Flight *delete_flight_helper(Flight *root)
{
  if (!root)
  {
    puts("\nNao ha voos disponiveis!");
    return root;
  }

  int number = 0;

  do
  {
    puts("\nDigite o numero do voo a ser excluido:");
    scanf("%d", &number);
    clear_buffer();
    if (number > 0)
      break;
    puts("Numero invalido!");
  } while (true);

  if (!search_flight(root, number))
  {
    puts("\nNao foi encontrado um voo com esse numero.");
    return root;
  }

  root = delete_flight(root, number);

  if (!is_tree_balanced(root))
  {
    destroy_list();
    append_flights_on_list(root);
    root = generate_flights_tree(flight_list, 0, flight_list_length - 1);
  }

  puts("\nVoo excluido com sucesso!");

  return root;
}

/**
 * Gets the data the user wants to call the recursive function that searches the matches.
*/
void search_flights_by_data_helper(Flight *root)
{
  if (!root)
  {
    puts("\nNao ha voos disponiveis!");
    return;
  }

  char origin[50];
  char destiny[50];
  Date date;

  printf("\nDigite a origem do voo:\n");
  scanf(" %50[^\n]", origin);
  clear_buffer();

  printf("\nDigite o destino:\n");
  scanf(" %50[^\n]", destiny);
  clear_buffer();

  do
  {
    printf("\nDigite o ano do voo:\n");
    scanf("%d", &(date.year));
    clear_buffer();
    if (date.year > 1900)
      break;
    puts("Ano invalido!");
  } while (true);

  do
  {
    printf("\nDigite o mes do voo:\n");
    scanf("%d", &(date.month));
    clear_buffer();
    if (date.month >= 1 && date.month <= 12)
      break;
    puts("Mes invalido!");
  } while (true);

  do
  {
    printf("\nDigite o dia do voo:\n");
    scanf("%d", &(date.day));
    clear_buffer();
    if (date.day >= 1 && date.day <= 31)
      break;
    puts("Dia invalido!");
  } while (true);

  puts("\nListando todos os voos que possuem esses dados:");
  int quantity_found = search_flights_by_data(root, origin, destiny, date);

  if (!quantity_found)
    puts("Nenhum foi encontrado.");
}

/**
 * Starts the function responsible to show all the flights that have less than 10 seats.
*/
void list_flights_by_max_seats_helper(Flight *root)
{
  if (!root)
  {
    puts("\nNao ha voos disponiveis!");
    return;
  }

  puts("\nListando todos os voos com menos de 10 assentos disponiveis:");
  int quantity_found = list_flight_by_max_seats(root);

  if (!quantity_found)
    puts("Nenhum foi encontrado.");
}

/**
 * Starts the function responsible for counting all the disponible flights.
*/
void count_flights_helper(Flight *root)
{
  if (!root)
  {
    puts("\nNao ha voos disponiveis!");
    return;
  }

  int disponible_flights = count_flights(root);
  printf("\nExiste(m) %d voo(s) disponivel(is).\n", disponible_flights);
}

/**
 * Deletes data from the flight list, appends all the flights that have disponible seats,
 * orders the list by date and time and prints on screen.
*/
void list_flights_with_disponible_seats_helper(Flight *root)
{
  if (!root)
  {
    puts("\nNao ha voos disponiveis!");
    return;
  }

  destroy_list();
  int found_quantity = list_flights_with_disponible_seats(root);

  if (!found_quantity)
  {
    puts("\nNenhum voo encontrado.");
    return;
  }

  qsort(flight_list, flight_list_length, sizeof(Flight *), compare_flights_by_datetime);

  puts("\nListando todos os voos com assentos disponiveis:");
  print_list();
}

/**
 * Generates 7 flight pointers with random data, appending to the flight list,
 * to generate a brand new tree.
*/
Flight *generate_random_flights(Flight *root)
{
  if (count_flights(root))
  {
    puts("\nJa existem voos presentes.");
    return root;
  }

  int generated_count = 7;
  printf("\nGerando %d voos aleatorios...\n", generated_count);

  destroy_list();
  for (int i = 0; i < generated_count; i++)
  {
    Flight *generated_flight = calloc(sizeof(Flight), 1);

    if (!generated_flight)
    {
      fprintf(stderr, "Ocorreu um erro de alocacao de memoria\n");
      exit(EXIT_FAILURE);
    }

    generated_flight->number = current_flight_id++;

    generate_random_string(generated_flight->origin, 50);
    generate_random_string(generated_flight->destiny, 50);

    generated_flight->seats = generate_random_integer(0, 500);

    generated_flight->date.year = generate_random_integer(1900, 2100);
    generated_flight->date.month = generate_random_integer(1, 12);
    generated_flight->date.day = generate_random_integer(1, 31);

    generated_flight->time.hours = generate_random_integer(0, 23);
    generated_flight->time.minutes = generate_random_integer(0, 59);

    generated_flight->left = NULL;
    generated_flight->right = NULL;

    append_on_list(generated_flight);
  }

  root = generate_flights_tree(flight_list, 0, flight_list_length - 1);
  return root;
}

/**
 * Starts the tree printing recursive function.
*/
void print_flight_tree_helper(Flight *root)
{
  if (!root)
  {
    puts("\nNao ha voos disponiveis!");
    return;
  }

  puts("\nMostrando todos os voos...\n");
  print_flight_tree(root, 0);
}
/* ==================== OPERATION FUNCTIONS ==================== */

int main(void)
{
  srand(time(NULL));
  Flight *root = NULL;
  int option = -1;

  do
  {
    print_menu();
    printf("Digite uma opcao:\n");
    scanf("%d", &option);
    clear_buffer();

    if (option == 0)
    {
      puts("Saindo do programa...");
      break;
    }

    switch (option)
    {
    case 1:
      root = insert_flight_helper(root);
      break;
    case 2:
      root = delete_flight_helper(root);
      break;
    case 3:
      root = generate_random_flights(root);
      break;
    case 4:
      print_flight_tree_helper(root);
      break;
    case 5:
      search_flights_by_data_helper(root);
      break;
    case 6:
      list_flights_by_max_seats_helper(root);
      break;
    case 7:
      count_flights_helper(root);
      break;
    case 8:
      list_flights_with_disponible_seats_helper(root);
      break;
    default:
      puts("Digite uma opcao valida!\n");
      break;
    }
    option = -1;
  } while (true);

  destroy_list();
  destroy_flight_tree(root);

  return 0;
}
