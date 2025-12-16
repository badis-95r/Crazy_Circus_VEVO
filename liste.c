#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "liste.h"

/*
 * Fichier : liste.c
 * Description : Implémentation des fonctions de gestion de la liste chaînée.
 */

 /* Initialise une liste à l'état vide */
void initListe(Liste* l) {
    l->tete = NULL;
    l->taille = 0;
}

/* Vérifie si la liste est vide */
int listeEstVide(const Liste* l) {
    return (l->tete == NULL);
}

/* Ajoute un élément au début de la liste (opération PUSH en pile) */
void insererTete(Liste* l, Element val) {
    /* Allocation dynamique d'une nouvelle cellule */
    Cellule* nouv = (Cellule*)malloc(sizeof(Cellule));
    if (nouv == NULL) {
        fprintf(stderr, "Erreur fatale : Echec d'allocation memoire dans insererTete\n");
        exit(EXIT_FAILURE);
    }

    nouv->valeur = val;       /* On stocke la donnée */
    nouv->suivant = l->tete;  /* Le suivant du nouveau est l'ancienne tête */
    l->tete = nouv;           /* La tête de la liste devient le nouveau maillon */
    l->taille++;
}

/* Supprime et retourne l'élément en tête de liste (opération POP en pile) */
Element supprimerTete(Liste* l) {
    if (listeEstVide(l)) {
        /* On pourrait gérer une erreur ici, mais pour ce projet scolaire,
           on suppose que l'appelant vérifie avant (via estVide).
           On retourne NULL en cas de liste vide. */
        return NULL;
    }

    Cellule* aSupprimer = l->tete; /* On garde une référence vers la cellule à supprimer */
    Element valeur = aSupprimer->valeur; /* On récupère la donnée */

    l->tete = aSupprimer->suivant; /* La tête avance au maillon suivant */
    free(aSupprimer);              /* On libère la mémoire de la cellule */
    l->taille--;

    return valeur;
}

/* Renvoie l'élément en tête sans le supprimer (opération PEEK en pile) */
Element teteListe(const Liste* l) {
    if (listeEstVide(l)) {
        return NULL;
    }
    return l->tete->valeur;
}

/* Vide toute la liste */
void detruireListe(Liste* l) {
    while (!listeEstVide(l)) {
        supprimerTete(l);
    }
}
