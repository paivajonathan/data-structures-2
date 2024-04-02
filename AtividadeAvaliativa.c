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

int max_value(int a, int b)
{
  return (a >= b) ? a : b;
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
void append_on_list(Flight *flight)
{
  if (!flight_list)
  {
    flight_list = calloc(sizeof(Flight *), 1);
  }
  else
  {
    flight_list = realloc(flight_list, sizeof(Flight *) * (flight_list_length + 1));
  }

  if (flight_list == NULL)
  {
    fprintf(stderr, "Memory allocation failed\n");
    exit(EXIT_FAILURE);
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

void print_flight(Flight *root, int spaces)
{
  if (!root)
    return;
  char delimiter = spaces ? ' ' : '\0';
  printf("\n%*cNumero: %d\n", spaces, delimiter, root->number);
  printf("%*cOrigem: %s\n", spaces, delimiter, root->origin);
  printf("%*cDestino: %s\n", spaces, delimiter, root->destiny);
  printf("%*cAssentos disponiveis: %d\n", spaces, delimiter, root->seats);
  printf("%*cData: %02d/%02d/%04d\n", spaces, delimiter, root->date.day, root->date.month, root->date.year);
  printf("%*cHora: %02d:%02d\n", spaces, delimiter, root->time.hours, root->time.minutes);
}

void print_list(void)
{
  if (!flight_list)
    puts("\nNenhum voo encontrado.");

  for (int i = 0; i < flight_list_length; i++)
    print_flight(flight_list[i], 0);
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

int get_tree_height(Flight *node)
{
  if (node == NULL)
    return 0;

  return 1 + max_value(get_tree_height(node->left), get_tree_height(node->right));
}

bool is_tree_balanced(Flight *root)
{
  int left_height;
  int right_height;

  if (root == NULL)
    return true;

  left_height = get_tree_height(root->left);
  right_height = get_tree_height(root->right);

  if (abs(left_height - right_height) <= 1 && is_tree_balanced(root->left) &&
      is_tree_balanced(root->right))
    return true;

  return false;
}

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

void print_flight_tree(Flight *root, int level)
{
  if (!root)
    return;
  print_flight_tree(root->right, level + 8);
  print_flight(root, level);
  print_flight_tree(root->left, level + 8);
}

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
Flight *search_flight_by_number(Flight *root, int number)
{
  if (!root)
    return root;

  if (number < root->number)
    return search_flight_by_number(root->left, number);

  else if (number > root->number)
    return search_flight_by_number(root->right, number);

  return root;
}

int search_flights_by_data(Flight *root, char *origin, char *destiny, Date date)
{
  if (!root)
    return 0;

  int total_count = 0;

  total_count += search_flights_by_data(root->right, origin, destiny, date);

  if (
      strcmp(root->origin, origin) == 0 &&
      strcmp(root->destiny, destiny) == 0 &&
      root->date.year == date.year &&
      root->date.month == date.month &&
      root->date.day == date.day)
  {
    total_count++;
    print_flight(root, 0);
  }

  total_count += search_flights_by_data(root->left, origin, destiny, date);

  return total_count;
}

int list_flight_by_max_seats(Flight *root)
{
  if (!root)
    return 0;

  int total_count = 0;

  total_count += list_flight_by_max_seats(root->right);

  if (root->seats < 10)
  {
    total_count++;
    print_flight(root, 0);
  }

  total_count += list_flight_by_max_seats(root->left);

  return total_count;
}

int count_flights(Flight *root)
{
  if (!root)
    return 0;

  int total_count = 0;

  total_count += count_flights(root->left);

  total_count++;

  total_count += count_flights(root->right);

  return total_count;
}

int list_flights_with_disponible_seats(Flight *root)
{
  if (!root)
    return 0;

  int total_count = 0;

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
Flight *insert_flight_helper(Flight *root)
{
  Flight *new_flight = calloc(sizeof(Flight), 1);
  if (!new_flight)
    exit(1);

  new_flight->number = current_flight_id;

  printf("Digite uma origem para o voo:\n");
  scanf(" %50[^\n]", new_flight->origin);
  clear_buffer();

  printf("Digite um destino para o voo:\n");
  scanf(" %50[^\n]", new_flight->destiny);
  clear_buffer();

  do
  {
    printf("Digite a quantidade de assentos disponiveis:\n");
    scanf("%d", &(new_flight->seats));
    clear_buffer();
    if (new_flight->seats >= 0)
      break;
    puts("Quantidade invalida!");
  } while (true);

  do
  {
    printf("Digite o ano do voo:\n");
    scanf("%d", &(new_flight->date.year));
    clear_buffer();
    if (new_flight->date.year > 1900)
      break;
    puts("Ano invalido!");
  } while (true);

  do
  {
    printf("Digite o mes do voo:\n");
    scanf("%d", &(new_flight->date.month));
    clear_buffer();
    if (new_flight->date.month >= 1 && new_flight->date.month <= 12)
      break;
    puts("Mes invalido!");
  } while (true);

  do
  {
    printf("Digite o dia do voo:\n");
    scanf("%d", &(new_flight->date.day));
    clear_buffer();
    if (new_flight->date.day >= 1 && new_flight->date.day <= 31)
      break;
    puts("Dia invalido!");
  } while (true);

  do
  {
    puts("Digite o horario do voo:");
    scanf("%d", &(new_flight->time.hours));
    clear_buffer();
    if (new_flight->time.hours >= 0 && new_flight->time.hours < 23)
      break;
    puts("Horario invalido");
  } while (true);

  do
  {
    puts("Digite os minutos:");
    scanf("%d", &(new_flight->time.minutes));
    clear_buffer();
    if (new_flight->time.minutes >= 0 && new_flight->time.minutes < 59)
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

  return root;
}

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

  if (!search_flight_by_number(root, number))
  {
    puts("\nNao foi encontrado um voo com esse numero.");
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

void search_flights_by_data_helper(Flight *root)
{
  if (!root)
  {
    puts("Nao ha voos disponiveis!");
    return;
  }

  char origin[50];
  char destiny[50];
  Date date;

  printf("Digite a origem do voo:\n");
  scanf("%50s", origin);
  clear_buffer();

  printf("Digite o destino:\n");
  scanf("%50s", destiny);
  clear_buffer();

  do
  {
    printf("Digite o ano do voo:\n");
    scanf("%d", &(date.year));
    clear_buffer();
    if (date.year > 1900)
      break;
    puts("Ano invalido!");
  } while (true);

  do
  {
    printf("Digite o mes do voo:\n");
    scanf("%d", &(date.month));
    clear_buffer();
    if (date.month >= 1 && date.month <= 12)
      break;
    puts("Mes invalido!");
  } while (true);

  do
  {
    printf("Digite o dia do voo:\n");
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
    return flight_a->time.hours != flight_b->time.hours;
  
  return flight_a->time.minutes != flight_b->time.minutes;
}

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

Flight *generate_random_flights(Flight *root)
{
  if (count_flights(root) != 0)
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
      exit(EXIT_FAILURE);
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

void print_flight_tree_helper(Flight *root)
{
  if (!root)
  {
    puts("Nao ha voos disponiveis!");
    return;
  }
  puts("Mostrando todos os voos...");
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
