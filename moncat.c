//
//  main.c
//  moncat
//
//  Created by MOUSSAOUI Abderahmane on 08/02/2018.
//  Copyright © 2018 Abdou. All rights reserved.
//

// Make file
/*
CC = cc -g -Wall -Wextra -Werror

OBJ = moncat.o

%.o: %.c
	 $(CC) -c -o $@ $<

moncat: $(OBJ)
	gcc -o $@ $^

clean:
	rm -rf *.o moncat
*/

// test_1: OK
// test_2: cmp: EOF on sortie (problème mongetchar)
// test_3: OK

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>


int optN=0, optE=0, filec=0, i=0, curf=0;
int c=0;


int mongetchar()
{

    static char buf[1024];
    static char *bufp = buf;
    static int i = 0;

    if (i <= 0)
    {
        if ((i = read(curf, buf, 1)) > 0)
            bufp = buf;
    }
    if ( --i >= 0 )
    {
        return  *bufp++;
    }
    else return EOF;
}


void getopts (int argc, char *argv[])
{
  int opt;
  while ((opt = getopt(argc, argv, "nE")) != -1)
  {
    switch (opt)
    {
        case 'n':
            optN = 1;
            break;

        case 'E':
            optE = 1;
            break;

        case '?':
            fprintf(stderr, "Usage: moncat -n / -E fichier .. fichier\n");
            break;

        default:
            perror("Error ");
            break;
    }
  }
}

void affiche_f (int argc, char *argv[])
{
  int nb_l = 1; int f=1;
  filec = argc - optind;
  char **filev = NULL;

/* moncat sans arguments */

  if ( filec == 0 )
  {
    int b;
    while ((b = mongetchar()) != EOF)
    {
          if (optN == 1 && f==1)
          {
            printf("%6u\t", nb_l);
            nb_l++;
            f=0;
          }
          if (b =='\n')
          {
          if (optE == 1) putchar('$');
             f=1;
          }
          putchar(b);
      }
    }

  /* moncat avec arguments */

  for (i=optind; i < argc; i++)
  {
      filev = argv + i;
      int d = open(*filev, O_RDONLY);

      if (d == -1) perror("Error ");
      curf=d;

      if (optN == 1)
      {
        printf("%6u\t", nb_l);
      }
      while ((c = mongetchar()) != EOF)
      {
        if (optN == 1 && f==1)
        {
          printf("%6u\t", nb_l);
          nb_l++;
          f=0;
        }
        if (c == '\n')
        {
        if (optE == 1) putchar('$');
           f=1;
        }
        putchar(c);
      }
        close(d);
  }
}

int main(int argc, char *argv[])
{
    getopts(argc, argv);
    affiche_f(argc, argv);
return 0;
}
