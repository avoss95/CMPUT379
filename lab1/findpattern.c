#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <math.h>    
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <inttypes.h>

#define MEM_RW 0
#define MEM_RO 1

struct patmatch {
  unsigned int  location;
  unsigned char mode; /* MEM_RW, or MEM_RO */
};

jmp_buf env;

// global variable so that env can be used in my signal handler as well as my loop

unsigned int findpattern(unsigned char *pattern, unsigned int patlength, struct patmatch *locations, unsigned int loclength) {
  
  int j, pagesize, count = 0;
  long long i, cap = 4294967296;
  int num_increments = 0;
  char * temp;
  int v;

  // might need to initialize the *locations array to 0 using the loclength

  for (j=0; j<loclength; j++) {

    locations[j].location = 0;
    
    locations[j].mode = MEM_RO;
    // with this now done, we can safely modify the array *locations by inserting structs into the correct locations

  }

  for (i=0; i<cap; i+=1) 
    {
    
  // i = 0;
  // do {
    
     
    j = sigsetjmp(env, 1);
    

    /*if (i<0)
    	break;
	}*/

    // put sigsetjmp() here so that if we try to read unaccessible memory, it jumps back to here

    if (j == 1) 
      {

      pagesize = getpagesize(); 
      
      //grab the page size and increment i by that amount so we can skip the unreadable memory
      i += pagesize;

      }
    
    
    
    printf("i = %lli\n", i);

    temp = (char *) i;
    // I can't do this because a double can't be turned into a char *; however, an int can
    // but an int is only 16 bits and I need a double, which is 32 bits

    v = 0;

    while (*(temp+v) == pattern[v])
      // assuming I know what I'm doing, this should simply go through and increment num_increments every time the *i and *pattern match (the specific byte we're looking at)
      {

	printf("pattern[v] = %c\n", pattern[v]);
	printf("temp = %c\n", *(temp+v));

    	v += 1;

	num_increments += 1;

      } 
      

    if (num_increments == patlength)
      // hyopthetically, this should ensure that we found a match exactly equal to the pattern (not shorter or longer)
      {

	printf("num_increments = %i\n", num_increments);
	sleep(2);

	//	printf("v = %i\n", v);

    	struct patmatch match; 
	// create a instance of the patmatch struct to store the current match in

	match.location = i; 
	// set the location of the match to the current address

	// I've started by assuming that the memory is RO
	// try to write- if it succeeds, change to RW
	// otherwise, leave it as is

	//	j = sigsetjmp(env, 1);
	// so if we get a segfault, it will jump back here 

	// crap- if I do the write here, and it segfaults, we get stuck in an infinite loop....

	if (j == 1) 
	  {

	  // if j == 1, then a segfault happened
	  // segfault means memory is RO, otherwise it is RW

	  
	  }

	//match.mode = MEM_RW; 


	locations[count] = match;  
	// I think ? this is (assuming this is the first match found) putting the match in the first spot in the locations array

	count += 1; 
	// can only increment after adding the match to the list since otherwise we would be off by one

	//i += v;
	// need to move on past the located pattern
	
	printf("Found a match.\n");

	sleep(2);

      }
   
    }
  
  return(count);

}

void sig_segv_handler(int sig) 
{
  //printf("This is the signal handler for SIGSEGV\n");
  
  // a segfault only happens when we're trying to read from memory that can't be read
  
  siglongjmp(env, 1);   
  // longjmp() back to right before the error occurred

 }




int main() {

  struct sigaction act;
  act.sa_handler = sig_segv_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  sigaction(SIGSEGV, &act, 0);

  // call findpattern() at some point, with all the required arguments
  // pretty sure that's all main() will do- call findpattern().... I think

  char pattern[] = "abcdefgfcdeba";
  unsigned int patlength = 13;
  struct patmatch locations[5];
  unsigned int loclength = 5;

  findpattern(pattern, patlength, locations, loclength);

  /*long  long i, max = 4294967296;
  for (i=0;i<max;i+=1024) 
    {
      printf("i = %lli\n", i);
      } */
  
  return 0;

}
