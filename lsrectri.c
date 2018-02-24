//
//  main.c
//  lsrectri
//
//  Created by Abderahmane_MOUSSAOUI on 22/02/2018.
//  Copyright © 2018 Abderahmane_MOUSSAOUI. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>



typedef struct s_dirreg
{
    char *name;
    size_t size;
} *dirreg;

int i=0;
dirreg *tab=NULL;


void lsrectri (char *dir)
{
    DIR *dp = opendir (dir);
    if (!dp)
    {
        return;
    }
    struct dirent *d;
    struct stat regdir;
    while (( d = readdir (dp)) != NULL )
    {
        if (( strcmp (d -> d_name , ".") != 0 &&
             strcmp (d -> d_name , "..") != 0))
        {
            char *sous_path = malloc(strlen(dir) + strlen(d->d_name) + 2);
            strcpy(sous_path, dir);
            strcat(sous_path, "/");
            strcat(sous_path, d->d_name);
            lstat(sous_path,&regdir);
            if (S_ISDIR(regdir.st_mode))
            {
                lsrectri(sous_path);
                free(sous_path);
            }
            else if (S_ISREG(regdir.st_mode))
            {
                i++;
                dirreg new = malloc(sizeof(struct s_dirreg));
                if (tab)
                {
                    tab = realloc(tab, i * sizeof(dirreg));
                } else
                {
                    tab = malloc(sizeof(dirreg));
                }
                new->size = regdir.st_size;
                new->name = sous_path;
                tab[i-1] = new;
            } else free(sous_path);
        }
    }
    closedir (dp);
}

int cmp(const void *a, const void *b)
{
    return (int)((*(dirreg *)a)->size - (*(dirreg *)b)->size);
}

int main (int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: ./lsrectri [repertoire] \n");
        exit(1);
    }
    lsrectri(argv[1]);
    qsort(tab, i, sizeof(dirreg), &cmp);
    int j;
    for (j=0; j<i; j++){
        printf("%zu\t %s\n",tab[j]->size, tab[j]->name);
        free(tab[j]->name);
        free(tab[j]);
    }
    free(tab);
    return 0;
}
