#define _CRT_SECURE_NO_WARNINGS
#include "pile.h"

/*
 * Fichier : pile.c
 * Description : Implémentation du TAD Pile en utilisant les fonctions de Liste.
 *               Cela respecte le principe d'encapsulation et de réutilisation.
 */

 /* Initialise la pile (appelle initListe) */
void initPile(Pile* p) {
    initListe(p);
}

/* Vérifie si la pile est vide */
int pileEstVide(const Pile* p) {
    return listeEstVide(p);
}

/* Empile un élément (insère en tête de liste) */
void empiler(Pile* p, Element val) {
    insererTete(p, val);
}

/* Dépile un élément (supprime la tête de liste) */
Element depiler(Pile* p) {
    return supprimerTete(p);
}

/* Regarde le sommet (regarde la tête de liste) */
Element sommet(const Pile* p) {
    return teteListe(p);
}

/* Renvoie la hauteur de la pile */
int taillePile(const Pile* p) {
    return p->taille;
}

/* Détruit la pile */
void detruirePile(Pile* p) {
    detruireListe(p);
}
