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
  
  int i;
  int max = pow(2,32);

  for (i=0; i<max; i++) {
    
    if (strncmp(*i, *pattern, patlength) == 0)  // I feel like strncmp won't work because i is an int, not a string.  possible to convert?
      {
	// if we get to this point this means that the pattern has been found

	// need to return in locations array the location where the pattern was found
	// need to do all the required stuff below -> create a function for that?
      }

  }

}

void sig_segv_handler(int sig) 
{
  printf("This is the signal handler for SIGSEGV\n");
  
  (void) signal(SIGSEGV, SIG_IGN);


 }
