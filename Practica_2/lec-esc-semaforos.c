#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_L  3
#define MAX_E  2


int dato = 0;
sem_t mutex;
sem_t m_escritores;
int n_lectores = 0;
int n_escritores = 0;

void *lector(void *arg) {
	int id = (int )arg;
	while (1) {
		sem_wait(&mutex);
		n_lectores++;
		if (n_lectores == 1) {
			sem_wait(&m_escritores);
		}
		sem_post(&mutex);
		// Leer datos
		printf("Lector %d leyendo: %d\n", id, dato);
		sem_wait(&mutex);
		n_lectores--;
		if (n_lectores == 0) {
			sem_post(&m_escritores);
		}
		sem_post(&mutex);
		usleep(rand() % 1000000); // Retraso aleatorio de hasta 1 segundo (en microsegundos)
	}
}

void *escritor(void *arg) {
	int id = (int )arg;
	int aux;
	while (1) {
		n_escritores++;
		sem_wait(&m_escritores);
		n_escritores--;
		aux = dato;
		usleep(rand() % 1000000);
		aux=aux+1;
		// Escribir datos
		dato = aux;
		printf("Escritor %d escribiendo: %d\n", id, dato);
		sem_post(&m_escritores);
		usleep(rand() % 1000000); // Retraso aleatorio de hasta 1 segundo (en microsegundos)
		
	}
}

int main() {
	// Inicializar semáforos
	sem_init(&mutex, 0, 1);
	sem_init(&m_escritores, 0, 1);

	// MAX_L lectores y MAX_E escritores
	pthread_t lectores[MAX_L], escritores[MAX_E]; 
	int i;
	pthread_attr_t atrib;

	// Inicializar la semilla del generador de números aleatorios
	srand(time(NULL)); 

	// Inicializar la estructura pthread_attr
	pthread_attr_init(&atrib);
	
	// Crear lectores
	for (i = 0; i < MAX_L; i++) {
		pthread_create(&lectores[i], &atrib, lector, (void *)i);
	}

	// Crear escritores
	for (i = 0; i < MAX_E; i++) {
		pthread_create(&escritores[i], &atrib, escritor, (void *)i);
	}
	
	// Esperar a que los hilos terminen
	for (i = 0; i < MAX_L; i++) {
		pthread_join(lectores[i], NULL);
	}
	for (i = 0; i < MAX_E; i++) {
		pthread_join(escritores[i], NULL);
	}

	// Destruir semáforos
	sem_destroy(&mutex);
	sem_destroy(&m_escritores);

	printf("Acaba el main\n");
	return 0;
}
