#include "synchro.h"
#include "ensitheora.h"

bool fini;

/* les variables pour la synchro, ici */
pthread_mutex_t window_mutex;
pthread_cond_t window_cond;

/* l'implantation des fonctions de synchro ici */
void envoiTailleFenetre(th_ycbcr_buffer buffer)
{
  // In the buffer the info are: buffer->width and buffer->height
  pthread_mutex_lock(&window_mutex);
  windowsx = buffer->width;
  windowsy = buffer->height;
  fini = true;
  pthread_cond_signal(&window_cond);
  pthread_mutex_unlock(&window_mutex);
}

void attendreTailleFenetre()
{
  pthread_mutex_lock(&window_mutex);
  while(!fini)
    pthread_cond_wait(&window_cond, &window_mutex);
  fini = false;
  pthread_mutex_unlock(&window_mutex);
}

void signalerFenetreEtTexturePrete()
{
}

void attendreFenetreTexture()
{
}

void debutConsommerTexture()
{
}

void finConsommerTexture()
{
}

void debutDeposerTexture()
{
}

void finDeposerTexture()
{
}
