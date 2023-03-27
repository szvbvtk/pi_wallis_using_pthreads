#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

long double suma = 1.0;

struct arg{
    int liczba_wyrazow;
    int indeks_pierwszego_wyrazu;
};


void *thread(void *dane){
    struct arg *argumenty = (struct arg *)dane;

    int liczba_wyrazow = argumenty->liczba_wyrazow;
    int indeks_wyrazu = argumenty->indeks_pierwszego_wyrazu;
    double suma_tmp = 1;
    pthread_t tid = pthread_self();

    fprintf(stdout, "Thread #%lu size=%d first=%d\n", tid, liczba_wyrazow, indeks_wyrazu); 

    for(int i = indeks_wyrazu; i < indeks_wyrazu + liczba_wyrazow; i++){
        //suma_tmp *= (4.0 * i * i) / ((2 * i - 1.0) * (2 * i + 1.0));
        suma_tmp *= (2.0*i / (2*i - 1.0)) * (2.0*i /(2*i + 1.0));
    }

    pthread_mutex_lock(&mutex);
    suma *= suma_tmp;
    pthread_mutex_unlock(&mutex);

    fprintf(stdout, "Thread #%lu prod=%0.20lf\n", tid, suma_tmp);
    // suma_tmp = 0;
}

int main(int argc, char **argv){

    if(argc < 3){
        fprintf(stderr, "Nieprawidłowa ilość argumentów\n"); 
        return 1;
    }

    char *endptr;
    long int n = strtol(argv[1], &endptr, 10);

    if(*endptr != '\0' || n < 1 || n > 1000000000){
        fprintf(stderr, "Nieprawidłowa ilość wyrazów\n"); 
        return 1;
    }


    int w = strtol(argv[2], &endptr, 10);

    if(*endptr != '\0' || w < 1 || w > 100){
        fprintf(stderr, "Nieprawidłowe ilość wątków\n"); 
        return 1;
    }

    int reszta = n % w;
    int liczba_wyrazow = n / w;

    // struct arg argumenty[w];
    struct arg* argumenty = malloc(w * sizeof (struct arg));

    int indeks_pierszego_wyrazu = 1;

    for(int i = 0; i < w; i++){
        argumenty[i].liczba_wyrazow = liczba_wyrazow;
        argumenty[i].indeks_pierwszego_wyrazu = indeks_pierszego_wyrazu;
        indeks_pierszego_wyrazu += liczba_wyrazow;
    }

    argumenty[w - 1].liczba_wyrazow += reszta;


	struct timespec start, end;


    // pthread_t watki[w];
    pthread_t* watki = (pthread_t*)malloc(w*sizeof(pthread_t));

    clock_gettime(CLOCK_REALTIME, &start);
    for(int i = 0; i < w; i++){
        pthread_create(watki + i, NULL, thread, (void *)&argumenty[i]);
    }

    for(int i = 0; i < w; i++){
        pthread_join(watki[i], NULL);
    }
        clock_gettime(CLOCK_REALTIME, &end);


	free(watki);	
	free(argumenty);	

    suma *= 2;
	time_t sekundy = end.tv_sec - start.tv_sec;
	long nanosekundy = end.tv_nsec - start.tv_nsec;
	double czas = sekundy + nanosekundy / (double)1000000000;
    fprintf(stdout, "w/Threads:  PI=%.20Lf time=%lf\n", suma, czas); 

    // bez wątków
    long double sumab = 1.0;
    clock_gettime(CLOCK_REALTIME, &start);
    for(int i = 1; i <= n; i++){
        sumab *= (2.0*i / (2*i - 1.0)) * (2.0*i /(2*i + 1.0));
    }
    clock_gettime(CLOCK_REALTIME, &end);

	sekundy = end.tv_sec - start.tv_sec;
	nanosekundy = end.tv_nsec - start.tv_nsec;
	czas = sekundy + nanosekundy / (double)1000000000;

    sumab *= 2;
    fprintf(stdout, "wo/Threads: PI=%.20Lf time=%lf\n", sumab, czas); 


    // fprintf(stdout, "wo/Threads: PI=%.30Lf\n", pi); 

    return 0;
}
