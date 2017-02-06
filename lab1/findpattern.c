#include "findpattern.h"

unsigned int findpattern(unsigned char *pattern, unsigned int patlength, struct patmatch *locations, unsigned int loclength) {
  
  int j, pagesize, count = 0;
  long long i, cap = 4294967296; // = 2^32
  unsigned int num_increments;
  char * temp;
  int v;

  // might need to initialize the *locations array to 0 using the loclength

  for (j=0; j<=loclength; j++) {

    locations[j].location = 0;
    
    locations[j].mode = MEM_RO;
    // with this now done, we can safely modify the array *locations by inserting structs into the correct locations

  }

  for (i=0; i<cap; i+=1) 
    { 
     
    j = sigsetjmp(env, 1);
    
    if (i>=cap)
	{
	  break;
	}
    // for reasons beyond my comprehension, end conditions for for and while loops cease to work so I need to hardcode them in

    // put sigsetjmp() here so that if we try to read unaccessible memory, it jumps back to here

    if (j == 1) 
      {

      pagesize = getpagesize(); 
      
      //grab the page size and increment i by that amount so we can skip the unreadable memory
      i += pagesize;

      }

    printf("i = %lli\n", i);

    temp = (char *) i;
    // we want to keep the original value of i since that would be the starting location
    
    v = 0;
    num_increments = 0;
    
    while (*(temp+v) == pattern[v])
      // assuming I know what I'm doing, this should simply go through and increment num_increments every time the *temp and *pattern match (the specific byte we're looking at)
      {
    	v += 1;
	
	num_increments += 1;
	
	if (num_increments >= patlength) 
	  {
	    printf("num_increments = %i\n", num_increments);
	    printf("pattern[v] = %c\n", pattern[v]);
	    printf("v = %i\n", v);
	    break;
	  }
	
      } 
    

    if (num_increments == patlength)
      // hyopthetically, this should ensure that we found a match exactly equal to the pattern (not shorter or longer)
      // since findpattern() might find the first portion of the pattern, but not all of it
      {

    	struct patmatch match; 
	// create a instance of the patmatch struct to store the current match in

	match.location = (unsigned int) i;
     
	// set the location of the match to the current address
	
	// I've started by assuming that the memory is RO
	// try to write- if it succeeds, change to RW
	// otherwise, leave it as is

	j = sigsetjmp(env, 1);
	// if an error occurs when attempting to write to memory, then the memory is RO and the mode shouldn't change (since we initialized it to RO)
	// we want to change the mode ONLY if the write succeeds (that is, j is equal to 0. It is equal to 1 only when an error is rasied)

	if (j==0)
	  {
	    
	    int val;
	    char * write_test = (char *) i;
	    *write_test = i;
	    // write succeeded

	    match.mode = MEM_RW;

	  }
	
	if (count <= loclength)
	  // if loclength is less than the number of locations where the pattern was found, then the locations array contains the first loclength entries
	  {

	    locations[count] = match;  
	    // put the match in the appropriate spot in the locations array

	  }

	count += 1; 
	// can only increment after adding the match to the list since otherwise we would be off by one

      }
   
    }
  
  return(count);
  // this is what gets returned regardless of what happens
}

void sig_segv_handler(int sig) 
{
  // a segfault only happens when we're trying to read from memory that can't be read/write to RO memory
  
  siglongjmp(env, 1);   
  // longjmp() back to right before the error occurred

 }




int main() {

  struct sigaction my_handler, old_handler;
  my_handler.sa_handler = sig_segv_handler;
  sigemptyset(&my_handler.sa_mask);
  my_handler.sa_flags = 0;
  sigaction(SIGSEGV, &my_handler, 0);

  char pattern[] = "a";
  unsigned int patlength = 1;
  struct patmatch locations[50];
  unsigned int loclength = 50;
  unsigned int p;
  int i;
  
  p = findpattern(pattern, patlength, locations, loclength);

  printf("count = %d\n", p);
  for (i=0;i<=loclength;i++)
    {
      printf("memory type = %i\n", locations[i].mode);
    }

  //  sigaction(SIGSEGV, &old_handler, NULL);
  
  return 0;

}
