#include "pizzeria.h"
#include "queue.h"
#include "helper.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h> 28     pthread_mutex_init(&liberando_mesas, NULL);


sem_t mesas_livres, garcons_livres, forno_livre;
pthread_mutex_t pegando_mesas, liberando_mesas;

int open;
int num_mesas;

queue_t* smart_deck;

pizza_t * pizza_no_balcao;

void pizzeria_init(int tam_forno, int n_pizzaiolos, int n_mesas,
                   int n_garcons, int tam_deck, int n_grupos) {
    // inicializando mutexes, semaforos e estruturas de dados...
    smart_deck = malloc(sizeof(queue_t));
	queue_init(smart_deck, tam_deck);

    sem_init(&garcons_livres, 0, n_garcons);
    sem_init(&mesas_livres, 0 , n_mesas);
    sem_init(&forno_livre, 0, tam_forno);
    pthread_mutex_init(&pegando_mesas, NULL);
    pthread_mutex_init(&liberando_mesas, NULL);
    num_mesas = n_mesas;
    open = 1;
}

void pizzeria_close() {
    // ATENCAO: precisa esperar todo mundo levantar!
    open = 0;
    for (int i = 0 =; i < num_mesas; i++)
        sem_wait(&mesas_livres);
}

void pizzeria_destroy() {
    // destroi mutexes, semaforos e estruturas de dados...
    queue_destroy(smart_deck);
    free(smart_deck);

    sem_destory(&mesas_livres);
    sem_destroy(&garcons_livres);
    sem_destroy(&forno_livre);
    pthread_mutex_destroy(&pegando_mesas);
    pthread_mutex_destroy(&liberando_mesas);
}

void pizza_assada(pizza_t* pizza) {
    // libera pizzaiolo_retirar_pizza_forno() e nao faz mais nada? vide email.
    sem_post(&pizza->assada);
}

int pegar_mesas(int tam_grupo) {
    int mesas = ceil(tam_grupo/4);
    lock()
    for (int i = 0; i < mesas; i++)
        sem_wait(&mesas_livres);
    if (mesas <= num_mesas && open) {
        num_mesas -= mesas;
        unlock()
        return 0;
    }
    unlock()
    return -1;
}

void garcom_tchau(int tam_grupo) {
    int mesas = ceil(tam_grupo/4);
    for (int i = 0; i < mesas; i++)
        sem_post(&mesas_livres);
    sem_post(&gracons_livres);
}

void garcom_chamar() {
    sem_wait(&garcons_livres);
}

void fazer_pedido(pedido_t* pedido) {
    queue.push(pedido);
    // ATENCAO: Colocar nas threads dos pizzaiolos!!!
}

thread pizzaiolo_n {
    ...
    pedido_t * pedido = smart_deck.pop();
    pizza_t * pizza = pizzaiolo_montar_pizza();
    // pthread_mutex_init(&pizza->pegador_de_pizza, NULL);
    sem_init(&pizza->assada, 0, 0);

    sem_wait(&forno_livre);
    pthread_mutex_lock(&pa_de_pizza);
    pizzaiolo_colocar_pizza_forno(pizza);
    pthread_mutex_unlock(&pa_de_pizza);
    sem_wait(&pizza->assada);
    pthread_mutex_lock(&pa_de_pizza);
    pizzaiolo_retirar_pizza_forno(pizza);
    pthread_mutex_unlock(&pa_de_pizza); 
    
    pthread_mutex_lock(&espaco_vazio);
    garcom_charmar();
    sem_post(&pedido_pronto);
    pthread_mutex_unlock(&espaco_vazio);
}

thread buscando_pizzas_do_deck {
    wait(&pedido_pronto); 
    wait(&garcons_livres);
    garcom_entregar(pizza)
}

int pizza_pegar_fatia(pizza_t* pizza) {
    // ATENCAO: como inicializar o pegador e o esperando_assar sem quebrar horrendamente
    // quando varias pessoas tentarem comer pizza dps que nao tem mais pizza? Nao perguntamos ainda!!!
    pthread_mutex_lock(&pizza->pegador);
    if (pizza->fatias > 0) {
        pizza->fatias -= 1;
        pthread_mutex_unlock(&pegador);
        return 0;
    }
    pthread_mutex_unlock(&pegador);
    return -1;
}
