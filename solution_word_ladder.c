//
// AED, November 2022 (TomÃ¡s Oliveira e Silva)
//
// Second practical assignement (speed run)
//
// Place your student numbers and names here
//   N.Mec. XXXXXX  Name: XXXXXXX
//
// Do as much as you can
//   1) MANDATORY: complete the hash table code
//      *) hash_table_create and hash_table_insert
//      *) find_word
//      *) hash_table_grow
//      *) hash_table_free
//      +) add code to get some statistical data about the hash table
//   2) HIGHLY RECOMMENDED: build the graph (including union-find data) -- use the similar_words function...
//      *) find_representative
//      *) add_edge
//   3) RECOMMENDED: implement breadth-first search in the graph
//      *) breadh_first_search
//   4) RECOMMENDED: list all words belonginh to a connected component
//      *) breadh_first_search
//      *) list_connected_component
//   5) RECOMMENDED: find the shortest path between to words
//      *) breadh_first_search
//      *) path_finder
//      *) test the smallest path from bem to mal
//         [ 0] bem
//         [ 1] tem
//         [ 2] teu
//         [ 3] meu
//         [ 4] mau
//         [ 5] mal
//      *) find other interesting word ladders
//   6) OPTIONAL: compute the diameter of a connected component and list the longest word chain
//      *) breadh_first_search
//      *) connected_component_diameter
//   7) OPTIONAL: print some statistics about the graph
//      *) graph_info
//   8) OPTIONAL: test for memory leaks
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// static configuration
//

#define _max_word_size_  32


//
// data structures (SUGGESTION --- you may do it in a different way)
//

typedef struct adjacency_node_s  adjacency_node_t;
typedef struct hash_table_node_s hash_table_node_t;
typedef struct hash_table_s      hash_table_t;

struct adjacency_node_s
{
  adjacency_node_t *next;            // link to th enext adjacency list node
  hash_table_node_t *vertex;         // the other vertex
};

struct hash_table_node_s
{
  // the hash table data
  char word[_max_word_size_];        // the word
  hash_table_node_t *next;           // next hash table linked list node
  // the vertex data
  adjacency_node_t *head;            // head of the linked list of adjancency edges
  int visited;                       // visited status (while not in use, keep it at 0)
  hash_table_node_t *previous;       // breadth-first search parent
  // the union find data
  hash_table_node_t *representative; // the representative of the connected component this vertex belongs to
  int number_of_vertices;            // number of vertices of the conected component (only correct for the representative of each connected component)
  int number_of_edges;               // number of edges of the conected component (only correct for the representative of each connected component)
};

struct hash_table_s
{
  unsigned int hash_table_size;      // the size of the hash table array
  unsigned int number_of_entries;    // the number of entries in the hash table
  unsigned int number_of_edges;      // number of edges (for information purposes only)
  hash_table_node_t **heads;         // the heads of the linked lists
};


//
// allocation and deallocation of linked list nodes (done)
//

static adjacency_node_t *allocate_adjacency_node(void)
{
  adjacency_node_t *node;

  node = (adjacency_node_t *)malloc(sizeof(adjacency_node_t));
  if(node == NULL)
  {
    fprintf(stderr,"allocate_adjacency_node: out of memory\n");
    exit(1);
  }
  return node;
}

static void free_adjacency_node(adjacency_node_t *node)
{
  free(node);
}

static hash_table_node_t *allocate_hash_table_node(void)
{
  hash_table_node_t *node;

  node = (hash_table_node_t *)malloc(sizeof(hash_table_node_t));
  if(node == NULL)
  {
    fprintf(stderr,"allocate_hash_table_node: out of memory\n");
    exit(1);
  }
  return node;
}

static void free_hash_table_node(hash_table_node_t *node)
{
  free(node);
}


//
// hash table stuff (mostly to be done)
//

unsigned int crc32(const char *str)
{
  static unsigned int table[256];
  unsigned int crc;

  if(table[1] == 0u) // do we need to initialize the table[] array?
  {
    unsigned int i,j;

    for(i = 0u;i < 256u;i++)
      for(table[i] = i,j = 0u;j < 8u;j++)
        if(table[i] & 1u)
          table[i] = (table[i] >> 1) ^ 0xAED00022u; // "magic" constant
        else
          table[i] >>= 1;
  }
  crc = 0xAED02022u; // initial value (chosen arbitrarily)
  while(*str != '\0')
    crc = (crc >> 8) ^ table[crc & 0xFFu] ^ ((unsigned int)*str++ << 24);
  return crc;
}

static hash_table_t *hash_table_create(void)
{
  hash_table_t *hash_table;
  unsigned int i;

  hash_table = (hash_table_t *)malloc(sizeof(hash_table_t));
  if(hash_table == NULL)
  {
    fprintf(stderr,"create_hash_table: out of memory\n");
    exit(1);
  }
  //
  // complete this
  //
  hash_table->hash_table_size = 50; // just a dummy value
  hash_table->number_of_entries = 0;
  hash_table->number_of_edges = 0;
  hash_table->heads = (hash_table_node_t **)malloc(sizeof(hash_table_node_t *) * hash_table->hash_table_size);

  if(hash_table->heads == NULL)
  {
    fprintf(stderr,"create_hash_table heads: out of memory\n");
    exit(1);
  }
  // set hash table elements to NULL pointer
  for(i = 0;i < hash_table->hash_table_size;i++)
    hash_table->heads[i] = NULL;

  return hash_table;
}

static void hash_table_grow(hash_table_t *hash_table)
{
  //
  // complete this
  //

  // criar um novo hash table com o dobro do tamanho
  // percorrer o hash table antigo e inserir os elementos no novo hash table
  // libertar o hash table antigo

  hash_table_node_t **old_heads, **new_heads;

  // create a new hash table with 2x more elements
  hash_table->hash_table_size = hash_table->hash_table_size * 2;

  new_heads = (hash_table_node_t **)malloc(sizeof(hash_table_node_t *) * hash_table->hash_table_size);

  // check if the reallocation was successful
  if(hash_table->heads == NULL)
  {
    fprintf(stderr,"create_hash_table heads: out of memory\n");
    exit(1);
  }

  // set hash table elements to NULL pointer
  for(unsigned int i = 0;i < hash_table->hash_table_size;i++)
    new_heads[i] = NULL;

  // save the old hash table
  old_heads = hash_table->heads;

  // set the new hash table
  hash_table->heads = new_heads;

  // percorrer o hash table antigo e inserir os elementos no novo hash table
  for(unsigned int i = 0;i < hash_table->hash_table_size/2;i++)
  {
    hash_table_node_t *node = old_heads[i];
    while(node != NULL)
    {
      hash_table_node_t *next = node->next;
      unsigned int j = crc32(node->word) % hash_table->hash_table_size;
      node->next = hash_table->heads[j]; // inserir no inicio da lista
      hash_table->heads[j] = node;
      node = next;
    }
  }
  // printf("-----------cheguei\n");
  // libertar o hash table antigo
  free(old_heads);
  // printf("---------------passei\n");
}

static void hash_table_free(hash_table_t *hash_table)
{
  //
  // complete this
  //
  for (unsigned int i=0; i<hash_table->hash_table_size; i++)  // loop through the hash table
  {
    hash_table_node_t *node = hash_table->heads[i]; // set node to the first element of the hash table
    while(node != NULL)                             // while the node has a next node
    {
      hash_table_node_t *temp = node; // set temp to the node
      node = node->next;              // set node to the next node                    // free the temp node
      adjacency_node_t *adj_node = temp->head; // set adj_node to the first element of the adjacency list
      while (adj_node != NULL)                 // while the adj_node has a next node
      {
        adjacency_node_t *temp_adj = adj_node; // set temp_adj to the adj_node
        adj_node = adj_node->next;             // set adj_node to the next node
        free_adjacency_node(temp_adj);                        // free the temp_adj node
      }
      free_hash_table_node(temp);                                   // free the temp node
    }
  }
  free(hash_table->heads);
  free(hash_table);

}

static hash_table_node_t *find_word(hash_table_t *hash_table,const char *word,int insert_if_not_found)
{
  hash_table_node_t *node;
  unsigned int i;

  i = crc32(word) % hash_table->hash_table_size;

  //
  // complete this
  //

  // percorrer a lista na posicao i para ver se la esta a palavra
  // se encontrar devolve o node onde a palavra esta
  // se nao encontrar e insert_if_not_found == 1 cria um node e insere na lista
  // se nao encontrar e insert_if_not_found == 0 devolve NULL

  node = hash_table->heads[i];
  // printf("Node inicialmente: %d\n",node);
  while(node != NULL)
  {
    // printf("Procurandooo....\n");
    if(strcmp(node->word,word) == 0){
      // printf("node word (found): %s\n", node->word);
      return node;
    }
    node = node->next;
  }

  if(insert_if_not_found == 1)
  {
    node = allocate_hash_table_node();
    node->previous = NULL;
    node->visited = 0;
    strcpy(node->word, word);
    node->head = NULL;
    node->next = hash_table->heads[i];
    hash_table->heads[i] = node;
    hash_table->number_of_entries++;
    node->representative = node;
    node->number_of_vertices = 1;
    node->number_of_edges = 0;

    
    FILE *fp = fopen("hash_table_loadfactor.txt", "a");

    if (fp == NULL)
    {
      printf("Error!");
      exit(1);
    }

    fprintf(fp, "%i,%.5f\n", hash_table->number_of_entries, (float)hash_table->number_of_entries/hash_table->hash_table_size);

    fclose(fp);

    // se number_of_entries > 0.75 * hash_table_size chamar hash_table_grow
    if(hash_table->number_of_entries >= 0.75 * hash_table->hash_table_size)
    {
      // printf("has grownnnn!! prev %i size\n", hash_table->hash_table_size);
      hash_table_grow(hash_table);
    }

    return node;
  }
  else
  {
    return NULL;
  }

  return node;
}


//
// add edges to the word ladder graph (mostly do be done)
//

static hash_table_node_t *find_representative(hash_table_node_t *node)
{
  hash_table_node_t *representative,*next_node,*temp;

  //
  // complete this
  //
  for(representative = node; representative->representative != representative; representative = representative->representative);

  for(next_node = node; next_node != representative; next_node = temp)
  {
    temp = next_node->representative;
    node->representative = representative;
  }

  return representative;
}

static void add_edge(hash_table_t *hash_table,hash_table_node_t *from,const char *word)
{
  hash_table_node_t *to,*from_representative,*to_representative;
  adjacency_node_t *edge_origin , *edge_to;

  to = find_word(hash_table,word,0);
  //
  // complete this
  //

  // add node from to the adjacency list of word
  // add word to the adjacency list of from
  // if from and word are not already in the same component
  // set the representative of the component of from to the representative of the component of word

  if(to == NULL || to == from)
  {
    return;
  }

  from_representative = find_representative(from);
  to_representative = find_representative(to);

  if(from_representative != to_representative)
  {
    if(from_representative->number_of_vertices < to_representative->number_of_vertices)
    {
      from_representative->representative = to_representative;
      to_representative->number_of_vertices += from_representative->number_of_vertices;
      to_representative->number_of_edges += from_representative->number_of_edges + 1;
      from_representative->number_of_edges = 0;
      from_representative->number_of_vertices = 0;
    }
    else
    {
      to_representative->representative = from_representative;
      from_representative->number_of_vertices += to_representative->number_of_vertices;
      from_representative->number_of_edges += to_representative->number_of_edges + 1;
      to_representative->number_of_edges = 0;
      to_representative->number_of_vertices = 0;
    }
  }else{
    from_representative->number_of_edges++;
  }
  
  // to_representative->number_of_edges++;
  // from_representative->number_of_edges++;

  edge_origin = allocate_adjacency_node();
  edge_origin->vertex = to;
  // adiciona no inicio da lista
  edge_origin->next = from->head;
  from->head = edge_origin;

  edge_to = allocate_adjacency_node();
  edge_to->vertex = from;
  // adiciona no inicio da lista
  edge_to->next = to->head;
  to->head = edge_to;
}


//
// generates a list of similar words and calls the function add_edge for each one (done)
//
// man utf8 for details on the uft8 encoding
//

static void break_utf8_string(const char *word,int *individual_characters)
{
  int byte0,byte1;

  while(*word != '\0')
  {
    byte0 = (int)(*(word++)) & 0xFF;
    if(byte0 < 0x80)
      *(individual_characters++) = byte0; // plain ASCII character
    else
    {
      byte1 = (int)(*(word++)) & 0xFF;
      if((byte0 & 0b11100000) != 0b11000000 || (byte1 & 0b11000000) != 0b10000000)
      {
        fprintf(stderr,"break_utf8_string: unexpected UFT-8 character\n");
        exit(1);
      }
      *(individual_characters++) = ((byte0 & 0b00011111) << 6) | (byte1 & 0b00111111); // utf8 -> unicode
    }
  }
  *individual_characters = 0; // mark the end!
}

static void make_utf8_string(const int *individual_characters,char word[_max_word_size_])
{
  int code;

  while(*individual_characters != 0)
  {
    code = *(individual_characters++);
    if(code < 0x80)
      *(word++) = (char)code;
    else if(code < (1 << 11))
    { // unicode -> utf8
      *(word++) = 0b11000000 | (code >> 6);
      *(word++) = 0b10000000 | (code & 0b00111111);
    }
    else
    {
      fprintf(stderr,"make_utf8_string: unexpected UFT-8 character\n");
      exit(1);
    }
  }
  *word = '\0';  // mark the end
}

static void similar_words(hash_table_t *hash_table,hash_table_node_t *from)
{
  static const int valid_characters[] =
  { // unicode!
    0x2D,                                                                       // -
    0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,           // A B C D E F G H I J K L M
    0x4E,0x4F,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,           // N O P Q R S T U V W X Y Z
    0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,           // a b c d e f g h i j k l m
    0x6E,0x6F,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,           // n o p q r s t u v w x y z
    0xC1,0xC2,0xC9,0xCD,0xD3,0xDA,                                              // Ã Ã‚ Ã‰ Ã Ã“ Ãš
    0xE0,0xE1,0xE2,0xE3,0xE7,0xE8,0xE9,0xEA,0xED,0xEE,0xF3,0xF4,0xF5,0xFA,0xFC, // Ã  Ã¡ Ã¢ Ã£ Ã§ Ã¨ Ã© Ãª Ã­ Ã® Ã³ Ã´ Ãµ Ãº Ã¼
    0
  };
  int i,j,k,individual_characters[_max_word_size_];
  char new_word[2 * _max_word_size_];

  break_utf8_string(from->word,individual_characters);
  for(i = 0;individual_characters[i] != 0;i++)
  {
    k = individual_characters[i];
    for(j = 0;valid_characters[j] != 0;j++)
    {
      individual_characters[i] = valid_characters[j];
      make_utf8_string(individual_characters,new_word);
      // avoid duplicate cases
      if(strcmp(new_word,from->word) > 0)
        add_edge(hash_table,from,new_word);
    }
    individual_characters[i] = k;
  }
}


//
// breadth-first search (to be done)
//
// returns the number of vertices visited; if the last one is goal, following the previous links gives the shortest path between goal and origin
//

static int breadh_first_search(int maximum_number_of_vertices,hash_table_node_t **list_of_vertices,hash_table_node_t *origin,hash_table_node_t *goal)
{
  //
  // complete this
  //

  // find representative of connected component
  //hash_table_node_t *representative = find_representative(origin);

  // list_of_vertices = (hash_table_node_t **) malloc(maximum_number_of_vertices * sizeof(hash_table_node_t*));
  hash_table_node_t *parent = NULL;


  if(origin == NULL)
  {
    fprintf(stderr,"breadh_first_search: origin is NULL\n");
    return -1;
  }

  int r = 0; int w = 1;
  list_of_vertices[0] = origin;
  origin->previous = NULL;
  origin->visited = 1;
  int stop = 0;

  while (r != w && stop == 0) {

    hash_table_node_t *current = list_of_vertices[r];
    
    parent = current;
    r++;

    for (adjacency_node_t *adj_node = current->head; adj_node != NULL; adj_node = adj_node->next) {
      if(adj_node->vertex->visited == 0) {

        list_of_vertices[w++] = adj_node->vertex;
        adj_node->vertex->visited = 1;
        adj_node->vertex->previous = parent;

        if (adj_node->vertex == goal){
          stop = 1;
          break;
        }
      }
    }

    // if (adj_node != NULL && adj_node->vertex == goal){
    //   break;
    // }
  }

  // passar todos os visited a zero
  for (int i = 0; i < w; i++) {
    list_of_vertices[i]->visited = 0;
  }


  // hash_table_node_t *node = goal;
  // int index = w-1; int count = 0;

  //ver quantos nodes tem o caminho mais curto
  // while(node != NULL){
  //   node = node->previous;
  //   count++;
  // }

  // hash_table_node_t *solArr[count];
  // node = goal;
  // solArr[count] = node;

  // // guardar todos os nodes do caminho mais curto
  // while(node != NULL){
  //   solArr[--count] = node;
  //   node = node->previous;
  // }

  // todos os nodes do caminho mais curto
  // printf("Solucao--------------------------------------------\n");

  // for (int i = 0; i < sizeof(solArr)/sizeof(solArr[0]); i++) {
  //   printf("%i - %s\n",i, solArr[i]->word);
  // }

  // todos os nodes visitados
  // printf("list_of_vertices--------------------------------------------\n");
  // for (int i = 0; i < w; i++) {
  //   printf("%i - %s\n",i, list_of_vertices[i]->word);
  // }

  return w;
}


//
// list all vertices belonging to a connected component (complete this)
//

static void list_connected_component(hash_table_t *hash_table,const char *word)
{
  //
  // complete this
  //

  hash_table_node_t *node = find_word(hash_table,word,0);


  if (node == NULL){

    printf("list_connected_component: Word inserida não existe\n");
    return;
  }


  int vertex_max = find_representative(node)->number_of_vertices;
  hash_table_node_t **list_of_vertices = (hash_table_node_t **) malloc(vertex_max * sizeof(hash_table_node_t*));
  
  int n_elements = breadh_first_search(vertex_max, list_of_vertices, node, NULL);

  // todos os nodes visitados
  printf("--------------------------------------------\n");
  printf("Lista de todos os nós pertencentes à componente de %s\n", word);
  for (int i = 0; i < n_elements; i++) {
    printf("%i - %s\n",i, list_of_vertices[i]->word);
  }
  printf("Tamanho da componente: %i\n", n_elements);
  printf("--------------------------------------------\n");

}


//
// compute the diameter of a connected component (optional)
//

static int largest_diameter, shortest_diameter;
static hash_table_node_t **largest_diameter_example, **shortest_diameter_example;

static int connected_component_diameter(hash_table_node_t *node)
{
  int diameter;

  //
  // complete this
  //
  // printf("connected_component_diameter: node %s\n", node->word);
  hash_table_node_t **path, **list_of_vertices, **list_of_vertices2;
  int vertex_max = find_representative(node)->number_of_vertices;

  list_of_vertices = (hash_table_node_t **) malloc(vertex_max * sizeof(hash_table_node_t*));
  list_of_vertices2 = (hash_table_node_t **) malloc(vertex_max * sizeof(hash_table_node_t*));
  // path = (hash_table_node_t **) malloc(vertex_max * sizeof(hash_table_node_t *));

  if(list_of_vertices == NULL || list_of_vertices2 == NULL)
  {
    fprintf(stderr,"connected_component_diameter: out of memory\n");
    exit(1);
  }

  int n = breadh_first_search(vertex_max, list_of_vertices, node, NULL);
  // print all list of vertices
  // for (int i = 0; i < vertex_max; i++) {
    // printf("list_of_vertices[%i] = %s\n", i, list_of_vertices[i]->word);
  // }

  diameter = 0;

  for (int i = 0; i < vertex_max; i++) {
    for (int j = i + 1; j < vertex_max; j++) {
      // printf("list[i] = %s\n", list_of_vertices[i]->word);
      breadh_first_search(vertex_max, list_of_vertices2, list_of_vertices[i], list_of_vertices[j]);

      // destino
      hash_table_node_t *temp = list_of_vertices[j];
      // printf("temp word = %s\n", temp->word);
      int count = 0;

      // ver quantos nodes tem o caminho mais curto
      while(temp != NULL){
        temp = temp->previous;
        count++;
      }

      if(count > diameter){
        diameter = count;
        // temp = list_of_vertices[j];



        // while(temp != NULL){
        //   path[--count] = temp;
        //   temp = temp->previous;
        // }

      }
    }
    // printf("cheguei3\n");
  }

  // diameter = diameter -1;

  // if(largest_diameter == NULL || diameter > largest_diameter) {
  //   largest_diameter = diameter;
  //   largest_diameter_example = path;
  // }
  // if(shortest_diameter == NULL || diameter < shortest_diameter) {
  //   shortest_diameter = diameter;
  //   shortest_diameter_example = path;
  // }
  // diameter_sum+=diameter;
  // number_of_connected_components++;

  // printf("--------------------------------------------\n");
  // printf("Diametro da componente: %i\n", diameter);

  // for (int i = 0; i < diameter; i++) {
  //   printf("%i - %s\n",i, path[i]->word);
  // }

  // printf("--------------------------------------------\n");


  free(list_of_vertices);
  free(list_of_vertices2);
  
  return diameter;
}


//
// find the shortest path from a given word to another given word (to be done)
//

static void path_finder(hash_table_t *hash_table,const char *from_word,const char *to_word)
{
  //
  // complete this
  //

  hash_table_node_t *from_node = find_word(hash_table,from_word,0);
  hash_table_node_t *to_node = find_word(hash_table,to_word,0);

  if (from_node == NULL || to_node == NULL){

    printf("path_finder: Word inserida não existe\n");
    return;
  }

  int vertex_max = find_representative(from_node)->number_of_vertices;

  hash_table_node_t **list_of_vertices = (hash_table_node_t **) malloc(vertex_max * sizeof(hash_table_node_t*));;

  int n_elements = breadh_first_search(vertex_max, list_of_vertices, from_node, to_node);

  hash_table_node_t *node = list_of_vertices[n_elements-1];
  // int index = n_elements-1; 
  int count = 0;

  // ver quantos nodes tem o caminho mais curto
  while(node != NULL){
    node = node->previous;
    count++;
  }

  hash_table_node_t *solArr[count];
  node = to_node;
  solArr[count] = node;

  // guardar todos os nodes do caminho mais curto
  while(node != NULL){
    solArr[--count] = node;
    node = node->previous;
  }

  //todos os nodes do caminho mais curto
  printf("--------------------------------------------\n");
  printf("Caminho mais curto de %s a %s\n", from_word, to_word);

  for (long unsigned int i = 0; i < sizeof(solArr)/sizeof(solArr[0]); i++) {
    printf("%li - %s\n",i, solArr[i]->word);
  }

  int temp = sizeof(solArr)/sizeof(solArr[0]);
  printf("Tamanho do caminho: %i\n", temp);
  printf("--------------------------------------------\n");

  if(temp > largest_diameter){
    largest_diameter = temp;
    largest_diameter_example = solArr;
  }
  free(list_of_vertices);
}


//
// some graph information (optional)
//

static void graph_info(hash_table_t *hash_table)
{
  //
  // complete this
  //

  //printf("entries = %i\n", hash_table->number_of_entries);

  hash_table_node_t **representatives = (hash_table_node_t *) malloc(hash_table->number_of_entries * sizeof(hash_table_node_t*));;
  hash_table_node_t *big=NULL, *small=NULL;


  if(representatives == NULL){
    printf("graph_info: Erro ao alocar memoria\n");
    return;
  }



  int index = 0, number_of_nodes = 0, number_of_edges = 0, diameter_sum = 0;

  for (int i=0; i<hash_table->hash_table_size; i++)  // loop through the hash table
  {

    hash_table_node_t *node = hash_table->heads[i]; // set node to the first element of the hash table

    while(node != NULL)                             // while the node has a next node
    {
      hash_table_node_t *temp = find_representative(node); // set temp to the node

      // check if temp in representatives
      int flag = 0;
  
      for (int j = 0; j < index; j++) {
        if (temp == representatives[j]) {
          flag = 1;
          break;
        }
      }

      if (flag == 0){
        representatives[index++] = temp;
        number_of_nodes += temp->number_of_vertices;
        hash_table->number_of_edges += temp->number_of_edges;
      }   

      node = node->next;              // set node to the next node   
    }

  }
  printf("aqui\n");

  for (int i = 0; i < index; i++) {
    hash_table_node_t *temp = representatives[i];
    printf("oi\n");
    int diameter = connected_component_diameter(temp) - 1;   // run connected_component_diameter
    printf(" - diam: %i || %s\n", diameter, temp->word);
    diameter_sum += diameter;

    if(largest_diameter == NULL || diameter > largest_diameter) {
      largest_diameter = diameter;
      big = temp;
      // largest_diameter_example = path;
    }
    if(shortest_diameter == NULL || diameter < shortest_diameter) {
      shortest_diameter = diameter;
      // shortest_diameter_example = path;
      small = temp;
    }

  }

  shortest_diameter_example = (hash_table_node_t **) malloc(small->number_of_vertices * sizeof(hash_table_node_t*));;
  breadh_first_search(small->number_of_vertices, shortest_diameter_example, small, NULL);

  largest_diameter_example = (hash_table_node_t **) malloc(big->number_of_vertices * sizeof(hash_table_node_t*));;
  breadh_first_search(small->number_of_vertices, largest_diameter_example, big, NULL);

  printf("Number of nodes: %i\n", number_of_nodes);
  printf("Number of edges: %i\n", hash_table->number_of_edges);

  printf("--------------------------------------------\n");
  // printf("teste: %i\n", diameter_sum);
  printf("Numero de componentes conexas: %i\n", index);
  printf("Diametro medio: %.4f\n", (float)diameter_sum/(float)index);
  printf("Diametro maximo: %i\n", largest_diameter);
  printf("Diametro minimo: %i\n", shortest_diameter);
  printf("--------------------------------------------\n");

  printf("-----------Largest Path--------------\n");
  for( int i = 0; i < big->number_of_vertices; i++ ){
    printf("LD %i - %s\n", i, largest_diameter_example[i]->word);
  }
  printf("-----------Shortest Path--------------\n");
  for( int i = 0; i < small->number_of_vertices; i++ ){
    printf("SD %i - %s\n", i, shortest_diameter_example[i]->word);
  }

  free(representatives);
  free(largest_diameter_example);
  free(shortest_diameter_example);
}



static void hash_table_info(hash_table_t *hash_table)
{
  printf("Hash table size : %i\n", hash_table->hash_table_size);
  printf("Hash table number of entries: %i\n", hash_table->number_of_entries);
  // printf("Has table number of edges: %i\n", hash_table->number_of_edges); // que?????????
  printf("Hash table load factor %.3f\n", (float)hash_table->number_of_entries/(float)hash_table->hash_table_size);

  int max = 0, min = -1, sum = 0, number_of_lists = 0;

  FILE *fp = fopen("hash_table_distribution.txt", "w");

    if (fp == NULL)
    {
      printf("Error!");
      exit(1);
    }

  
  for(unsigned int i = 0; i < hash_table->hash_table_size; i++){
    hash_table_node_t *node = hash_table->heads[i];
    int count = 0;

    if(node == NULL){
      continue;
    }

    number_of_lists++;

    while(node != NULL){
      count++;
      node = node->next;
    }
    if(count > max){
      max = count;
    }
    if(min == -1 || count < min){
      min = count;
    }
    sum += count;
    fprintf(fp, "%d,%d\n", i, count);

  }
  fclose(fp);

  printf("Hash table longest chain size: %i\n", max);
  printf("Hash table shortest chain size: %i\n", min);
  printf("Hash table average chain size: %.3f\n", (float)sum/(float)number_of_lists);

}



//
// main program
//

int main(int argc,char **argv)
{
  char word[100],from[100],to[100];
  hash_table_t *hash_table;
  hash_table_node_t *node;
  unsigned int i;
  int command;
  FILE *fp;

  // // initialize hash table
  hash_table = hash_table_create();

  // printf("%s\n", find_word(hash_table,"banana",1));
  // printf("%s\n", find_word(hash_table,"banana",1));
  // printf("%s\n", find_word(hash_table,"ananas",1));
  // printf("%s\n", find_word(hash_table,"coisa",1));
  // printf("%s\n", find_word(hash_table,"pera",1));

  // read words
  fp = fopen((argc < 2) ? "wordlist-four-letters.txt" : argv[1],"rb");
  if(fp == NULL)
  {
    fprintf(stderr,"main: unable to open the words file\n");
    exit(1);
  }
  while(fscanf(fp,"%99s",word) == 1) {
    (void)find_word(hash_table,word,1);
  }
  fclose(fp);
  // find all similar words
  for(i = 0u;i < hash_table->hash_table_size;i++) {
    for(node = hash_table->heads[i];node != NULL;node = node->next)
      similar_words(hash_table,node);
  }

  // connected_component_diameter(find_word(hash_table,"tudo",0));
  graph_info(hash_table);


  // hash_table_info(hash_table);


  // ask what to do
  for(;;)
  {
    fprintf(stderr,"Your wish is my command:\n");
    fprintf(stderr,"  1 WORD       (list the connected component WORD belongs to)\n");
    fprintf(stderr,"  2 FROM TO    (list the shortest path from FROM to TO)\n");
    fprintf(stderr,"  3            (terminate)\n");
    fprintf(stderr,"> ");
    if(scanf("%99s",word) != 1)
      break;
    command = atoi(word);
    if(command == 1)
    {
      if(scanf("%99s",word) != 1)
        break;
      list_connected_component(hash_table,word);
    }
    else if(command == 2)
    {
      if(scanf("%99s",from) != 1)
        break;
      if(scanf("%99s",to) != 1)
        break;
      path_finder(hash_table,from,to);
    }
    else if(command == 3)
      break;
  }



  //
  // TESTAR ADJACENCY LIST ------------------------------------------------------------
  //


  // print adjacency list
  //     printf("Adjacency list of %s: ", hash_table->heads[42]->word);
  //     adjacency_node_t *temp = hash_table->heads[42]->head;
  //     while(temp != NULL)
  //     {
  //       printf("%s ", temp->vertex->word);
  //       temp = temp->next;
  //     }
  //     printf("\n");

  // //print elements in head of hash table
  // for(i = 0u;i < hash_table->hash_table_size;i++) {
  //   for(node = hash_table->heads[i];node != NULL;node = node->next){
  //     printf("%d-> %s\n", i,node->word);
  //   }
  // }

  // testar breadth first search
  // printf("banana\n");


  
  // print all representatives of the graph
  // for(i = 0u;i < hash_table->hash_table_size;i++) {
  //   for(node = hash_table->heads[i];node != NULL;node = node->next){
  //     printf("%s -> %s\n", node->word ,node->representative->word);
  //   }
  // }


  // list_connected_component(hash_table,"tudo");
  // path_finder(hash_table,"nada","tudo");

  // clean up
  hash_table_free(hash_table);
  return 0;
}