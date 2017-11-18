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
	if (!pthread_join(vorbis_pid, &status)) {
		perror("Error while waiting Vorbis: ");
		exit(EXIT_FAILURE);
	}

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
}


void after_threads()
{
	pthread_mutex_destroy(&mutex_draw2SDL);
	pthread_mutex_destroy(&mutex_hashmap);
}
