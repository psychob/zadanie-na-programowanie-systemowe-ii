/**
 * interfejs
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

typedef union
{
 intmax_t ir;
 char     sr[sizeof(intmax_t)];
} tochar;

int main ( )
{
 if (mkfifo("/tmp/p2_int", 0770) != 0)
 {
  printf ( "Nie udało się zainicjalizować nazwanego potoku\n");
  return 1;
 }
 printf ( "Inicjalizacja interfejsu, uruchom program p2_logic\n" );
 
 FILE * f = fopen ( "/tmp/p2_int", "w" );
 
 printf( "Wprowadź wyrażenie do obliczenia w formie: <LICZBA> <WYR> <LICZBA>:\n");
 printf( " Gdzie <LICZBA> to liczba\n");
 printf( " Gdzie <WYR> to znak wyrażenia (+, -, /, *)\n");
 printf( " Pusta linia kończy wykonywanie programu\n");
 
 char buff[1024] = { 0 };
 while (true)
 {
  printf ("> ");
  
  unsigned it = 0;
  int jt = 0;
  
  while ((buff[it] = getc(stdin)) != '\n') it++;
  if (it == 0)
  {
   char buff[sizeof(intmax_t)*2+sizeof(char)*2] = { 0 };
   fwrite(buff, 1, sizeof(intmax_t)*2+sizeof(char)*2, f);
   break;
  }
  
  // analizujemy te liczby, zakładając że to będą dwa inty
  char * fn = 0,
       * fne = 0,
       * ws  = 0,
       * sn  = 0,
       * sne = 0;
  for (jt = 0; jt < it; ++jt)
  {
   if (isspace(buff[jt]))
   {
    if (fn && !fne)
     fne = buff + jt;
    else if (sn && !sne)
    {
     sne = buff + jt;
     break;
    }
   } else if (isdigit(buff[jt]))
   {
    if (!fn && !fne)
     fn = buff + jt;
    else if (fne && !sn && !sne)
     sn = buff + jt;
   } else if (buff[jt] == '+' || buff[jt] == '-' || buff[jt] == '*' || buff[jt] == '/')
    if (!ws)
    {
     if (!fne)
      fne = buff +jt;
     ws = buff + jt;
    }
  }
  
  if (fn == NULL || sn == NULL || ws == NULL)
  {
   printf ( "Nieprawidłowe wyrażenie\n" );
   continue;
  }
  
  intmax_t fnmb = strtoimax(fn, NULL, 10),
           snmb  = strtoimax(sn, NULL, 10);
  
  char xbuff[sizeof(intmax_t)*2+sizeof(char)*2] = { 0 };
  {
   tochar x;
   x.ir = fnmb;
   memcpy(xbuff, x.sr, sizeof(intmax_t));
   x.ir = snmb;
   memcpy(xbuff + sizeof(intmax_t), x.sr, sizeof(intmax_t));
   xbuff[sizeof(intmax_t)*2] = *ws;
  }
  
  fwrite(xbuff, 1, sizeof(intmax_t)*2+sizeof(char)*2, f);
  fflush(f);
 }
 
 fclose ( f );
 
 unlink( "/tmp/p2_int" );
 
 return 0;
}