#ifndef SYNCHRO_H
#define SYNCHRO_H

#include <stdbool.h>
#include "ensitheora.h"

extern bool window_fini;
extern bool texture_fini;
extern int texture_count;

/* Les extern des variables pour la synchro ici */
extern  pthread_mutex_t window_mutex;
extern  pthread_cond_t window_cond;

extern pthread_mutex_t texture_mutex;
extern pthread_cond_t texture_cond;

extern pthread_mutex_t prod_conso_mutex;
extern pthread_cond_t empty_cond;
extern pthread_cond_t full_cond;

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
