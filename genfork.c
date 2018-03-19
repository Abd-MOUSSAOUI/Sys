//
//  main.c
//  genfork
//
//  Created by Abderahmane_MOUSSAOUI on 08/03/2018.
//  Copyright © 2018 Abdou. All rights reserved.
//


/*********************
        Le test prends un peu de temps (5min environ) mais à la fin
                        il affiche Test réussi

Ps: Quand j'enlève le test de mémoire il affiche Test réussi au bout de 2min !
Sinon avec le test de mémoire ça prends du temps mais à la fin le test réussi
                                                          ********************/


/*Test noter*/
//    Test 100 OK
//    Test 200 FAIL !!
//    Test 300 OK

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>			// pour les msg d'erreur uniquement
#include <stdnoreturn.h>		// C norme 2011
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>

#define	CODE_ERREUR	255

int dflag = 0;
int vflag = 0;

char *prog ;				// nom du programme pour les erreurs

noreturn void raler (int syserr, const char *fmt, ...)
{
    va_list ap ;

    va_start (ap, fmt) ;
    fprintf (stderr, "%s: ", prog) ;
    vfprintf (stderr, fmt, ap) ;
    fprintf (stderr, "\n") ;
    va_end (ap) ;

    if (syserr)
	perror ("") ;

    exit (CODE_ERREUR) ;
}

noreturn void usage (void)
{
    fprintf (stderr, "usage: %s [-d|-v] [fichier-spec]\n", prog) ;
    exit (1) ;
}


struct sommet
{
    int val ;					// étiquette du sommet
    struct sommet *frere_suivant ;		// liste des frères
    struct sommet *premier_fils ;		// premier des fils
    struct sommet *dernier_fils ;		// pour insertion en queue
} ;


enum ret_al { AL_ERR, AL_FIN, AL_OK } ;		// retour de analyser_ligne

enum ret_al analyser_ligne (char **spec, int *prof, int *val)
{
    *prof = *val = 0 ;				// mettre les compteurs à 0

    if (**spec == '\0')				// spec terminée
	return AL_FIN ;


    while (**spec == ' ')
    {
	(*spec)++ ;
	(*prof)++ ;
    }


    char *debut = *spec ;			// vérifier au moins un chiffre
    while (isdigit (**spec))
    {
	*val = *val * 10 + (**spec - '0') ;
	(*spec)++ ;
    }
    if (debut == *spec)				// on n'a pas avancé => pb
	return AL_ERR ;


    if (**spec != '\n')
	return AL_ERR ;
    (*spec)++ ;					// passer au sommet suivant

    return AL_OK ;
}


struct sommet *creer_sommet (int val)
{
    struct sommet *s ;

    s = malloc (sizeof *s) ;
    if (s == NULL)
	raler (1, "cannot malloc %d bytes", sizeof *s) ;

    s->frere_suivant = NULL ;
    s->premier_fils = NULL ;
    s->dernier_fils = NULL ;
    s->val = val ;
    return s ;
}


struct sommet *analyser_spec_profondeur (int *numl, char **spec, int prof)
{
    char *nspec ;
    int nprof, val ;
    struct sommet *s ;
    enum ret_al r ;

    nspec = *spec ;				// ne pas avancer si pas ok
    r = analyser_ligne (&nspec, &nprof, &val) ;	// lire la ligne numl
    switch (r)
    {
	case AL_ERR :
	    raler (0, "cannot parse spec at line %d", *numl) ;
	case AL_FIN :
	    return NULL ;
	case AL_OK :				// on continue en séquence
	    break ;
	default :				// programmation défensive
	    raler (0, "internal error at line %d", *numl) ;
    }
    if (nprof != prof)
	raler (0, "unexpected depth (%d) at line %d", nprof, *numl) ;

    s = creer_sommet (val) ;
    *spec = nspec ;
    (*numl)++ ;

    while ((r = analyser_ligne (&nspec, &nprof, &val)) == AL_OK && nprof > prof)
    {
	struct sommet *f ;			// sous-arbre fils trouvé

	nspec = *spec ;
	f = analyser_spec_profondeur (numl, &nspec, prof + 1) ;

	if (s->dernier_fils == NULL)
	{
	    s->premier_fils = s->dernier_fils = f ;
	}
	else
	{
	    s->dernier_fils->frere_suivant = f ;
	    s->dernier_fils = f ;
	}
	*spec = nspec ;
    }

    if (r == AL_ERR)
	raler (0, "cannot parse spec at line %d", *numl) ;

    return s ;
}

struct sommet *analyser_spec (char *spec)
{
    struct sommet *s ;				// arbre obtenu
    int numl ;					// numéro de ligne pour msg err

    numl = 1 ;
    s = analyser_spec_profondeur (&numl, &spec, 0) ;
    if (*spec != '\0')				// est-on bien à la fin ?
	raler (0, "invalid spec at line %d", numl) ;
    return s ;
}

/******************************************************************************
 * Ajoutez vos fonctions à la suite
 */

void analyser_opts(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "vd")) != -1)
    {
        switch (opt)
        {
            case 'v':
                vflag = 1;
                break;
            case 'd':
                dflag = 1;
                break;
            case '?':
                usage();
                exit(CODE_ERREUR);
            default:
                abort();
                break;
        }
    }
}

char *lire_spec(int fd)
{
    char c, *spec = malloc(1);
    *spec = '\0';
    int nbchar = 0;
    while ((read(fd, &c, 1)) > 0)
    {
        spec = realloc(spec, nbchar + 2);
        if (spec == NULL)
    	   raler (1, "cannot malloc %d bytes", sizeof *spec) ;
        spec[nbchar] = c;
        spec[nbchar + 1] = '\0';
        nbchar++;
    }
    return spec;
}


void affiche_arbre(struct sommet *racine, int prof)
{
    if (racine == NULL) return;
    struct sommet *tmp = racine->premier_fils;
    int i;
    for (i = 0; i < prof; i++)
        printf(" ");
    printf("%d\n", racine->val);
    while(tmp != NULL)
    {
        affiche_arbre(tmp, prof + 1);
        tmp = tmp->frere_suivant;
    }
}

int parcourir_arbre(struct sommet *racine)
{
    if (racine == NULL) return 0;
    int fils = 0;
    struct sommet *tmp = racine->premier_fils;
    while (tmp != NULL)
    {
        pid_t pid = fork();
        switch (pid)
        {
            case -1: // ERREUR
                fprintf(stderr, "Erreur fork du processus\n");
                exit(CODE_ERREUR);
                break;
            case 0: // FILS
                exit(parcourir_arbre(tmp));
                break;
            default: // PARENT
                if (vflag == 1)
                    printf("pid %d -> %d\n", getpid(), pid);
                break;
        }
        tmp = tmp->frere_suivant;
    }
    usleep(racine->val * 1000);
    if (vflag == 1)
        printf("pid %d attente %d ms\n", getpid(), racine->val);

    tmp = racine->premier_fils;
    while (tmp != NULL)
    {
        int etat;
        wait(&etat);
        if (WIFEXITED(etat))
        {
            fils += WEXITSTATUS(etat);
            if (fils >= 255)
                exit(CODE_ERREUR);
            if (vflag)
                printf("pid %d <- %d processus\n", getpid(), fils);
        }
        tmp = tmp->frere_suivant;
    }
    return fils + 1;
}

void libere_arbre(struct sommet **racine)
{
    if (*racine == NULL) return;
    struct sommet *tmp = (*racine)->premier_fils;
    while (tmp != NULL)
    {
        struct sommet *suiv = tmp->frere_suivant;
        libere_arbre(&tmp);
        tmp = suiv;
    }
    free(*racine);
}

void libere_ressources(struct sommet **racine, char **spec)
{
    libere_arbre(racine);
    free(*spec);
}


int main(int argc, char *argv[])
{
    int fd = 0;
    prog = argv[0];

    analyser_opts(argc, argv);

    if (dflag == 1 && vflag == 1)
    {
        usage();
        exit(CODE_ERREUR);
    }

    if (argc > optind+1)
    {
        usage();
        exit(CODE_ERREUR);
    } else if (argc == optind+1)
    {
        fd = open(argv[optind], O_RDONLY);
        if (fd == -1)
        {
            fprintf(stderr, "Erreur d'ouveture du fichier %s\n", argv[optind]);
            exit(CODE_ERREUR);
        }
    }

    char *spec = lire_spec(fd);

    struct sommet *racine = analyser_spec(spec);

    if (racine == NULL) return 0;

    if (dflag)
    {
        affiche_arbre(racine, 0);
        libere_ressources(&racine, &spec);
        return 0;
    }

    int c = parcourir_arbre(racine);

    libere_ressources(&racine, &spec);

    return c;
}
