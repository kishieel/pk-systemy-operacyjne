#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_READERS 5
#define MAX_WRITERS 5

// struktura danych dla wątku pisarza
typedef struct
{
    int id;
} writer_data_t;

// struktura danych dla wątku czytelnika
typedef struct
{
    int id;
} reader_data_t;

// zmienne globalne
int readers_count = 0;
int writers_count = 0;
int data = 0;
sem_t readers_mutex;
sem_t writers_mutex;
sem_t readers_sem;
sem_t writers_sem;

// funkcja czytelnika
void *reader(void *arg)
{
    reader_data_t *data = (reader_data_t *)arg;

    while (1)
    {
        sem_wait(&readers_mutex); // blokada dla czytelników
        readers_count++;
        if (readers_count == 1)
        {
            sem_wait(&writers_sem); // blokada dla pisarzy
        }
        sem_post(&readers_mutex); // odblokowanie dla czytelników

        // odczyt danych
        printf("Czytelnik %d odczytuje dane: %d\n", data->id, data);

        sem_wait(&readers_mutex); // blokada dla czytelników
        readers_count--;
        if (readers_count == 0)
        {
            sem_post(&writers_sem); // odblokowanie dla pisarzy
        }
        sem_post(&readers_mutex); // odblokowanie dla czytelników

        sleep(1); // symulacja czasu oczekiwania na następne odczyty
    }

    pthread_exit(NULL);
}

// funkcja pisarza
void *writer(void *arg)
{
    writer_data_t *data = (writer_data_t *)arg;

    while (1)
    {
        sem_wait(&writers_mutex); // blokada dla pisarzy
        writers_count++;
        if (writers_count == 1)
        {
            sem_wait(&readers_sem); // blokada dla czytelników
        }
        sem_post(&writers_mutex); // odblokowanie dla pisarzy

        // zapis danych
        data++;
        printf("Pisarz %d zapisuje dane: %d\n", data->id, data);

        sem_wait(&writers_mutex); // blokada dla pisarzy
        writers_count--;
        if (writers_count == 0)
        {
            sem_post(&readers_sem); // odblokowanie dla czytelników
        }
        sem_post(&writers_mutex); // odblokowanie dla pisarzy

        sleep(1); // symulacja czasu oczekiwania na następne zapisy
    }

    pthread_exit(NULL);
}

int main()
{
    // inicjalizacja semaforów
    sem_init(&readers_mutex, 0, 1);
    sem_init(&writers_mutex, 0, 1);
    sem_init(&readers_sem, 0, MAX_READERS);
    sem_init(&writers_sem, 0, MAX_WRITERS);

    // inicjalizacja wątków
    pthread_t readers[MAX_READERS];
    pthread_t writers[MAX_WRITERS];
    reader_data_t reader_data[MAX_READERS];
    reader_data_t writer_data[MAX_WRITERS];

    for (int i = 0; i < MAX_READERS; i++)
    {
        reader_data[i].id = i;
        pthread_create(&readers[i], NULL, reader, (void *)&reader_data[i]);
    }

    for (int i = 0; i < MAX_WRITERS; i++)
    {
        writer_data_t *writer_data = (writer_data_t *)malloc(sizeof(writer_data_t));
        writer_data->id = i;
        pthread_create(&writers[i], NULL, writer, (void *)writer_data);
    }

    // czekanie na zakończenie wątków
    for (int i = 0; i < MAX_READERS; i++)
    {
        pthread_join(readers[i], NULL);
    }

    for (int i = 0; i < MAX_WRITERS; i++)
    {
        pthread_join(writers[i], NULL);
    }

    // usuwanie semaforów
    sem_destroy(&readers_mutex);
    sem_destroy(&writers_mutex);
    sem_destroy(&readers_sem);
    sem_destroy(&writers_sem);

    return 0;
}