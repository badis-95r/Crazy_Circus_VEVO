#pragma once

#include "liste.h"

/*
 * Fichier : pile.h
 * Description : Définition du Type Abstrait de Données (TAD) Pile.
 *               Conformément aux consignes du cours, la Pile est basée sur la Liste.
 *               Une Pile fonctionne en LIFO (Last In, First Out).
 */

 /* Définition du type Pile comme un alias de Liste */
typedef Liste Pile;

/* Prototypes des fonctions de la Pile */

/* Initialise une pile vide */
void initPile(Pile* p);

/* Vérifie si la pile est vide (1 si vide, 0 sinon) */
int pileEstVide(const Pile* p);

/* Ajoute un élément au sommet de la pile */
void empiler(Pile* p, Element val);

/* Retire et renvoie l'élément au sommet de la pile */
Element depiler(Pile* p);

/* Renvoie l'élément au sommet sans le retirer */
Element sommet(const Pile* p);

/* Renvoie la taille de la pile (hauteur) */
int taillePile(const Pile* p);

/* Vide la pile */
void detruirePile(Pile* p);
