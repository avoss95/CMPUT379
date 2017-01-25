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
}

unsigned int findpattern (unsigned char *pattern, unsigned int patlength, struct patmatch *locations, unsigned int loclength);


int main() {

  (void) signal(SIGSEGV, sig_segv_handler);

  // call findpattern() at some point, with all the required arguments


}



unsigned int findpattern(unsigned char *pattern, unsigned int patlength, struct patmatch *locations, unsigned int loclength) {
  
  int j, i, count = 0;
  int max = pow(2,32);

  // might need to initialize the *locations array to 0 using the loclength

  for (j=0; j<loclength; j++) {
    
    locations[j].location = 0;
    // no idea if this is proper syntax or not- I really need to brush up on my C pointer knowledge
    // maybe this is why I didn't do too well on this section on the 201 final?
    locations[j].mode = NULL;
    
    // with this now done, we can safely modify the array *locations by inserting structs into the correct locations
    
  }

  for (i=0; i<max; i++) {
    
    if (strncmp(*i, *pattern, patlength) == 0)  //  we're dereferencing i -> in this case it should be whatever data is stored at that address.... except it might not be a string.... how to compare?
      {
	// if we get to this point this means that the pattern has been found
	count += 1;

	struct patmatch match; // create a instance of the patmatch struct to store the current match in
	match.location = i; // set the location of the match to the current address -> might change depending on how the meeting on Friday goes

	match.mode = NULL; // I forget how to determine if the memory is RW or RO, so until I figure that out this placeholder will be here

	// so I still need to add this struct to the *locations array (an array of structs of type patmatch)
	// don't remember how to do that but it's on the list of things to do

	locations[count] = match;  // I think ? this is (assuming this is the first match found) putting the match in the first spot in the locations array

      }

  }

  return(count);

}

void sig_segv_handler(int sig) 
{
  printf("This is the signal handler for SIGSEGV\n");
  
  (void) signal(SIGSEGV, SIG_IGN);

  // a segfault only happens when we're trying to read from memory that can't be read
  // we'll somehow need to use those fun setjmp() and longjmp() functions to deal with that
  //

 }
