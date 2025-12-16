#pragma once

#include "pile.h"
#include "animal.h"

/*
 * Fichier : jeu.h
 * Description : Définitions et prototypes pour la logique du jeu Crazy Circus.
 *               Contient la structure d'état (2 piles), les fonctions de mouvement,
 *               l'affichage, et la gestion des permutations.
 */

 /* Représente l'état du jeu à un instant T : Deux podiums (Bleu et Rouge) */
typedef struct {
    Pile podiumBleu;
    Pile podiumRouge;
} EtatJeu;

/* Structure pour stocker toutes les positions possibles (le "deck" de cartes) */
typedef struct {
    EtatJeu* positions; /* Tableau dynamique de toutes les positions possibles */
    int nbPositions;    /* Nombre total de positions stockées */
    int* estUtilisee;   /* Tableau de booléens pour savoir si une carte a déjà été tirée */
} Deck;

/* --- Gestion de l'état --- */

/* Initialise un état avec des piles vides */
void initEtat(EtatJeu* e);

/* Nettoie la mémoire d'un état */
void libererEtat(EtatJeu* e);

/* Copie profonde d'un état (utile pour la sauvegarde ou la génération) */
void copierEtat(const EtatJeu* src, EtatJeu* dest);

/* Compare deux états. Retourne 1 s'ils sont identiques, 0 sinon. */
int estMemeEtat(const EtatJeu* e1, const EtatJeu* e2);

/* --- Commandes du Jeu (Mouvements) --- */
/* Toutes ces fonctions retournent 1 si le mouvement a pu être fait, 0 sinon (ex: pile vide) */

/* KI : Sommet Bleu -> Sommet Rouge */
int commandeKI(EtatJeu* e);

/* LO : Sommet Rouge -> Sommet Bleu */
int commandeLO(EtatJeu* e);

/* SO : Echange les sommets (Swap) */
int commandeSO(EtatJeu* e);

/* NI : Bas Bleu -> Sommet Bleu (Rotation) */
int commandeNI(EtatJeu* e);

/* MA : Bas Rouge -> Sommet Rouge (Rotation) */
int commandeMA(EtatJeu* e);

/* Exécute une séquence de commandes donnée sous forme de chaîne (ex: "KIMALO") */
/* Modifie l'état 'e' en place. Retourne 1 si valide, 0 si séquence invalide ou impossible. */
int executerSequence(EtatJeu* e, const char* seq);

/* --- Algorithmes et Génération --- */

/* Génère toutes les positions possibles (permutations + répartitions) */
void genererToutesPositions(const ConfigJeu* config, Deck* deck);

/* Tire une nouvelle position cible aléatoire qui n'a pas encore été jouée */
/* Retourne un pointeur vers l'état cible, ou NULL si plus de cartes */
EtatJeu* tirerNouvelleCarte(Deck* deck);

/* --- Affichage --- */

/* Affiche l'état du jeu (le duel entre la position actuelle et l'objectif)
   en respectant scrupuleusement le format ASCII demandé. */
void afficherDuel(const EtatJeu* depart, const EtatJeu* arrivee);

/* Affiche les ordres disponibles */
void afficherOrdresPossibles(const ConfigJeu* config);
