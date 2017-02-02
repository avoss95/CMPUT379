#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <math.h>    
#include <unistd.h>
#include <signal.h>

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
  long i, max = pow(2,32);
  int num_increments = 0;
  char * temp;
  int v;

  // might need to initialize the *locations array to 0 using the loclength

  for (j=0; j<loclength; j++) {
    
    locations[j].location = 0;
    // no idea if this is proper syntax or not- I really need to brush up on my C pointer knowledge
    // maybe this is why I didn't do too well on that section of the 201 final?
    locations[j].mode = MEM_RO;
    
    // with this now done, we can safely modify the array *locations by inserting structs into the correct locations
    
  }

  for (i=0; i<max; i+=4) {

    j = sigsetjmp(env, 1);
    // printf("j = %d\n", j);
    // put setjmp() here so that if we try to read unaccessible memory, it jumps back to here

    if (j == 1) {


      //i += 1;
      //do I even need to do this?  
      pagesize = getpagesize(); 
      
      // printf("pagesize = %d\n", pagesize);

      //grab the page size and increment i by that amount so we can skip the unreadable memory
      i += pagesize;



    }

    //    printf("i = %d\n", i);

    temp = (char *) i;

    //    printf("dereferenced i\n");

    v = 0;

    //printf("temp = %c\n", *temp);

    while (*(temp+v) == pattern[v])
      // assuming I know what I'm doing, this should simply go through and increment num_increments every time the *i and *pattern match (the specific byte we're looking at)
      {
	//	printf("Made it into the while loop\n");
	v += 1;
	*pattern += 1;
	num_increments += 1;
      } 
      

    if (num_increments == patlength)
      // hyopthetically, this should ensure that we found a match exactly equal to the pattern (not shorter or longer)
      {

	struct patmatch match; 
	// create a instance of the patmatch struct to store the current match in

	match.location = i; 
	// set the location of the match to the current address

	locations[count] = match;  
	// I think ? this is (assuming this is the first match found) putting the match in the first spot in the locations array

	count += 1; 
	// can only increment after adding the match to the list since otherwise we would be off by one
	
	// I've started by assuming that the memory is RO
	// try to write- if it succeeds, change to RW
	// otherwise, leave it as is

	//j = sigsetjmp(env, 1);
	// so if we get a segfault, it will jump back here 

	// crap- if I do the write here, and it segfaults, we get stuck in an infinite loop....

	if (j == 1) {

	  // if j == 1, then a segfault happened
	  // segfault means memory is RO, otherwise it is RW

	  
	}

	//match.mode = MEM_RW; 
	// I forget how to determine if the memory is RW or RO, so until I figure that out this placeholder will be here

      }

  }

  return(count);

}

void sig_segv_handler(int sig) 
{
  //  printf("This is the signal handler for SIGSEGV\n");
  
  //  (void) signal(SIGSEGV, SIG_IGN);

  // a segfault only happens when we're trying to read from memory that can't be read
  

  siglongjmp(env, 1);   
  //I think this is the right spot for longjmp()- the error has been handled, so we want to go back 

 }




int main() {

  (void) signal(SIGSEGV, sig_segv_handler);

  // call findpattern() at some point, with all the required arguments
  // pretty sure that's all main() will do- call findpattern().... I think

  char * pattern = "a";
  unsigned int patlength = 1;
  struct patmatch *locations;
  unsigned int loclength = 5;

  findpattern(pattern, patlength, locations, loclength);

  return 0;

}
