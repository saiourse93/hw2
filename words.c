#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>

//pthread_t thread[4];

pthread_mutex_t mtex;

typedef struct dict {  //This is a structure ,dict, and it's elements word,count,and next;
  char *word;
  int count;
  struct dict *next;
} dict_t;    //dict_t is an instance of the structure dict.It means that dict_t can access anything inside dict
dict_t *d;
char *make_word( char *word ) {	 //Takes in a string as input
	//malloc allocates space in memory. strcpy=copy string in string
  return strcpy( malloc( strlen( word ) +1), word );	//We add one becuase a string always has the null terminating character "\0"
}

dict_t *make_dict(char *word) {			//This function takes in a string as input and returns a dictionary.
  dict_t *nd = (dict_t *) malloc( sizeof(dict_t) );		// Creates a dictionary
  nd->word = make_word( word );				//create a word in dictionary
  nd->count = 1;					//set the counter to one
  nd->next = NULL;					//set the pointer next to NULL
  return nd;				//returns a dictionary, nd
}

dict_t *insert_word( dict_t *d, char *word ) {
  
  //   Insert word into dict or increment count if already there
  //   return pointer to the updated dict
  
  dict_t *nd;
  dict_t *pd = NULL;		// prior to insertion point 
  dict_t *di = d;		// following insertion point

  // Search down list to find if present or point of insertion
  while(di && ( strcmp(word, di->word ) >= 0) ) { //strcmp() returns an integer( it compares word to di->word
    if( strcmp( word, di->word ) == 0 ) { //Checks if the word is in the dictionary ==0 if its there
      di->count++; //it increments the count of the found word
      return d;	 // return the pointer of the updated dictionary
    }else{ //if thw words not equal
    pd = di;	//if the word is not the dictionary (bcoz pd=NULL), create a new word	
    di = di->next; // pointer moved to the next word
    }
  } 

//if woed not there
  nd = make_dict(word);	// not found, make entry 

//countyer new word is now 1 and its word is word and its next is null. 
  nd->next = di; //combine new dict nd to old dict di, to make one dict consististing of the added word 
  if (pd) { //pd is the last pointer after the entire process has excuted
    pd->next = nd;
    return d;			// insert beond head 
  }
  return nd; // new dict
}

void print_dict(dict_t *d) {
  while (d) { //d is not empty ie there are words in.
    printf("[%d] %s\n", d->count, d->word);
    d = d->next;//moves to next word in dict
  }
}

int
get_word( char *buf, int n, FILE *infile) {
  int inword = 0;
  int c;  
  while( (c = fgetc(infile)) != EOF ) {
    if (inword && !isalpha(c)) {
      buf[inword] = '\0';	// terminate the word string
      return 1;
    } 
    if (isalpha(c)) {
      buf[inword++] = c;
    }
  }
  return 0;			// no more words
}

#define MAXWORD 1024
void *
words(void * input) {
FILE *infile = (FILE *) input;
 // dict_t *wd = NULL;
  char wordbuf[MAXWORD];
  while( get_word( wordbuf, MAXWORD, infile ) ) {
pthread_mutex_lock(&mtex);
    d = insert_word(d, wordbuf); // add to dict
pthread_mutex_unlock(&mtex);
  }
  //return wd;
}

int
main( int argc, char *argv[] ) {
if(pthread_mutex_init(&mtex,NULL)!=0)
{
printf("Failed to create mutex lock");
return 0;
}
  d = NULL;
  FILE *infile = stdin;
  if (argc >= 2) {
    infile = fopen (argv[1],"r");
  }
  if( !infile ) {
    printf("Unable to open %s\n",argv[1]);
    exit( EXIT_FAILURE );
  }
 //
  pthread_t thread[4];			//This is a thread identifier;

for(int i=0; i<4; i++)
{
  pthread_create(&thread[i],NULL,&words,infile);
  pthread_join(thread[i],NULL);
//  pthread_exit(&thread);

}
 // pthread_create(&thread,NULL,print_dict,d);
 // pthread_join(&thread,NULL);
  print_dict( d ); 

pthread_mutex_destroy(&mtex);
  fclose( infile );
}

