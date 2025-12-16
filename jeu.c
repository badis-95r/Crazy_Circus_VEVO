#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "jeu.h"

/*
 * Fichier : jeu.c
 * Description : Implémentation de la logique de jeu.
 */

 /* --- Fonctions de base sur l'Etat --- */

void initEtat(EtatJeu* e) {
    initPile(&e->podiumBleu);
    initPile(&e->podiumRouge);
}

void libererEtat(EtatJeu* e) {
    detruirePile(&e->podiumBleu);
    detruirePile(&e->podiumRouge);
}

void copierEtat(const EtatJeu* src, EtatJeu* dest) {
    /* On suppose dest déjà initialisé ou propre, mais par sécurité on vide */
    detruirePile(&dest->podiumBleu);
    detruirePile(&dest->podiumRouge);
    initPile(&dest->podiumBleu);
    initPile(&dest->podiumRouge);

    /* Pour copier une pile sans accès direct aux maillons (respect du TAD),
       on utilise une pile temporaire pour inverser l'ordre deux fois.

       Cependant, comme nous avons accès à la structure interne Liste via "liste.h" inclus indirectement
       et que Pile est un typedef de Liste, nous pouvons tricher un peu pour l'efficacité
       OU faire une copie propre via itération.

       Restons scolaires et respectons l'abstraction Pile le plus possible.
       Mais comme Pile ne donne pas d'itérateur...
       On va supposer qu'on peut accéder à la liste sous-jacente car nous sommes les concepteurs.
       La façon la plus simple ici est de copier la liste chaînée élément par élément.
    */

    /* Méthode : On copie la liste manuellement car Pile = Liste ici */
    /* Copie Bleu */
    Cellule* courant = src->podiumBleu.tete;
    /* Pour garder l'ordre, il faut empiler à l'envers ou insérer en queue.
       Mais notre liste n'a que insererTete.
       Donc on peut lire la source du début à la fin, stocker dans un tableau temporaire,
       puis empiler depuis la fin du tableau pour retrouver l'ordre.
    */

    /* Approche tableau temporaire */
    char* temp[100];
    int n = 0;
    while (courant != NULL) {
        temp[n++] = courant->valeur;
        courant = courant->suivant;
    }
    /* On empile depuis la fin du tableau (le bas de la pile source) vers le début */
    for (int i = n - 1; i >= 0; i--) {
        empiler(&dest->podiumBleu, temp[i]);
    }

    /* Copie Rouge (idem) */
    courant = src->podiumRouge.tete;
    n = 0;
    while (courant != NULL) {
        temp[n++] = courant->valeur;
        courant = courant->suivant;
    }
    for (int i = n - 1; i >= 0; i--) {
        empiler(&dest->podiumRouge, temp[i]);
    }
}

int estMemeEtat(const EtatJeu* e1, const EtatJeu* e2) {
    /* Comparaison structurelle des deux piles */
    if (taillePile(&e1->podiumBleu) != taillePile(&e2->podiumBleu)) return 0;
    if (taillePile(&e1->podiumRouge) != taillePile(&e2->podiumRouge)) return 0;

    Cellule* c1 = e1->podiumBleu.tete;
    Cellule* c2 = e2->podiumBleu.tete;
    while (c1 != NULL) {
        if (strcmp(c1->valeur, c2->valeur) != 0) return 0;
        c1 = c1->suivant;
        c2 = c2->suivant;
    }

    c1 = e1->podiumRouge.tete;
    c2 = e2->podiumRouge.tete;
    while (c1 != NULL) {
        if (strcmp(c1->valeur, c2->valeur) != 0) return 0;
        c1 = c1->suivant;
        c2 = c2->suivant;
    }

    return 1;
}

/* --- Commandes --- */

int commandeKI(EtatJeu* e) {
    /* KI : Sommet Bleu -> Sommet Rouge */
    if (pileEstVide(&e->podiumBleu)) return 0;
    empiler(&e->podiumRouge, depiler(&e->podiumBleu));
    return 1;
}

int commandeLO(EtatJeu* e) {
    /* LO : Sommet Rouge -> Sommet Bleu */
    if (pileEstVide(&e->podiumRouge)) return 0;
    empiler(&e->podiumBleu, depiler(&e->podiumRouge));
    return 1;
}

int commandeSO(EtatJeu* e) {
    /* SO : Echange les deux sommets */
    if (pileEstVide(&e->podiumBleu) || pileEstVide(&e->podiumRouge)) return 0;

    char* valBleu = depiler(&e->podiumBleu);
    char* valRouge = depiler(&e->podiumRouge);

    empiler(&e->podiumBleu, valRouge);
    empiler(&e->podiumRouge, valBleu);
    return 1;
}

/* Helper pour NI et MA : Rotation (Bas vers Haut) */
static int rotationBasVersHaut(Pile* p) {
    if (pileEstVide(p)) return 0;
    if (taillePile(p) == 1) return 1; /* Rien à faire mais valide */

    /* On doit récupérer le bas de la pile.
       Comme c'est une pile, on doit tout dépiler pour l'atteindre,
       puis tout rempiler sauf lui, et enfin l'empiler au sommet. */

    Pile temp;
    initPile(&temp);

    /* 1. On vide p dans temp jusqu'au dernier élément */
    while (taillePile(p) > 1) {
        empiler(&temp, depiler(p));
    }

    /* 2. Le dernier élément restant est celui du bas */
    char* bas = depiler(p);

    /* 3. On remet tout de temp vers p */
    while (!pileEstVide(&temp)) {
        empiler(p, depiler(&temp));
    }

    /* 4. On met l'élément du bas au sommet */
    empiler(p, bas);

    detruirePile(&temp);
    return 1;
}

int commandeNI(EtatJeu* e) {
    /* NI : Bas Bleu -> Sommet Bleu */
    return rotationBasVersHaut(&e->podiumBleu);
}

int commandeMA(EtatJeu* e) {
    /* MA : Bas Rouge -> Sommet Rouge */
    return rotationBasVersHaut(&e->podiumRouge);
}

int executerSequence(EtatJeu* e, const char* seq) {
    /* On itère 2 char par 2 char (KI, LO...) */
    int len = strlen(seq);
    /* Syntaxe invalide si longueur impaire */
    if (len % 2 != 0) return 0;

    for (int i = 0; i < len; i += 2) {
        char cmd[3];
        cmd[0] = seq[i];
        cmd[1] = seq[i + 1];
        cmd[2] = '\0';

        /* Vérification si c'est une commande connue */
        if (strcmp(cmd, "KI") != 0 && strcmp(cmd, "LO") != 0 &&
            strcmp(cmd, "SO") != 0 && strcmp(cmd, "NI") != 0 &&
            strcmp(cmd, "MA") != 0) {
            /* Commande inconnue -> On imprime laquelle pour l'affichage d'erreur dans le main ?
               Non, executerSequence doit renvoyer un code.
               Code -1 : Ordre inconnu */
            return -1;
        }

        int res = 0;
        if (strcmp(cmd, "KI") == 0) res = commandeKI(e);
        else if (strcmp(cmd, "LO") == 0) res = commandeLO(e);
        else if (strcmp(cmd, "SO") == 0) res = commandeSO(e);
        else if (strcmp(cmd, "NI") == 0) res = commandeNI(e);
        else if (strcmp(cmd, "MA") == 0) res = commandeMA(e);

        if (res == 0) return 0; /* Mouvement impossible */
    }

    return 1;
}

/* --- Algorithme de Heap (Génération des permutations) --- */

/*
   L'algo de Heap génère toutes les permutations d'un tableau.
   Mais ici, en plus de l'ordre des animaux (permutation),
   il faut gérer leur répartition entre les deux piles (la "coupure").

   Si on a N animaux, pour chaque permutation P :
   On peut couper P à l'index 0 (tous sur Rouge), 1, ..., N (tous sur Bleu).
   Cela donne N+1 répartitions par permutation.
*/

static void enregistrerConfiguration(Deck* deck, char** animaux, int n, int coupure) {
    /* Crée un état basé sur le tableau animaux et l'indice de coupure */
    /* Les animaux de 0 à coupure-1 vont sur Bleu */
    /* Les animaux de coupure à n-1 vont sur Rouge */
    /* Attention à l'ordre d'empilement !
       Si le tableau est [A, B, C] et coupure=2 (A,B sur bleu).
       Est-ce que A est au fond ou au sommet ?
       Le sujet montre "OURS" au dessus de "LION".
       Disons que l'index 0 est le fond de la pile Bleu, etc.
       C'est une convention interne, tant qu'elle est cohérente partout.
    */

    /* On suppose que l'allocation a été faite correctement avec la taille exacte calculée avant */

    EtatJeu* nouv = &deck->positions[deck->nbPositions];
    initEtat(nouv);

    /* Remplissage Bleu (0 à coupure-1) */
    for (int i = 0; i < coupure; i++) {
        empiler(&nouv->podiumBleu, animaux[i]);
        /* Si on empile i=0 puis i=1, i=1 sera au dessus.
           Donc index 0 = Fond. C'est logique. */
    }

    /* Remplissage Rouge (coupure à n-1) */
    for (int i = coupure; i < n; i++) {
        empiler(&nouv->podiumRouge, animaux[i]);
    }

    deck->nbPositions++;
}

/* Fonction utilitaire pour calculer la factorielle */
static int factorielle(int n) {
    if (n <= 1) return 1;
    int res = 1;
    for (int i = 2; i <= n; i++) res *= i;
    return res;
}

/* Implémentation itérative de Heap */
void genererToutesPositions(const ConfigJeu* config, Deck* deck) {
    /* Calcul de la capacité nécessaire : N! * (N+1) */
    int n = config->nbAnimaux;
    int factN = factorielle(n);
    int capaciteMax = factN * (n + 1);

    /* Allocation du deck */
    deck->positions = (EtatJeu*)malloc(sizeof(EtatJeu) * capaciteMax);
    if (deck->positions == NULL) {
        fprintf(stderr, "Erreur fatale : Plus de mémoire pour générer les positions (besoin de %d états)\n", capaciteMax);
        exit(EXIT_FAILURE);
    }
    deck->nbPositions = 0;

    /* Copie locale des animaux pour permuter */
    /* n est deja defini au debut de la fonction */
    char* A[MAX_ANIMAUX];
    for (int i = 0; i < n; i++) A[i] = config->nomsAnimaux[i];

    /* Tableau c pour l'algo de Heap */
    int c[MAX_ANIMAUX];
    for (int i = 0; i < n; i++) c[i] = 0;

    /* 1. Enregistrer la perm initiales (toutes les coupures possibles) */
    for (int k = 0; k <= n; k++) enregistrerConfiguration(deck, A, n, k);

    /* 2. Boucle de Heap */
    int i = 0;
    while (i < n) {
        if (c[i] < i) {
            if (i % 2 == 0) {
                /* Swap A[0] et A[i] */
                char* tmp = A[0]; A[0] = A[i]; A[i] = tmp;
            }
            else {
                /* Swap A[c[i]] et A[i] */
                char* tmp = A[c[i]]; A[c[i]] = A[i]; A[i] = tmp;
            }

            /* Enregistrer cette nouvelle permutation (avec toutes ses coupures) */
            for (int k = 0; k <= n; k++) enregistrerConfiguration(deck, A, n, k);

            c[i] += 1;
            i = 0;
        }
        else {
            c[i] = 0;
            i += 1;
        }
    }

    /* Init du tableau d'utilisation */
    deck->estUtilisee = (int*)calloc(deck->nbPositions, sizeof(int));
}

EtatJeu* tirerNouvelleCarte(Deck* deck) {
    /* Compte combien il reste de dispo */
    int dispo = 0;
    for (int i = 0; i < deck->nbPositions; i++) {
        if (!deck->estUtilisee[i]) dispo++;
    }

    if (dispo == 0) return NULL;

    /* On choisit le k-ième disponible aléatoirement */
    int choix = rand() % dispo;
    int compteur = 0;
    for (int i = 0; i < deck->nbPositions; i++) {
        if (!deck->estUtilisee[i]) {
            if (compteur == choix) {
                deck->estUtilisee[i] = 1;
                return &deck->positions[i];
            }
            compteur++;
        }
    }
    return NULL;
}

/* --- Affichage --- */

void afficherOrdresPossibles(const ConfigJeu* config) {
    /* KI (B -> R) | LO (B <- R) | SO (B <-> R) | NI (B ^) | MA (R ^) */
    /* On ne doit afficher QUE ceux présents dans la config */
    for (int i = 0; i < config->nbOrdres; i++) {
        char* o = config->ordres[i];
        printf("%s ", o);
        if (strcmp(o, "KI") == 0) printf("(B -> R)");
        else if (strcmp(o, "LO") == 0) printf("(B <- R)");
        else if (strcmp(o, "SO") == 0) printf("(B <-> R)");
        else if (strcmp(o, "NI") == 0) printf("(B ^)");
        else if (strcmp(o, "MA") == 0) printf("(R ^)");

        if (i < config->nbOrdres - 1) printf(" | ");
    }
    printf("\n\n");
}

/* Helper pour convertir une pile en tableau pour l'affichage (index 0 = sommet) */
static int pileVersTableau(const Pile* p, char* tab[]) {
    int h = 0;
    Cellule* c = p->tete;
    while (c != NULL) {
        tab[h++] = c->valeur;
        c = c->suivant;
    }
    return h;
}

void afficherDuel(const EtatJeu* depart, const EtatJeu* arrivee) {
    /*
    Format:
    OURS                       LION
    LION   ELEPHANT            ELEPHANT     OURS
    ----   ----         ==>    ----         ----
    BLEU   ROUGE               BLEU         ROUGE
    */

    /* Récupération des contenus sous forme de tableaux (0 = sommet) */
    char* b1[MAX_ANIMAUX]; int hB1 = pileVersTableau(&depart->podiumBleu, b1);
    char* r1[MAX_ANIMAUX]; int hR1 = pileVersTableau(&depart->podiumRouge, r1);

    char* b2[MAX_ANIMAUX]; int hB2 = pileVersTableau(&arrivee->podiumBleu, b2);
    char* r2[MAX_ANIMAUX]; int hR2 = pileVersTableau(&arrivee->podiumRouge, r2);

    /* Calcul de la hauteur max totale pour savoir combien de lignes afficher */
    int maxH = 0;
    if (hB1 > maxH) maxH = hB1;
    if (hR1 > maxH) maxH = hR1;
    if (hB2 > maxH) maxH = hB2;
    if (hR2 > maxH) maxH = hR2;

    /* Largeur de colonne fixe ou dynamique ?
       Le sujet montre un alignement propre.
       "LION   ELEPHANT" -> il y a des espaces.
       On va définir une largeur de colonne standard, disons 10 chars, ou la longueur max.
    */
    int colW = 12;

    /* Affichage ligne par ligne, du haut vers le bas */
    /* Attention : les tableaux ont l'indice 0 au sommet.
       Mais on doit afficher en partant du "ciel" (maxH - 1) jusqu'à 0 ?
       Non, si maxH est 3, la ligne la plus haute est la ligne correspondant à l'étage 3 (si existe).
       L'étage 0 est le bas (sur le podium).

       Attends, `pileVersTableau` met tete (sommet) en 0.
       Donc tab[0] est l'élément le plus haut.
       Pour afficher correctement :
       Ligne 0 (Sommet absolu) : tab[0] si h==maxH ? Non.

       Prenons l'exemple :
       Bleu: OURS(0,Sommet), LION(1,Bas). h=2.
       Rouge: ELEPHANT(0,Sommet). h=1.
       MaxH = 2.

       Ligne d'affichage 0 (Haut) : Doit afficher OURS (Bleu) et RIEN (Rouge).
       Ligne d'affichage 1 (Bas)  : Doit afficher LION (Bleu) et ELEPHANT (Rouge).

       L'élément à l'étage 'k' (en partant du bas 0) se trouve à l'index `h - 1 - k` dans le tableau ?
       Ex: Etage 1 (Haut) : index 2 - 1 - 1 = 0 (OURS). OK.
       Ex: Etage 0 (Bas)  : index 2 - 1 - 0 = 1 (LION). OK.
    */

    for (int k = maxH - 1; k >= 0; k--) {
        /* Partie Gauche (Départ) */

        /* Bleu */
        if (k < hB1) printf("%-*s", colW, b1[hB1 - 1 - k]);
        else printf("%-*s", colW, "");

        /* Rouge */
        if (k < hR1) printf("%-*s", colW, r1[hR1 - 1 - k]);
        else printf("%-*s", colW, "");

        /* Séparateur central */
        /* On met des espaces vides entre les deux podiums pour les animaux */
        printf("          ");

        /* Partie Droite (Arrivée) */

        /* Bleu */
        if (k < hB2) printf("%-*s", colW, b2[hB2 - 1 - k]);
        else printf("%-*s", colW, "");

        /* Rouge */
        if (k < hR2) printf("%s", r2[hR2 - 1 - k]);
        /* Pas de padding nécessaire à la toute fin de ligne */

        printf("\n");
    }

    /* Socles */
    /* ----   ----         ==>    ----         ---- */
    /* La flèche est bien au niveau des socles dans l'exemple du sujet ! */
    /* Ah non, regardons l'exemple :
       LION   ELEPHANT            ELEPHANT     OURS
       ----   ----         ==>    ----         ----
    */
    /* Ma logique précédente mettait la flèche au dessus des socles si k==0.
       On va corriger : Flèche sur la ligne des socles. */

    printf("%-*s%-*s   ==>    %-*s%-*s\n", colW, "----", colW, "----", colW, "----", colW, "----");
    printf("%-*s%-*s          %-*s%-*s\n", colW, "BLEU", colW, "ROUGE", colW, "BLEU", colW, "ROUGE");
    printf("\n");
}
