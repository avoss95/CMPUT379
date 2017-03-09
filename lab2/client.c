#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <math.h>
#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>

#define	 MY_PORT  2222

void send_query(int s, FILE * keyfile);
char *base64decode (const void *b64_decode_this, int decode_this_many_bytes);
int decrypt(unsigned char *ciphertext, unsigned char *plaintext, FILE * keyfile);
char *base64encode (const void *b64_encode_this, int encode_this_many_bytes);
int encrypt(unsigned char * intext, unsigned char * outtext, FILE * keyfile);
void plaintext_entry(int s);
void encrypted_entry(int s, FILE * keyfile);
void clean_entry(int s);

int main(int argc, char * argv[])
{
  // argv[1] = hostname, argv[2] = portnumber, argv[3] (if it exists) = keyfile

  FILE * keyfile;
  
  if (argc = 4)
    {
     keyfile = fopen(argv[3], "r+");
    }
  else
    {
      keyfile = NULL;
    }
  
  int i, s, number, n;

  unsigned char greeting[10000]; //, message[10000], plaintext[10000];

  struct sockaddr_in server;
  
  struct hostent *host;
  
  host = gethostbyname ("localhost");
  
  if (host == NULL) {
    perror ("Client: cannot get host description");
    exit (1);
  } 
  
  s = socket (AF_INET, SOCK_STREAM, 0);
  
  if (s < 0) {
    perror ("Client: cannot open socket");
    exit (1);
  }
  
  bzero (&server, sizeof (server));
  bcopy (host->h_addr, & (server.sin_addr), host->h_length);
  server.sin_family = AF_INET;//= host->h_addrtype;
  server.sin_addr.s_addr = inet_addr(argv[1]); //run server on ug13, 129.128.29.43
  server.sin_port = htons (atoi(argv[2]));
  
  if (connect (s, (struct sockaddr*) & server, sizeof (server))) {
    perror ("Client: cannot connect to server");
    exit (1);
  }
  
  recv(s, greeting, sizeof(greeting), 0);
  printf("%s", greeting);

  //send_query(s, keyfile); //the keyfile is for testing purposes- i will not be using a keyfile for queries, just for encryption/decryption
  
  
  char user_option[1000];

  while(1) 
    {
      printf("What would you like to do?\n");
      printf("1 - Send query\n");
      printf("2 - Update a whiteboard entry without encryption\n");
      printf("3 - Update a whiteboard entry with encryption\n");
      printf("4 - Clean an entry\n");
      printf("5 - Exit\n");

      //scanf("%i", &user_option);

      fgets(user_option, sizeof(user_option), stdin);
      
      if (strncmp(user_option, "1", 1) == 0)
	{
	  send_query(s, keyfile);
	}

      if (strncmp(user_option, "2", 1) == 0)
	{
	  plaintext_entry(s);
	}

      if (strncmp(user_option, "3", 1) == 0)
	{
	  encrypted_entry(s, keyfile);
	}

      if (strncmp(user_option, "4", 1) == 0)
	{
	  clean_entry(s);
	}
      
      if (strncmp(user_option, "5", 1) == 0)
	{
	  close(s);
	  return 1;
	}
      
      /*else
	{
	  printf("Please enter in one of the above options\n");
	  }*/

    } 

}
  
void send_query(int s, FILE * keyfile)
{
  
  printf("Which entry of the whiteboard would you like to receive?\n");

  char response[10000], query[10000], char1[10], char2[10], message[10000], plaintext[10000], prepend[] = "?";

  int entry_num, entry_len;

  fgets(query, sizeof(query), stdin);

  strcat(prepend, query);
  strcat(prepend, "\n");

  // printf("sent = %s\n", prepend);
  
  send(s, prepend, sizeof(prepend), 0);

  recv(s, response, sizeof(response), 0);

  /*
  char message[] = "CMPUT379some Crypto Text test blah yadda yadda random really wowehie";
  char plaintext[10000], outtext[10000], server_message[10000];

 
  printf("encrypted, encoded message = %s\n", outtext);
  send(s, outtext, sizeof(outtext), 0);

  recv(s, server_message, sizeof(server_message), 0);
  printf("message received back from server = %s\n", server_message);*/

  sscanf(response, "%s %i %s %i %s", char1, &entry_num, char2, &entry_len, message);

  //  printf("char2 = %s\n", char2);

  if (strcmp(char2, "c") == 0)
    // if the entry is encrypted, we want to decrypt it
    {
      decrypt(message, plaintext, keyfile);
      rewind(keyfile);
      printf("%s%i%s%i%s\n", char1, entry_num, char2, entry_len, plaintext); 
    }

  else
    // otherwise we just print out the whole thing without doing anything to it
    {
      printf("%s\n", response);
    }

  
  //read (s, &number, sizeof (number));
  // close (s);
  //fprintf (stderr, "Process %d gets number %d\n", getpid (),
  //	     ntohl (number));
  //sleep (2);

  
  //free(base64_encoded);
  //free(base64_decoded);
}
 
char *base64decode (const void *b64_decode_this, int decode_this_many_bytes){
  BIO *b64_bio, *mem_bio;      //Declares two OpenSSL BIOs: a base64 filter and a memory BIO.
  char *base64_decoded = calloc( (decode_this_many_bytes*3)/4+1, sizeof(char) ); //+1 = null.
  b64_bio = BIO_new(BIO_f_base64());                      //Initialize our base64 filter BIO.
  mem_bio = BIO_new(BIO_s_mem());                         //Initialize our memory source BIO.
  BIO_write(mem_bio, b64_decode_this, decode_this_many_bytes); //Base64 data saved in source.
  BIO_push(b64_bio, mem_bio);          //Link the BIOs by creating a filter-source BIO chain.
  BIO_set_flags(b64_bio, BIO_FLAGS_BASE64_NO_NL);          //Don't require trailing newlines.
  int decoded_byte_index = 0;   //Index where the next base64_decoded byte should be written.
  while ( 0 < BIO_read(b64_bio, base64_decoded+decoded_byte_index, 1) ){ //Read byte-by-byte.
    decoded_byte_index++; //Increment the index until read of BIO decoded data is complete.
  } //Once we're done reading decoded data, BIO_read returns -1 even though there's no error.
  BIO_free_all(b64_bio);  //Destroys all BIOs in chain, starting with b64 (i.e. the 1st one).
  return base64_decoded;        //Returns base-64 decoded data with trailing null terminator.
}
 
 
int decrypt(unsigned char *ciphertext, unsigned char *plaintext, FILE * keyfile)
{
  EVP_CIPHER_CTX ctx;
  //  unsigned char key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  unsigned char key[32];
  unsigned char iv[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  int len, tmplen;
  int plaintext_len;
  float j, k, ciphertext_len;

  
  //printf("message after base64 decoding = %s\n", ciphertext);

  EVP_CIPHER_CTX_init(&ctx);

  // need a loop here to go through all keys in keyfile
  // successful decrypt should have the first word as "CMPUT379"
  // failed decrypt ran out of keys/error

  bool successful_decrypt = false;

  if (keyfile != NULL)
  {

    int bytes_to_decode = strlen(ciphertext); //Number of bytes in string to base64 decode.
	
    char *base64_decoded = base64decode(ciphertext, bytes_to_decode);   //Base-64 decoding.
    
    strcpy(ciphertext, base64_decoded);
  
    while ((fgets(key, 33, keyfile) != NULL))
      {
	
	EVP_DecryptInit_ex(&ctx, EVP_aes_256_cbc(), NULL, key, iv);
	
	
	ciphertext_len = strlen(ciphertext);
	printf("string_length after base64 decoding = %lf\n", ciphertext_len);
	

	if(!EVP_DecryptUpdate(&ctx, plaintext, &len, ciphertext, ciphertext_len)) {
	  return 0;
	}
	
	if (strncmp(plaintext, "CMPUT379", 8) == 0)
	  {
	    successful_decrypt = true;
	    printf("decrypt succeeded\n");
	    break;
	  }
	
      }
    
    if (successful_decrypt != true)
      {
	printf("decrypt failed, key not in possession\n");
	return 0;
      }

    else
      {
	if(!EVP_DecryptFinal_ex(&ctx, plaintext + len, &tmplen)) {
	  return 0;
	}
	
	
	len += tmplen;
	
	plaintext[len] = 0;
	
	EVP_CIPHER_CTX_cleanup(&ctx);
	
	
	return 1;
      }	    
    
  }
  
  //NEED AN ELSE FOR NOT HAVING KEYFILE
  else
    {
      printf("cannot decrypt without a keyfile\n");
      
      return 0;
    }	    
  
} 
  



char *base64encode (const void *b64_encode_this, int encode_this_many_bytes){
  BIO *b64_bio, *mem_bio;      //Declares two OpenSSL BIOs: a base64 filter and a memory BIO.
  BUF_MEM *mem_bio_mem_ptr;    //Pointer to a "memory BIO" structure holding our base64 data.
  b64_bio = BIO_new(BIO_f_base64());                      //Initialize our base64 filter BIO.
  mem_bio = BIO_new(BIO_s_mem());                           //Initialize our memory sink BIO.
  BIO_push(b64_bio, mem_bio);            //Link the BIOs by creating a filter-sink BIO chain.
  BIO_set_flags(b64_bio, BIO_FLAGS_BASE64_NO_NL);  //No newlines every 64 characters or less.
  BIO_write(b64_bio, b64_encode_this, encode_this_many_bytes); //Records base64 encoded data.
  BIO_flush(b64_bio);   //Flush data.  Necessary for b64 encoding, because of pad characters.
  BIO_get_mem_ptr(mem_bio, &mem_bio_mem_ptr);  //Store address of mem_bio's memory structure.
  BIO_set_close(mem_bio, BIO_NOCLOSE);   //Permit access to mem_ptr after BIOs are destroyed.
  BIO_free_all(b64_bio);  //Destroys all BIOs in chain, starting with b64 (i.e. the 1st one).
  BUF_MEM_grow(mem_bio_mem_ptr, (*mem_bio_mem_ptr).length + 1);   //Makes space for end null.
  (*mem_bio_mem_ptr).data[(*mem_bio_mem_ptr).length] = '\0';  //Adds null-terminator to tail.
  return (*mem_bio_mem_ptr).data; //Returns base-64 encoded data. (See: "buf_mem_st" struct).
}


int encrypt(unsigned char * intext, unsigned char * outtext, FILE * keyfile) {
  int outlen, tmplen, i;
  //unsigned char key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  unsigned char key[32];
  unsigned char iv[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  //  char intext[] = "some Crypto Text test blah yadda yadda random really really ruownowrni uwrnwirowni owurowurwior 7579"; 
  EVP_CIPHER_CTX ctx;
  FILE *out;
  float string_length, j, k;

  EVP_CIPHER_CTX_init(&ctx);

  
  if (keyfile != NULL)
    {
  
      // grab the first key from the keyfile.... I think
      if (fgets(key, 33, keyfile) != NULL)
	{

	  //printf("key = %s\n", key);
	  //printf("key length = %li\n", sizeof(key));
	  

	  // I need to use this because I want to start from the top of the file after every use of fgets
	  rewind(keyfile);


	  // alright so at this point I know that the key is properly being read in from the file, and that the key is the proper length (32)
	  

	  EVP_EncryptInit_ex(&ctx, EVP_aes_256_cbc(), NULL, key, iv);

	  //printf("original plaintext message = %s\n", intext);
    
	  string_length = strlen(intext);
	  //printf("original string_length = %lf\n", string_length);
	  
	  
	  if(!EVP_EncryptUpdate(&ctx, outtext, &outlen, intext, strlen(intext))) {
	    return 0;
	  }
	  
	  if(!EVP_EncryptFinal_ex(&ctx, outtext + outlen, &tmplen)) {
	    return 0;
	  }
	  

	  outlen += tmplen;
	  EVP_CIPHER_CTX_cleanup(&ctx);

	  //    for  (i=0; i<=outlen-1; i++){
	  //printf("%c", outbuf[i]);
	  //}
	  
	  int bytes_to_encode = outlen; //Number of bytes in string to base64 encode.
	  
	  char *base64_encoded = base64encode(outtext, bytes_to_encode);   //Base-64 encoding.
	  
	  strcpy(outtext, base64_encoded);
	  
	  return 1;
	}
    }
  
  else 
    {
      printf("encryption not possible without a keyfile\n");
      return 0;
    }

}

void plaintext_entry(int s)
{

  char response[10000], user_input[10000], entry_num[10], entry_len[10], intext[10000];
  int entry_len_int;

  printf("What is the message you would like to send?\n");

  fgets(user_input, sizeof(user_input), stdin);
  
  entry_len_int = (strlen(user_input) - 1);

  // need to convert the length of the input to string so it can be concatenated
  sprintf(entry_len, "%i", entry_len_int);

  //printf("entry_len = %s\n", entry_len);

  printf("What is the number of the whiteboard entry you would like to update?\n");

  fgets(entry_num, sizeof(entry_num), stdin);

  // need to remove newline at the end of input
  entry_num[strcspn(entry_num, "\n")] = 0;
  
  strcat(intext, "@");
  strcat(intext, entry_num);
  strcat(intext, "p");
  strcat(intext, entry_len);
  strcat(intext, "\n");
  strcat(intext, user_input);
  strcat(intext, "\n");
  
  //printf("sent = %s\n", intext);

  send(s, intext, sizeof(intext), 0);

  recv(s, response, sizeof(response), 0);

  printf("response = %s\n", response);

  memset(intext, 0, sizeof(intext));
  memset(response, 0, sizeof(response));

  //printf("%s\n", response); 

}

void encrypted_entry(int s, FILE * keyfile)
{

  char response[10000], entry_num[10], entry_len[10], intext[10000], outtext[10000], prepend1[10000], prepend2[] = "CMPUT379 Whiteboard Encrypted v0\n";
  int entry_len_int;

  printf("What is the message you would like to send?\n");

  fgets(intext, sizeof(intext), stdin);

  strcat(prepend2, intext);

  //printf("prepend2 = %s\n", prepend2);

  printf("What is the number of the whiteboard entry you would like to update?\n");

  fgets(entry_num, sizeof(entry_num), stdin);

  // need to remove newline at the end of input
  entry_num[strcspn(entry_num, "\n")] = 0;
  
  strcat(prepend1, "@");
  strcat(prepend1, entry_num);
  strcat(prepend1, "c");
  
  encrypt(prepend2, outtext, keyfile);
  
  entry_len_int = (strlen(outtext));
  // need to convert the length of the input to string so it can be concatenated
  sprintf(entry_len, "%i", entry_len_int);
  //printf("entry_len = %s\n", entry_len);

  strcat(prepend1, entry_len);
  strcat(prepend1, "\n");


  strcat(prepend1, outtext);

  //printf("sent = %s\n", prepend1);

  send(s, prepend1, sizeof(prepend1), 0);

  recv(s, response, sizeof(response), 0);

  printf("response = %s\n", response);

  //printf("%s\n", response); 

  memset(prepend1, 0, sizeof(prepend1));
  memset(response, 0, sizeof(response));

}

void clean_entry(int s)
{

  char entry_num[10], message[1000], reply[10000];

  printf("Which entry do you want to clean?\n");

  fgets(entry_num, sizeof(entry_num), stdin);

  entry_num[strcspn(entry_num, "\n")] = 0;
  
  strcat(message, "@");
  strcat(message, entry_num);
  strcat(message, "p0\n\n");

  //printf("message = %s\n", message);

  send(s, message, sizeof(message), 0);

  recv(s, reply, sizeof(reply), 0);

  printf("reply = %s\n", reply);

}
