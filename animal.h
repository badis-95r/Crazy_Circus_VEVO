#pragma once

#include "liste.h" /* Nécessaire si on stockait une liste d'animaux, 
                      mais ici on va surtout gérer la configuration */

                      /*
                       * Fichier : animal.h
                       * Description : Gestion de la configuration du jeu (lecture du fichier crazy.cfg)
                       *               et des définitions liées aux animaux.
                       */

                       /* Constantes pour les limites */
#define MAX_ANIMAUX 10    /* Nombre maximum d'animaux supportés (arbitraire mais suffisant) */
#define MAX_ORDRES 10     /* Nombre maximum d'ordres configurables */
#define TAILLE_NOM 50     /* Taille max pour un nom d'animal */
#define DEF_FICHIER_CONFIG "crazy.cfg"

/* Structure représentant la configuration du jeu */
typedef struct {
    char* nomsAnimaux[MAX_ANIMAUX]; /* Tableau de chaînes pour les noms */
    int nbAnimaux;                  /* Nombre réel d'animaux lus */
    char* ordres[MAX_ORDRES];       /* Tableau de chaînes pour les ordres (KI, LO...) */
    int nbOrdres;                   /* Nombre réel d'ordres lus */
} ConfigJeu;

/* Prototypes */

/*
 * Lit le fichier de configuration et remplit la structure ConfigJeu.
 * Retourne 1 si tout est OK, 0 en cas d'erreur (fichier manquant, format invalide).
 */
int chargerConfiguration(ConfigJeu* config, const char* nomFichier);

/* Libère la mémoire allouée pour les noms d'animaux et ordres dans la config */
void libererConfiguration(ConfigJeu* config);

/* Affiche la config (pour debug ou info) */
void afficherConfiguration(const ConfigJeu* config);

/*
 * Vérifie si le fichier de config respecte les règles :
 * - Au moins 2 animaux
 * - Au moins 3 ordres valides
 */
int validerConfiguration(const ConfigJeu* config);
