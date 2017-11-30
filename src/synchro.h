#ifndef SYNCHRO_H
#define SYNCHRO_H

#include <stdbool.h>
#include "ensitheora.h"

extern bool fini;

/* Les extern des variables pour la synchro ici */
extern  pthread_mutex_t window_mutex;
extern  pthread_cond_t window_cond;

extern pthread_mutex_t texture_mutex;
extern pthread_cond_t texture_cond;

/* Fonctions de synchro à implanter */

void envoiTailleFenetre(th_ycbcr_buffer buffer);
void attendreTailleFenetre();

void attendreFenetreTexture();
void signalerFenetreEtTexturePrete();

void debutConsommerTexture();
void finConsommerTexture();

void debutDeposerTexture();
void finDeposerTexture();

#endif
