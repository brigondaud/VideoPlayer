#include "synchro.h"
#include "ensitheora.h"

bool window_fini;
bool texture_fini;

/* les variables pour la synchro, ici */
pthread_mutex_t window_mutex;
pthread_cond_t window_cond;

pthread_mutex_t texture_mutex;
pthread_cond_t texture_cond;

/* l'implantation des fonctions de synchro ici */
void envoiTailleFenetre(th_ycbcr_buffer buffer)
{
  // In the buffer the info are: buffer->width and buffer->height
  pthread_mutex_lock(&window_mutex);
  windowsx = buffer->width;
  windowsy = buffer->height;
  window_fini = true;
  
  pthread_cond_signal(&window_cond);
  pthread_mutex_unlock(&window_mutex);
}

void attendreTailleFenetre()
{
  pthread_mutex_lock(&window_mutex);
  while(!window_fini)
    pthread_cond_wait(&window_cond, &window_mutex);

  window_fini = false;
  pthread_mutex_unlock(&window_mutex);
}

void signalerFenetreEtTexturePrete()
{
  pthread_mutex_lock(&window_mutex);
  pthread_mutex_lock(&texture_mutex);
  window_fini = true;
  texture_fini = true;
  pthread_cond_signal(&texture_cond);
  pthread_cond_signal(&window_cond);
  pthread_mutex_unlock(&texture_mutex);
  pthread_mutex_unlock(&window_mutex);
}

void attendreFenetreTexture()
{
  pthread_mutex_lock(&texture_mutex);
  while(!texture_fini)
    pthread_cond_wait(&texture_cond, &texture_mutex);
  texture_fini = false;
  pthread_mutex_unlock(&texture_mutex);
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
