#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <SDL2/SDL.h>
#include <pthread.h>

#include "stream_common.h"
#include "oggstream.h"

int main(int argc, char *argv[])
{
	int res;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s FILE", argv[0]);
		exit(EXIT_FAILURE);
	}
	assert(argc == 2);

	// Initialisation de la SDL
	res = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);
	atexit(SDL_Quit);
	assert(res == 0);

    /*******************
     * MULTI-THREADING *
     *******************/
	before_threads();

	void *status;
	pthread_t theora_pid, vorbis_pid;

	// start the two stream readers
	pthread_create(&theora_pid, NULL, theoraStreamReader, (void *)argv[1]);
	pthread_create(&vorbis_pid, NULL, vorbisStreamReader, (void *)argv[1]);

	// wait audio thread
	pthread_join(vorbis_pid, &status);

	// 1 seconde de garde pour le son,
	sleep(1);

	// tuer les deux threads videos si ils sont bloqués
	pthread_cancel(theora_pid);
	pthread_cancel(theora2sdlthread);

	// attendre les 2 threads videos
	pthread_join(theora_pid, NULL);
	pthread_join(theora2sdlthread, NULL);

	after_threads();
	exit(EXIT_SUCCESS);
}


void before_threads()
{
	pthread_mutex_init(&mutex_hashmap, NULL);
	pthread_mutex_init(&mutex_draw2SDL, NULL);

	// Size of the window
	pthread_mutex_init(&window_mutex, NULL);
	pthread_cond_init(&window_cond, NULL);

	// Texture of the window
	pthread_mutex_init(&texture_mutex, NULL);
	pthread_cond_init(&texture_cond, NULL);

	// Producer-consumer
	pthread_mutex_init(&prod_conso_mutex, NULL);
	pthread_cond_init(&empty_cond, NULL);
	pthread_cond_init(&full_cond, NULL);

	// Guarantee vars
	window_fini = false;
	texture_fini = false;
	texture_count = 0;
}


void after_threads()
{
	// Producer-consumer
	pthread_cond_destroy(&full_cond);
	pthread_cond_destroy(&empty_cond);
	pthread_mutex_destroy(&prod_conso_mutex);

	// Texture of the window
	pthread_cond_destroy(&texture_cond);
	pthread_mutex_destroy(&texture_mutex);

	// Size of the window
	pthread_cond_destroy(&window_cond);
	pthread_mutex_destroy(&window_mutex);

	pthread_mutex_destroy(&mutex_draw2SDL);
	pthread_mutex_destroy(&mutex_hashmap);
}
