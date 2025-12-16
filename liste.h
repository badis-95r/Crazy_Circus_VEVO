#pragma once

/*
 * Fichier : liste.h
 * Description : Définition d'une liste chaînée générique simple pour stocker des chaînes de caractères.
 *               Dans ce projet, elle servira de base pour la Pile (podiums) et potentiellement
 *               pour d'autres collections.
 */

 /* Définition du type d'élément stocké dans la liste.
  * Ici, nous manipulons des noms d'animaux, donc des chaînes de caractères (char*).
  */
typedef char* Element;

/* Structure représentant un maillon (cellule) de la liste */
typedef struct Cellule {
    Element valeur;          /* La donnée stockée (le nom de l'animal) */
    struct Cellule* suivant; /* Pointeur vers le maillon suivant */
} Cellule;

/* Structure représentant la liste elle-même (pointeur vers le premier élément) */
typedef struct {
    Cellule* tete; /* Pointeur vers le sommet de la liste */
    int taille;    /* Nombre d'éléments dans la liste (pratique pour le jeu) */
} Liste;

/* Prototypes des fonctions */

/* Initialise une liste vide */
void initListe(Liste* l);

/* Vérifie si la liste est vide (renvoie 1 si vide, 0 sinon) */
int listeEstVide(const Liste* l);

/* Insère un élément en tête de liste */
void insererTete(Liste* l, Element val);

/* Supprime l'élément en tête de liste et le renvoie */
Element supprimerTete(Liste* l);

/* Renvoie la valeur de l'élément en tête sans le supprimer (Consultation) */
Element teteListe(const Liste* l);

/* Vide la liste et libère la mémoire de toutes les cellules */
void detruireListe(Liste* l);
