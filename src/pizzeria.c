#include "pizzeria.h"
#include "queue.h"
#include "helper.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>

sem_t mesas_livres, garcons_livres, forno_livre;
pthread_mutex_t pegando_mesas, liberando_mesas;

int open;
int num_mesas;

void pizzeria_init(int tam_forno, int n_pizzaiolos, int n_mesas,
                   int n_garcons, int tam_deck, int n_grupos) {
    // inicializando mutexes, semaforos e estruturas de dados...
    sem_init(&garcons_livres, 0, n_garcons);
    sem_init(&mesas_livres, 0 , n_mesas);
    sem_init(&forno_livre, 0, tam_forno);
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
    sem_destroy(&garcons_livres);
    sem_destroy(&forno_livre);
    pthread_mutex_destroy(&pegando_mesas);
    pthread_mutex_destroy(&liberando_mesas);
}

void pizza_assada(pizza_t* pizza) {
    // Pode fazer isso com um semaforo?
    sem_post(&pizza.esperando_assar);
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
    int mesas = ceil(tam_grupo/4);
    for (int i = 0; i < mesas; i++)
        sem_post(&mesas_livres);
    pthread_mutex_unlock(&liberando_mesas);
}

void garcom_chamar() {
    sem_wait(&garcons_livres);
    sem_post(&gracons_livres);
}

void fazer_pedido(pedido_t* pedido) {
    // ATENCAO: separar essa funcao em partes menores!
    // colocar o pedido no smart deck (numa queue?)
    // falta controlar a ordenacao dos pedidos:
        sem_wait(&pizzaiolos_livres); 
            pizza_t * pizza = pedido_montar_pizza(pedido);
            sem_wait(&forno_livre);
            pthread_mutex_lock(&pa_de_pizza);
                pizzaiolo_colocar_pizza_forno(pedido);
            pthread_mutex_unlock(&pa_de_pizza);
            sem_wait(&pizza.esperando_assar);
            pthread_mutex_lock(&pa_de_pizza);
                pizzaiolo_retirar_pizza_forno(pedido);
            pthread_mutex_unlock(&pa_de_pizza);
            sem_post(&forno_livre);
            garcom_chamar();
        sem_post(&pizzaiolos_livres);
        garcom_entregar(pizza);
}

int pizza_pegar_fatia(pizza_t* pizza) {
    // ATENCAO: a sintaxe pizza.pegador_de_pizza pode estar errada (favor verificar)
    // e pode dar memory leak o esquema de criar um mutex pra cada pizza (mas pede na descricao)
    // entao ficar bem atento que esse mutex esta sem seu pthread_mutex_destroy()!!!
    pthread_mutex_init(&pizza->pegador);
    pthread_mutex_lock(&pizza->pegador);
    if (pizza.fatias > 0) {
        pizza.fatias -= 1;
        pthread_mutex_unlock(&pegador);
        return 0;
    }
    pthread_mutex_unlock(&pegador);
    return -1;
}
