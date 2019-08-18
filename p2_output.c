/**
 * wyjście
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

typedef union
{
 intmax_t ir;
 char     sr[sizeof(intmax_t)];
} tochar;

int main ( )
{
 printf ( "Inicjalizacja wyjścia\n" );
 
 FILE * fo = fopen ( "/tmp/p2_out", "r" );
 char sdata[sizeof(intmax_t)+sizeof(char)] = { 0 };
 
 while ( !feof (fo) )
 {
  fread(sdata, 1, sizeof(intmax_t)+sizeof(char), fo);
  if (sdata[0] == 0)
   break;
  
  tochar x;
  memcpy(x.sr, sdata+sizeof(char), sizeof(intmax_t));
  
  printf ( "Wynik: %lli\n", x.ir );
 }
 
 fclose ( fo );
 
 return 0;
}