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

#define	 MY_PORT  2222


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
 
 
int decrypt(unsigned char *ciphertext, unsigned char *plaintext)
{
  EVP_CIPHER_CTX ctx;
  unsigned char key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  unsigned char iv[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  int len, tmplen;
  int plaintext_len;
  float j, k, ciphertext_len;

  int bytes_to_decode = strlen(ciphertext); //Number of bytes in string to base64 decode.
   
  char *base64_decoded = base64decode(ciphertext, bytes_to_decode);   //Base-64 decoding.

  strcpy(ciphertext, base64_decoded);

  //printf("message after base64 decoding = %s\n", ciphertext);

  EVP_CIPHER_CTX_init(&ctx);
  EVP_DecryptInit_ex(&ctx, EVP_aes_256_cbc(), NULL, key, iv);

  ciphertext_len = strlen(ciphertext);
  printf("string_length after base64 decoding = %lf\n", ciphertext_len);
  
  if(!EVP_DecryptUpdate(&ctx, plaintext, &len, ciphertext, ciphertext_len)) {
    return 0;
  }


  if(!EVP_DecryptFinal_ex(&ctx, plaintext + len, &tmplen)) {
    return 0;
  }


  len += tmplen;
  EVP_CIPHER_CTX_cleanup(&ctx);


  return 1;
  

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


int encrypt(unsigned char * message) {
  int outlen, tmplen, i;
  unsigned char key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  unsigned char iv[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  char intext[] = "some Crypto Text test blah yadda yadda random really really"; 
  EVP_CIPHER_CTX ctx;
  FILE *out;
  float string_length, j, k;

  EVP_CIPHER_CTX_init(&ctx);
  EVP_EncryptInit_ex(&ctx, EVP_aes_256_cbc(), NULL, key, iv);


  printf("original plaintext message = %s\n", intext);
 
  string_length = strlen(intext);
  printf("original string_length = %lf\n", string_length);
  

  if(!EVP_EncryptUpdate(&ctx, message, &outlen, intext, strlen(intext))) {
    return 0;
  }

  
  if(!EVP_EncryptFinal_ex(&ctx, message + outlen, &tmplen)) {
    return 0;
  }

  outlen += tmplen;
  EVP_CIPHER_CTX_cleanup(&ctx);
  for  (i=0; i<=outlen-1; i++){
    //printf("%c", outbuf[i]);
  }

  int bytes_to_encode = strlen(message); //Number of bytes in string to base64 encode.

  char *base64_encoded = base64encode(message, bytes_to_encode);   //Base-64 encoding.

  strcpy(message, base64_encoded);

  return 1;
}



int main(int argc, char * argv[])
{
  // argv[1] = hostname, argv[2] = portnumber, argv[3] (if it exists) = keyfile
  
  int i, s, number, n;

  unsigned char greeting[1024], message[1024], plaintext[1024];

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
  
  int user_option;

  while(1) 
    {
      printf("What would you like to do?\n");
      printf("1 - Send query\n");
      printf("2 - Update a whiteboard entry without encryption\n");
      printf("3 - Update a whiteboard entry with encryption\n");
      printf("4 - Clean an entry\n");
      printf("5 - Exit\n");

      scanf("%i", &user_option);
      
      if (user_option == 1)
	{
	  break;
	}

      if (user_option == 2)
	{
	  break;
	}

      if (user_option == 3)
	{
	  break;
	}

      if (user_option == 4)
	{
	  break;
	}
      
      if (user_option == 5)
	{
	  break;
	}

    }
  

  
  encrypt(message);
  printf("encrypted, encoded message = %s\n", message);
  n = send(s, message, sizeof(message), 0);
 
  recv(s, message, sizeof(message), 0);
  printf("message received back from server = %s\n", message);
  
  decrypt(message, plaintext);
  int j = strlen(plaintext);
  int k = plaintext[j-1];
  printf("last char = %i\n", k);
  printf("decrypted message = %s\n", plaintext);

  /* printf("decrypted message = ");
     for (i=0;i<20;i++) {
     printf("%c", message[i]);
     }
     printf("\n"); */
  
  //read (s, &number, sizeof (number));
  close (s);
  //fprintf (stderr, "Process %d gets number %d\n", getpid (),
  //	     ntohl (number));
  sleep (2);
 
  
  
  //free(base64_encoded);
  //free(base64_decoded);
}
 

