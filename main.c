#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "jeu.h"

/*
 * Fichier : main.c
 * Description : Programme principal du jeu Crazy Circus.
 *               Gère l'initialisation, la boucle de jeu, les scores et les interactions.
 */

#define MAX_JOUEURS 10
#define LEN_CMD 100

typedef struct {
    char* nom;
    int score;
} Joueur;

/* Compare pour qsort (tri décroissant score, puis alphabétique nom) */
int compareJoueurs(const void* a, const void* b) {
    const Joueur* j1 = (const Joueur*)a;
    const Joueur* j2 = (const Joueur*)b;

    if (j1->score != j2->score) {
        return j2->score - j1->score; /* Décroissant */
    }
    return strcmp(j1->nom, j2->nom); /* Alphabétique croissant */
}

/* Helper pour lire une chaine proprement */
static void lireChaine(char* buffer, int taille) {
    if (fgets(buffer, taille, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
    }
}

int main(int argc, char* argv[]) {
    /* Initialisation des joueurs */
    Joueur joueurs[MAX_JOUEURS];
    int nbJoueurs = 0;

    /* Allocation dynamique pour les noms en mode interactif */
    char* nomsAlloues[MAX_JOUEURS] = { NULL };
    char fichierConfig[256] = DEF_FICHIER_CONFIG;

    /* 1. Gestion des Joueurs (Arguments ou Interactif) */
    if (argc >= 3) {
        /* Mode Arguments Ligne de Commande */
        for (int i = 1; i < argc; i++) {
            if (nbJoueurs >= MAX_JOUEURS) break;

            /* Check doublon */
            int existe = 0;
            for (int k = 0; k < nbJoueurs; k++) {
                if (strcmp(joueurs[k].nom, argv[i]) == 0) {
                    existe = 1;
                    break;
                }
            }
            if (existe) {
                printf("Erreur: Les noms des joueurs doivent etre distincts (%s).\n", argv[i]);
                return EXIT_FAILURE;
            }

            joueurs[nbJoueurs].nom = argv[i];
            joueurs[nbJoueurs].score = 0;
            nbJoueurs++;
        }
    }
    else {
        /* Mode Interactif (Pour Visual Studio / Lancement double-clic) */
        printf("--- Mode Interactif ---\n");

        /* Demande du fichier de configuration */
        printf("Entrez le nom du fichier de configuration (Defaut: %s) : ", DEF_FICHIER_CONFIG);
        char buffer[100];
        lireChaine(buffer, sizeof(buffer));
        if (strlen(buffer) > 0) {
            strncpy(fichierConfig, buffer, sizeof(fichierConfig) - 1);
            fichierConfig[sizeof(fichierConfig) - 1] = '\0';
        }

        printf("Entrez le nombre de joueurs (min 2, max %d) : ", MAX_JOUEURS);
        lireChaine(buffer, sizeof(buffer));
        int n = atoi(buffer);

        if (n < 2) {
            printf("Il faut au moins 2 joueurs.\n");
            /* Pause pour lire avant fermeture */
            printf("Appuyez sur Entree pour quitter...");
            getchar();
            return EXIT_FAILURE;
        }
        if (n > MAX_JOUEURS) n = MAX_JOUEURS;

        for (int i = 0; i < n; i++) {
            printf("Nom du joueur %d : ", i + 1);
            lireChaine(buffer, sizeof(buffer));
            if (strlen(buffer) == 0) {
                printf("Nom vide invalide.\n");
                i--; continue;
            }

            /* Check doublon */
            int existe = 0;
            for (int k = 0; k < nbJoueurs; k++) {
                if (strcmp(joueurs[k].nom, buffer) == 0) {
                    existe = 1;
                    break;
                }
            }
            if (existe) {
                printf("Ce nom est deja pris.\n");
                i--; continue;
            }

            /* Allocation et copie */
            nomsAlloues[nbJoueurs] = (char*)malloc(strlen(buffer) + 1);
            strcpy(nomsAlloues[nbJoueurs], buffer);
            joueurs[nbJoueurs].nom = nomsAlloues[nbJoueurs];
            joueurs[nbJoueurs].score = 0;
            nbJoueurs++;
        }
    }

    /* 2. Chargement de la config */
    ConfigJeu config;
    if (!chargerConfiguration(&config, fichierConfig)) {
        /* En mode interactif, on attend avant de fermer pour voir l'erreur */
        if (argc < 3) {
            printf("Appuyez sur Entree pour quitter...");
            getchar();
        }
        return EXIT_FAILURE;
    }
    if (!validerConfiguration(&config)) {
        libererConfiguration(&config);
        return EXIT_FAILURE;
    }

    /* 3. Préparation du jeu */
    srand(time(NULL)); /* Init aléatoire */

    Deck deck;
    genererToutesPositions(&config, &deck);
    /* On pourrait mélanger le deck ici ou juste tirer au hasard via estUtilisee (ce que fait tirerNouvelleCarte) */

    afficherOrdresPossibles(&config);

    /* Etat courant et Etat objectif */
    EtatJeu* courant = tirerNouvelleCarte(&deck); /* Position de départ aléatoire */
    EtatJeu* objectif = tirerNouvelleCarte(&deck); /* Premier objectif */

    /* Tableau pour suivre qui peut encore jouer ce tour ci */
    int* peutJouer = (int*)malloc(sizeof(int) * nbJoueurs);
    for (int i = 0; i < nbJoueurs; i++) peutJouer[i] = 1;
    int nbJoueursEnLice = nbJoueurs;

    /* Boucle principale */
    while (objectif != NULL) {
        /* Affiche la situation */
        afficherDuel(courant, objectif);

        /* Boucle de saisie pour un tour */
        int tourTermine = 0;
        while (!tourTermine) {
            char buffer[256];
            /* Lecture stdin */
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                /* Fin de flux (Ctrl+D) */
                tourTermine = 1;
                objectif = NULL; /* Force fin de partie */
                break;
            }

            /* Nettoyage du \n */
            buffer[strcspn(buffer, "\n")] = 0;
            if (strlen(buffer) == 0) continue;

            /* Parsing : "NOM SEQUENCE" */
            char* nomJoueur = strtok(buffer, " ");
            char* sequence = strtok(NULL, " ");

            /* Trouver le joueur */
            int idJoueur = -1;
            for (int i = 0; i < nbJoueurs; i++) {
                if (strcmp(joueurs[i].nom, nomJoueur) == 0) {
                    idJoueur = i;
                    break;
                }
            }

            if (idJoueur == -1) {
                printf("Joueur inconnu\n");
                continue;
            }

            /* Vérifie s'il a le droit de jouer */
            if (!peutJouer[idJoueur]) {
                printf("%s ne peut pas jouer\n", nomJoueur);
                continue;
            }

            if (sequence == NULL) {
                printf("Sequence manquante\n");
                continue;
            }

            /* Vérifie si l'ordre existe dans la config ?
               Le sujet dit "Si l'ordre LI n'existe pas -> message".
               Notre executerSequence renvoie 0 si commande inconnue.
            */

            /* Test de la séquence sur une copie de l'état courant */
            EtatJeu testState;
            initEtat(&testState);
            copierEtat(courant, &testState);

            int codeRetour = executerSequence(&testState, sequence);

            /* Vérification spéciale : Trouver le premier ordre inconnu si codeRetour == -1 */
            if (codeRetour == -1) {
                /* On doit trouver quel ordre est invalide pour l'afficher */
                char ordreFaux[3] = "??";
                for (size_t i = 0; i < strlen(sequence); i += 2) {
                    char sub[3]; sub[0] = sequence[i]; sub[1] = sequence[i + 1]; sub[2] = '\0';
                    if (strcmp(sub, "KI") != 0 && strcmp(sub, "LO") != 0 &&
                        strcmp(sub, "SO") != 0 && strcmp(sub, "NI") != 0 &&
                        strcmp(sub, "MA") != 0) {
                        strcpy(ordreFaux, sub);
                        break;
                    }
                }
                printf("L'ordre %s n'existe pas\n", ordreFaux);
            }

            int bonneSolution = 0;
            if (codeRetour == 1) {
                if (estMemeEtat(&testState, objectif)) {
                    bonneSolution = 1;
                }
            }

            libererEtat(&testState); /* Nettoyage copie */

            if (bonneSolution) {
                printf("%s gagne un point\n\n", nomJoueur);
                joueurs[idJoueur].score++;
                tourTermine = 1;
            }
            else {
                /* Si ce n'était pas un ordre inconnu, on affiche le message standard d'erreur */
                if (codeRetour != -1) {
                    if (codeRetour == 0) {
                        /* Mouvement impossible (ex: pile vide) ou syntaxe impaire */
                        printf("La sequence ne conduit pas a la situation attendue -- %s ne peut plus jouer durant ce tour\n", nomJoueur);
                    }
                    else {
                        /* Code 1 mais pas bon etat */
                        printf("La sequence ne conduit pas a la situation attendue -- %s ne peut plus jouer durant ce tour\n", nomJoueur);
                    }
                }
                else {
                    /* On a deja affiché "L'ordre XX n'existe pas", mais faut-il ajouter que le joueur est éliminé ?
                       Le sujet montre :
                       MZ KILI
                       L'ordre LI n'existe pas
                       (MZ rejoue ensuite KIKI -> donc il n'est PAS éliminé ?)

                       Attends, l'annexe montre :
                       MZ KILI
                       L'ordre LI n'existe pas
                       MZ KIKI
                       La sequence ne conduit pas...

                       Ah ! Donc une commande inconnue N'ÉLIMINE PAS le joueur !
                       Contrairement à une mauvaise séquence qui l'élimine ("MZ ne peut plus jouer durant ce tour").
                    */
                }

                if (codeRetour != -1) {
                    peutJouer[idJoueur] = 0;
                    nbJoueursEnLice--;
                }

                /* Si plus qu'un seul joueur */
                if (nbJoueursEnLice == 1) {
                    /* Retrouver le survivant */
                    int survivant = -1;
                    for (int i = 0; i < nbJoueurs; i++) {
                        if (peutJouer[i]) {
                            survivant = i;
                            break;
                        }
                    }
                    /* Il gagne par forfait */
                    printf("%s gagne un point car lui seul peut encore jouer durant ce tour\n\n", joueurs[survivant].nom);
                    joueurs[survivant].score++;
                    tourTermine = 1;
                }
                else if (nbJoueursEnLice == 0) {
                    /* Cas rare : tout le monde a perdu.
                       Le sujet dit : "Si durant un tour, il n'y a plus qu'un joueur a ne pas avoir proposer de sequence, celui-ci remporte le tour."
                       Il ne précise pas si tout le monde se trompe (ex: 2 joueurs, les 2 se trompent).
                       Supposons que personne ne marque si tout le monde est éliminé (mais avec la condition nbEnLice==1, le dernier gagne avant de jouer).
                       Sauf si le dernier joue et se trompe aussi ?
                       Avec ma logique, dès qu'il ne reste qu'un joueur (après l'erreur de l'avant-dernier), il gagne direct.
                       Donc nbJoueursEnLice ne peut pas atteindre 0 sauf s'il n'y avait qu'un joueur au départ (impossible par config).
                    */
                }
            }
        } /* Fin boucle tour */

        /* Préparation tour suivant */
        if (objectif != NULL) {
            /* L'objectif atteint devient le nouveau départ */
            /* Attention : 'courant' pointe vers une case du tableau deck.
               'objectif' pointe vers une autre case.
               On doit juste changer les pointeurs.
            */
            courant = objectif;

            /* Tirer nouvel objectif */
            objectif = tirerNouvelleCarte(&deck);

            /* Reset joueurs */
            for (int i = 0; i < nbJoueurs; i++) peutJouer[i] = 1;
            nbJoueursEnLice = nbJoueurs;
        }

    } /* Fin boucle partie */

    /* Fin de partie - Affichage scores */
    /* Tri des joueurs */
    qsort(joueurs, nbJoueurs, sizeof(Joueur), compareJoueurs);

    for (int i = 0; i < nbJoueurs; i++) {
        printf("%s %d\n", joueurs[i].nom, joueurs[i].score);
    }

    /* Nettoyage */
    free(peutJouer);
    /* Libération du deck */
    /* Les états dans deck->positions contiennent des piles qu'il faut vider */
    for (int i = 0; i < deck.nbPositions; i++) {
        libererEtat(&deck.positions[i]);
    }
    free(deck.positions);
    free(deck.estUtilisee);

    libererConfiguration(&config);

    return EXIT_SUCCESS;
}
