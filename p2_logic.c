/**
 * logika
 * (c) 2013 Andrzej Budzanowski
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <ctype.h>
#include <stdlib.h>

#define BUFFER_SIZE sizeof(intmax_t)*2+sizeof(char)*2

typedef union
{
 intmax_t ir;
 char     sr[sizeof(intmax_t)];
} tochar;

int main ( )
{
 if (mkfifo("/tmp/p2_out", 0777) != 0)
 {
  printf ( "Nie udało się zainicjalizować nazwanego potoku\n");
  return 1;
 }
 
 printf ( "Inicjalizacja logiki, uruchom program p2_output\n" );
 
 FILE * fo = fopen ( "/tmp/p2_out", "w" ),
      * fi = fopen ( "/tmp/p2_int", "r" );
 
 // pobieramy z potoku dane po jednej lini
 char buff[BUFFER_SIZE] = { 0 },
      cbuf[BUFFER_SIZE] = { 0 };
 char sdata[sizeof(intmax_t)+sizeof(char)] = { 0 };
 while (!feof(fi))
 {
  fread(buff, 1, sizeof(intmax_t)*2+sizeof(char)*2, fi);
  
  if (!memcmp(buff, cbuf, BUFFER_SIZE))
  {
   memset(sdata, 0, sizeof(intmax_t) + sizeof(char));
   fwrite(sdata, 1, sizeof(intmax_t) + sizeof(char), fo);
   break;
  }
  
  // ładujemy
  intmax_t f, s;
  {
   tochar x;
   memcpy(x.sr, buff, sizeof(intmax_t));
   f = x.ir;
   memcpy(x.sr, buff + sizeof(intmax_t), sizeof(intmax_t));
   s = x.ir;
  }
  
  intmax_t w;
  switch (*(buff+sizeof(intmax_t)*2))
  {
   case '+': w = f + s; break;
   case '-': w = f - s; break;
   case '*': w = f * s; break;
   case '/': w = f / s; break;
  }
  
  // wysyłamy dane
  {
   tochar x;
   x.ir = w;
   sdata[0] = 1;
   memcpy(sdata+sizeof(char), x.sr, sizeof(intmax_t));
   fwrite(sdata, 1, sizeof(intmax_t) + sizeof(char), fo);
   fflush(fo);
  }
 }
 
 fclose ( fo );
 fclose ( fi );
 
 unlink( "/tmp/p2_out" );
 
 return 0;
}