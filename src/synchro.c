#include "synchro.h"
#include "ensitheora.h"

/* Mettre à 1 pour une trace des synchronisations */
#define DEBUG_THREADS 0

/* les variables pour la synchro, ici */
bool window_fini;
bool texture_fini;

// Size of the window
pthread_mutex_t window_mutex;
pthread_cond_t window_cond;

// Texture of the window
pthread_mutex_t texture_mutex;
pthread_cond_t texture_cond;

/* Variables pour le producteur consommateur */
int texture_count;

// Sharing textures
pthread_mutex_t prod_conso_mutex;
pthread_cond_t empty_cond;
pthread_cond_t full_cond;

/* Switch DEBUG_THREADS to 1 to get a trace. */
void debugprintf(FILE *, const char *);


/* l'implantation des fonctions de synchro ici */
void envoiTailleFenetre(th_ycbcr_buffer buffer)
{
	debugprintf(stderr, "~. Envoi taille fenêtre, avant LOCK.\n");
	pthread_mutex_lock(&window_mutex);	// LOCK
	debugprintf(stderr, "!. Envoi taille fenêtre, après LOCK.\n");

	// Set window settings
	windowsx = buffer->width;
	windowsy = buffer->height;
	window_fini = true;

	pthread_cond_signal(&window_cond);

	debugprintf(stderr, "!. Envoi taille fenêtre, avant UNLOCK.\n");
	pthread_mutex_unlock(&window_mutex);	// UNLOCK
	debugprintf(stderr, "+. Envoi taille fenêtre, après UNLOCK.\n");
}


void attendreTailleFenetre()
{
	debugprintf(stderr, "~. Attendre taille fenêtre, avant LOCK.\n");
	pthread_mutex_lock(&window_mutex);	// LOCK
	debugprintf(stderr, "!. Attendre taille fenêtre, après LOCK.\n");

	while (!window_fini)
		pthread_cond_wait(&window_cond, &window_mutex);

	debugprintf(stderr, "!. Attendre taille fenêtre, avant UNLOCK.\n");
	pthread_mutex_unlock(&window_mutex);	// UNLOCK
	debugprintf(stderr, "+. Attendre taille fenêtre, après UNLOCK.\n");
}


void signalerFenetreEtTexturePrete()
{
	debugprintf(stderr, "~. Signaler fenêtre et textures prêtes, avant LOCK.\n");
	pthread_mutex_lock(&texture_mutex);	// LOCK
	debugprintf(stderr, "!. Signaler fenêtre et textures prêtes, après LOCK.\n");

	texture_fini = true;
	pthread_cond_signal(&texture_cond);

	debugprintf(stderr, "!. Signaler fenêtre et textures prêtes, avant UNLOCK.\n");
	pthread_mutex_unlock(&texture_mutex);	// UNLOCK
	debugprintf(stderr, "+. Signaler fenêtre et textures prêtes, après UNLOCK.\n");
}


void attendreFenetreTexture()
{
	debugprintf(stderr, "~. Attendre fenêtre texture, avant LOCK.\n");
	pthread_mutex_lock(&texture_mutex);	// LOCK
	debugprintf(stderr, "!. Attendre fenêtre texture, après LOCK.\n");

	while (!texture_fini) {
		debugprintf(stderr, "B. Bloqué dans texture fini.\n");
		pthread_cond_wait(&texture_cond, &texture_mutex);
	}

	debugprintf(stderr, "!. Attendre fenêtre texture, avant UNLOCK.\n");
	pthread_mutex_unlock(&texture_mutex);	// UNLOCK
	debugprintf(stderr, "+. Attendre fenêtre texture, après UNLOCK.\n");
}


void debutConsommerTexture()
{
	debugprintf(stderr, "~. Début consommer texture, avant LOCK.\n");
	pthread_mutex_lock(&prod_conso_mutex);	// LOCK
	debugprintf(stderr, "!. Début consommer texture, après LOCK.\n");

	while (texture_count == 0) {
		debugprintf(stderr, "B. Bloqué dans début consommer texture.\n");
		pthread_cond_wait(&empty_cond, &prod_conso_mutex);
	}

	pthread_mutex_unlock(&prod_conso_mutex);	// UNLOCK
	debugprintf(stderr, ">. Début consommer texture, on ENTRE.\n");
}


void finConsommerTexture()
{
	pthread_mutex_lock(&prod_conso_mutex);	// LOCK
	debugprintf(stderr, "<. Fin consommer texture, on SORT.\n");

	texture_count--;
	pthread_cond_signal(&full_cond);

	debugprintf(stderr, "!. Fin consommer texture, avant UNLOCK.\n");
	pthread_mutex_unlock(&prod_conso_mutex);	// UNLOCK
	debugprintf(stderr, "+. Fin consommer texture, après UNLOCK.\n");
}


void debutDeposerTexture()
{
	debugprintf(stderr, "~. Début déposer texture, avant LOCK.\n");
	pthread_mutex_lock(&prod_conso_mutex);	// LOCK
	debugprintf(stderr, "!. Début déposer texture, après LOCK.\n");

	while (texture_count == NBTEX) {
		debugprintf(stderr, "B. Bloqué dans début déposer texture.\n");
		pthread_cond_wait(&full_cond, &prod_conso_mutex);
	}

	pthread_mutex_unlock(&prod_conso_mutex);	// UNLOCK
	debugprintf(stderr, ">. Début produire texture, on ENTRE.\n");
}


void finDeposerTexture()
{
	pthread_mutex_lock(&prod_conso_mutex);	// LOCK
	debugprintf(stderr, "<. Fin déposer texture, on SORT.\n");

	texture_count++;
	pthread_cond_signal(&empty_cond);

	debugprintf(stderr, "!. Fin déposer texture, avant UNLOCK.\n");
	pthread_mutex_unlock(&prod_conso_mutex);	// UNLOCK
	debugprintf(stderr, "+. Fin déposer texture, après UNLOCK.\n");
}


/**
 * Monitoring tools
 */
void debugprintf(FILE *stream, const char *str) {
	if (DEBUG_THREADS)
		fprintf(stream, "%s", str);
}
