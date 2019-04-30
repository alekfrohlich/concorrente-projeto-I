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

int open;
int num_mesas;

void pizzeria_init(int tam_forno, int n_pizzaiolos, int n_mesas,
                   int n_garcons, int tam_deck, int n_grupos) {
    // inicializando mutexes, semaforos e estruturas de dados...
    sem_init(&mesas_livres, 0 , n_mesas);
    pthread_mutex_init(&pegando_mesas, NULL);
    pthread_mutex_init(&liberando_mesas, NULL);
    num_mesas = n_mesas;
    open = 1;
}

void pizzeria_close() {
    // pensando bem acho que nao precisa esperar, so nao pode sentar pessoas depois que fecha e pra isso ja tem o open = 0
    open = 0;
}

void pizzeria_destroy() {
    // destroi mutexes, semaforos e estruturas de dados...
    sem_destory(&mesas_livres);
    pthread_mutex_destroy(&pegando_mesas);
    pthread_mutex_destroy(&liberando_mesas);
}

void pizza_assada(pizza_t* pizza) {
    // nao entendi esse aqui? eh pra gente usar um wait(pizza.ts)? O que significa avisar que a pizza ta pronta?
}

int pegar_mesas(int tam_grupo) {
    pthread_mutex_lock(&pegando_mesas);
    int mesas = ceil(tam_grupo/4);
    for (int i = 0; i < mesas; i++)
        sem_wait(&mesas_livres);
    if (mesas <= num_mesas && open) {
        num_mesas -= mesas;
        pthread_mutex_unlock(&pegando_mesas);
        return 0;
    }
    pthread_mutex_unlock(&pegando_mesas);
    return -1;
}

void garcom_tchau(int tam_grupo) {
    pthread_mutex_lock(&liberando_mesas);
    int mesas = mesas(tam_grupo/4);
    for (int i = 0; i < mesas; i++)
        sem_post(&mesas_livres);
    pthread_mutex_unlock(&liberando_mesas);
}

void garcom_chamar() {
    sem_wait(&garcons_livres);
    sem_post(&gracons_livres);
}

void fazer_pedido(pedido_t* pedido) {
    // colocar o pedido no smart deck (numa queue?)
    // falta controlar a concorrencia disso:
        pizza_t * pizza = pedido_montar_pizza(pedido);
        pizzaiolo_colocar_pizza_forno(pedido);
        pizzaiolo_retirar_pizza_forno(pedido);
        garcom_entregar(pizza);
}

int pizza_pegar_fatia(pizza_t* pizza) {
    pthread_mutex_lock(&pegador_de_pizza);
    if (pizza.fatias > 0) {
        pizza.fatias -= 1;
        pthread_mutex_unlock(&pegador_de_pizza);
        return 0;
    }
    pthread_mutex_unlock(&pegador_de_pizza);
    return -1;
}
