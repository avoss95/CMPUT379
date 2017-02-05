#include "findpattern.h"



unsigned int findpattern(unsigned char *pattern, unsigned int patlength, struct patmatch *locations, unsigned int loclength) {
  
  int j, pagesize, count = 0;
  long long i, cap = 4294967296;
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
    
    // printf("j = %i\n", j);
    //sleep(30);
   
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
    
    
    
    //printf("i = %lli\n", i);

    temp = (char *) i;
    // I can't do this because a double can't be turned into a char *; however, an int can
    // but an int is only 16 bits and I need a double, which is 32 bits

    v = 0;
    num_increments = 0;
    
    while (*(temp+v) == pattern[v])
      // assuming I know what I'm doing, this should simply go through and increment num_increments every time the *temp and *pattern match (the specific byte we're looking at)
      {
    	v += 1;
	
	num_increments += 1;
	
	if (num_increments >= patlength) 
	  {
	    break;
	  }
	
      } 
    

    if (num_increments == patlength)
      // hyopthetically, this should ensure that we found a match exactly equal to the pattern (not shorter or longer)
      {

	//	printf("v = %i\n", v);

    	struct patmatch match; 
	// create a instance of the patmatch struct to store the current match in

	match.location = (unsigned int) i;
     
	// set the location of the match to the current address
	
	// I've started by assuming that the memory is RO
	// try to write- if it succeeds, change to RW
	// otherwise, leave it as is

	j = sigsetjmp(env, 1);

	if (j==0)
	  {
	    
	    int val;
	    char * write_test = (char *) i;
	    *write_test = i;
	    // write succeeded

	    match.mode = MEM_RW;

	  }
	


	locations[count] = match;  
	// I think ? this is (assuming this is the first match found) putting the match in the first spot in the locations array

	count += 1; 
	// can only increment after adding the match to the list since otherwise we would be off by one

      }
   
    }
  
  return(count);
  
}

void sig_segv_handler(int sig) 
{
  // printf("This is the signal handler for SIGSEGV\n");
  
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

  char pattern[] = "abcdefgfcdeba";
  unsigned int patlength = 13;
  struct patmatch locations[5];
  unsigned int loclength = 5;
  unsigned int p;
  int i;
  
  p = findpattern(pattern, patlength, locations, loclength);
  
  printf("count = %i\n", p);
  
  return 0;

}
