#include "pizzeria.h"
#include "queue.h"
#include "helper.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h> 

sem_t garcons_livres, forno_livre;
pthread_mutex_t espaco_vazio, pa_de_pizza, pegando_mesas;
pthread_cond_t liberou_mesas, ultimo_cliente_saiu;

int open;
int cozinha_fechada;
int num_mesas_total;
int num_pizzaiolos;
int mesas_livres;

queue_t * smart_deck;

pthread_t * pizzaiolos;

void * garcom_busca_pizza_balcao(void * arg) {
    pizza_t* pizza =  (pizza_t *) arg;
    garcom_entregar(pizza);
    sem_post(&garcons_livres);
    pthread_exit(NULL);
}

void * pizzaiolo_func(void * arg) {
    while(1) {
        pedido_t * pedido = (pedido_t *) queue_wait(smart_deck);
        if(cozinha_fechada)
            break;
        pizza_t * pizza = pizzaiolo_montar_pizza(pedido);
        pthread_mutex_init(&pizza->pegador, NULL);
        sem_init(&pizza->assada, 0, 0);
        sem_wait(&forno_livre);
        pthread_mutex_lock(&pa_de_pizza);
        pizzaiolo_colocar_forno(pizza);
        pthread_mutex_unlock(&pa_de_pizza);

        sem_wait(&pizza->assada);
        pthread_mutex_lock(&pa_de_pizza);
        pizzaiolo_retirar_forno(pizza);
        sem_post(&forno_livre);
        pthread_mutex_unlock(&pa_de_pizza); 

        pthread_mutex_lock(&espaco_vazio);
        garcom_chamar();
        pthread_t garcom_busca_pizza_balcao_;
        pthread_create(&garcom_busca_pizza_balcao_, NULL, garcom_busca_pizza_balcao, (void*) pizza);
        pthread_detach(garcom_busca_pizza_balcao_);
        pthread_mutex_unlock(&espaco_vazio);
    }
    pthread_exit(NULL);
}

void pizzeria_init(int tam_forno, int n_pizzaiolos, int n_mesas,
                   int n_garcons, int tam_deck, int n_grupos) {
    // inicializando mutexes, semaforos e estruturas de dados...
    if (!open) {
        smart_deck = malloc(sizeof(queue_t));
        queue_init(smart_deck, tam_deck);

        pizzaiolos = malloc(sizeof(pthread_t) * n_pizzaiolos);

        pthread_mutex_init(&espaco_vazio, NULL);
        pthread_mutex_init(&pa_de_pizza, NULL);
        pthread_mutex_init(&pegando_mesas, NULL);

        pthread_cond_init(&liberou_mesas, NULL);
        pthread_cond_init(&ultimo_cliente_saiu, NULL);

        sem_init(&garcons_livres, 0, n_garcons);
        sem_init(&forno_livre, 0, tam_forno);

        mesas_livres = num_mesas_total;
        num_pizzaiolos = n_pizzaiolos;
        num_mesas_total = n_mesas;
        open = 1;
        cozinha_fechada = 0;

        for(int i=0; i<n_pizzaiolos; i++)
            pthread_create(&pizzaiolos[i], NULL, pizzaiolo_func, NULL);
    }
}

void pizzeria_close() {
    // ATENCAO: precisa esperar todo mundo levantar!
    if (open) {
        open = 0;
        
        pthread_mutex_lock(&pegando_mesas);
        while (mesas_livres != num_mesas_total) {
            pthread_cond_wait(&ultimo_cliente_saiu, &pegando_mesas);
        }
        pthread_mutex_unlock(&pegando_mesas);

        cozinha_fechada = 1;

        for (int i = 0; i < num_pizzaiolos; i++)
            queue_push_back(smart_deck, (pedido_t *) NULL); 

        for(int i=0; i< num_pizzaiolos; i++)
            pthread_join(pizzaiolos[i], NULL);
    }
}

void pizzeria_destroy() {
    // espera threads, destroi mutexes, semaforos e estruturas de dados...
    queue_destroy(smart_deck);
    free(smart_deck);
    free(pizzaiolos);

    pthread_mutex_destroy(&espaco_vazio);
    pthread_mutex_destroy(&pa_de_pizza);
    pthread_mutex_destroy(&pegando_mesas);

    pthread_cond_destroy(&liberou_mesas);
    pthread_cond_destroy(&liberou_mesas);

    sem_destroy(&garcons_livres);
    sem_destroy(&forno_livre);
}

void pizza_assada(pizza_t* pizza) {
    // libera o pizzaiolo para ele retirar a pizza do forno.
    sem_post(&pizza->assada);
}

int pegar_mesas(int tam_grupo) {
    while (open) {

        int mesas = ceil(tam_grupo/4.0);
        if (mesas > num_mesas_total)
            return -1;

        pthread_mutex_lock(&pegando_mesas);
        while (mesas > mesas_livres && open)
            pthread_cond_wait(&liberou_mesas, &pegando_mesas);


        if (open) {
            mesas_livres -= mesas;
            pthread_mutex_unlock(&pegando_mesas);
            return 0;
        }
        pthread_mutex_unlock(&pegando_mesas);
    }
    return -1;
}

void garcom_tchau(int tam_grupo) {

    int mesas = ceil(tam_grupo/4.0);

    pthread_mutex_lock(&pegando_mesas);
    mesas_livres += mesas;
    pthread_cond_broadcast(&liberou_mesas);
    if (mesas_livres == num_mesas_total && !open) // E se o ultimo cliente sair antes da pizzaria fechar??
        pthread_cond_broadcast(&ultimo_cliente_saiu);
    pthread_mutex_unlock(&pegando_mesas);
   
    sem_post(&garcons_livres);
}

void garcom_chamar() {
    sem_wait(&garcons_livres);
}

void fazer_pedido(pedido_t* pedido) {
    queue_push_back(smart_deck,pedido);
}

int pizza_pegar_fatia(pizza_t* pizza) {
    pthread_mutex_lock(&pizza->pegador);
    if (pizza->fatias > 0) {
        pizza->fatias -= 1;
        pthread_mutex_unlock(&pizza->pegador);
        return 0;
    }
    pthread_mutex_unlock(&pizza->pegador);
    return -1;
}

