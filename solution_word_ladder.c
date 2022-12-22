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
  for(int i = 0;i < hash_table->hash_table_size;i++)
    new_heads[i] = NULL;

  // save the old hash table
  old_heads = hash_table->heads;

  // set the new hash table
  hash_table->heads = new_heads;

  // percorrer o hash table antigo e inserir os elementos no novo hash table
  for(int i = 0;i < hash_table->hash_table_size/2;i++)
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
  for (int i=0; i<hash_table->hash_table_size; i++)  // loop through the hash table
  {
    hash_table_node_t *node = hash_table->heads[i]; // set node to the first element of the hash table
    while(node != NULL)                             // while the node has a next node
    {
      hash_table_node_t *temp = node;               // set temp to the node
      node = node->next;                            // set node to the next node
      free(temp);                                   // free the temp node
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
    strcpy(node->word, word);
    node->head = NULL;
    node->next = hash_table->heads[i];
    hash_table->heads[i] = node;
    hash_table->number_of_entries++;
    node->representative = node;
    node->number_of_vertices = 1;
    node->number_of_edges = 0;


    // printf("node word (not found): %s\n", node->word);
    // printf("Hash table size: %d\n", hash_table->hash_table_size);
    // printf("Number of entries: %d\n", hash_table->number_of_entries);

    // se number_of_entries > 0.75 * hash_table_size chamar hash_table_grow
    if(hash_table->number_of_entries >= 0.75 * hash_table->hash_table_size)
    {
      printf("has grownnnn!!\n");
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
      to_representative->number_of_edges += from_representative->number_of_edges;
    }
    else
    {
      to_representative->representative = from_representative;
      from_representative->number_of_vertices += to_representative->number_of_vertices;
      from_representative->number_of_edges += to_representative->number_of_edges;
    }
  }
  
  to_representative->number_of_edges++;
  from_representative->number_of_edges++;


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
  
  // create array of nodes
  // printf("O3333I\n");
  hash_table_node_t *nodesArray[maximum_number_of_vertices];
  hash_table_node_t *parent = NULL;

  int r = 0; int w = 1;
  nodesArray[0] = origin;
  origin->previous = NULL;
  origin->visited = 1;

  while (r != w) {

    hash_table_node_t *current = nodesArray[r];
    parent = current;
    r++;
    adjacency_node_t *adj_node = current->head;

    for (int i = 0; i < maximum_number_of_vertices && adj_node != NULL; i++) {
      if(adj_node->vertex->visited == 0) {

        nodesArray[w++] = adj_node->vertex;
        adj_node->vertex->visited = 1;
        adj_node->vertex->previous = parent;

        if (adj_node->vertex == goal){
          break;
        }
      }
      adj_node = adj_node->next;
    }

    if (adj_node != NULL && adj_node->vertex == goal){
      break;
    }

  }

  // create solution array of length w

  hash_table_node_t *node = goal;
  int index = w-1; int count = 1;

  while(node->previous != NULL){
    count++;
    node = node->previous;
  }

  hash_table_node_t *solArr[count];
  node = goal;
  solArr[count] = node;


  while(node != NULL){
    solArr[--count] = node;
    node = node->previous;
  }

  printf("Solucao--------------------------------------------\n");

  for (int i = 0; i < sizeof(solArr)/sizeof(solArr[0]); i++) {
    printf("%i - %s\n",i, solArr[i]->word);
  }

  printf("nodesArray--------------------------------------------\n");
  for (int i = 0; i < w; i++) {
    printf("%i - %s\n",i, nodesArray[i]->word);
  }
  
  printf("R: %i\n", w);

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
}


//
// compute the diameter of a connected component (optional)
//

static int largest_diameter;
static hash_table_node_t **largest_diameter_example;

static int connected_component_diameter(hash_table_node_t *node)
{
  int diameter;

  //
  // complete this
  //
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
}


//
// some graph information (optional)
//

static void graph_info(hash_table_t *hash_table)
{
  //
  // complete this
  //
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
  fp = fopen((argc < 2) ? "wordlist-five-letters.txt" : argv[1],"rb");
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
  graph_info(hash_table);
  // printf("%s\n", find_word(hash_table,"Comba",1));
  // printf("%s\n", find_word(hash_table,"Congo",1));
  // ask what to do
  // for(;;)
  // {
  //   fprintf(stderr,"Your wish is my command:\n");
  //   fprintf(stderr,"  1 WORD       (list the connected component WORD belongs to)\n");
  //   fprintf(stderr,"  2 FROM TO    (list the shortest path from FROM to TO)\n");
  //   fprintf(stderr,"  3            (terminate)\n");
  //   fprintf(stderr,"> ");
  //   if(scanf("%99s",word) != 1)
  //     break;
  //   command = atoi(word);
  //   if(command == 1)
  //   {
  //     if(scanf("%99s",word) != 1)
  //       break;
  //     list_connected_component(hash_table,word);
  //   }
  //   else if(command == 2)
  //   {
  //     if(scanf("%99s",from) != 1)
  //       break;
  //     if(scanf("%99s",to) != 1)
  //       break;
  //     path_finder(hash_table,from,to);
  //   }
  //   else if(command == 3)
  //     break;
  // }



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
  for(i = 0u;i < hash_table->hash_table_size;i++) {
    for(node = hash_table->heads[i];node != NULL;node = node->next){
      printf("%d-> %s\n", i,node->word);
    }
  }

  // testar breadth first search
  int x = breadh_first_search(find_representative(find_word(hash_table,"tudo",0))->number_of_vertices, hash_table->heads,find_word(hash_table,"tudo",0),find_word(hash_table,"nuda",0));

  // print all representatives of the graph
  // for(i = 0u;i < hash_table->hash_table_size;i++) {
  //   for(node = hash_table->heads[i];node != NULL;node = node->next){
  //     printf("%s -> %s\n", node->word ,node->representative->word);
  //   }
  // }



  // clean up
  hash_table_free(hash_table);
  return 0;
}