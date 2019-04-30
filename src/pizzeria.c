#include "pizzeria.h"
#include "queue.h"
#include "helper.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>

sem_t mesas_livres;
pthread_mutex_t pegando_mesas, liberando_mesas;

void pizzeria_init(int tam_forno, int n_pizzaiolos, int n_mesas,
                   int n_garcons, int tam_deck, int n_grupos) {

}

void pizzeria_close() {
    // espera outras funcoes acabarem e entao setta open = 0
}

void pizzeria_destroy() {
    // destroi mutexes e semaforos
}

void pizza_assada(pizza_t* pizza) {
}

int pegar_mesas(int tam_grupo) {
    pthread_mutex_lock(&pegando_mesas);
    int ceil = ceil(tam_grupo/4);
    for (int i = 0; i < ceil; i++)
        sem_wait(&mesas_livres);
    if (ceil <= num_mesas && open) {
        num_mesas -= ceil;
        pthread_mutex_unlock(&pegando_mesas);
        return 0;
    }
    pthread_mutex_unlock(&pegando_mesas);
    return -1; //erro: não fui implementado (ainda)!
}

void garcom_tchau(int tam_grupo) {
    pthread_mutex_lock(&liberando_mesas);
    int ceil = ceil(tam_grupo/4);
    for (int i = 0; i < ceil; i++)
        sem_post(&mesas_livres);
    pthread_mutex_unlock(&liberando_mesas);
}

void garcom_chamar() {
    sem_wait(&garcons_livres);
    sem_post(&gracons_livres);
}

void fazer_pedido(pedido_t* pedido) {

}

int pizza_pegar_fatia(pizza_t* pizza) {
    return -1; // erro: não fui implementado (ainda)!
}
