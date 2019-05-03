#include "pizzeria.h"
#include "queue.h"
#include "helper.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h> //28     pthread_mutex_init(&liberando_mesas, NULL);



pizza_t* pizza_no_balcao;

sem_t mesas_livres, garcons_livres, forno_livre, pedido_pronto;

int open;
int num_mesas;
int cozinha_fechada;

pthread_mutex_t  espaco_vazio, pa_de_pizza;


pthread_t garcons;
pthread_t *pizzaiolos;

int n_pizzaiolos;

queue_t* smart_deck;

pizza_t * pizza_no_balcao;

void pizzeria_init(int tam_forno, int qtde_pizzaiolos, int n_mesas,
                   int n_garcons, int tam_deck, int n_grupos) {
    // inicializando mutexes, semaforos e estruturas de dados...
    n_pizzaiolos = qtde_pizzaiolos;

    smart_deck = malloc(sizeof(queue_t));
	queue_init(smart_deck, tam_deck);

    pthread_mutex_init(&espaco_vazio, NULL);
    pthread_mutex_init(&pa_de_pizza, NULL);
    
    sem_init(&garcons_livres, 0, n_garcons);
    sem_init(&mesas_livres, 0 , n_mesas);
    sem_init(&forno_livre, 0, tam_forno);
    sem_init(&pedido_pronto, 0, 0);

    for(int i=0; i<n_pizzaiolos; i++){
        pthread_create(&pizzaiolos[i], NULL, pizzaiolo_func, NULL);
    }
    pthread_create(&garcons, NULL, garcons_buscam_pizza_deck, NULL);

    num_mesas = n_mesas;
    open = 1;
    cozinha_fechada = 0;
}

void pizzeria_close() {
    // ATENCAO: precisa esperar todo mundo levantar!
    open = 0;
    for (int i = 0; i < num_mesas; i++)
        sem_wait(&mesas_livres);
    cozinha_fechada = 1;
    sem_post(&pedido_pronto);

    pthread_join(garcons, NULL);
    for(int i=0; i< n_pizzaiolos; i++){
        pthread_join(pizzaiolos[i], NULL);
    }
}

void pizzeria_destroy() {
    // destroi mutexes, semaforos e estruturas de dados...
    queue_destroy(smart_deck);
    free(smart_deck);

    pthread_mutex_destroy(&espaco_vazio);
    pthread_mutex_destroy(&pa_de_pizza);

    sem_destroy(&pedido_pronto);
    sem_destroy(&mesas_livres);
    sem_destroy(&garcons_livres);
    sem_destroy(&forno_livre);
}

void pizza_assada(pizza_t* pizza) {
    // libera pizzaiolo_retirar_pizza_forno() e nao faz mais nada? vide email.
    sem_post(&pizza->assada);
}

int pegar_mesas(int tam_grupo) {
    int mesas = ceil(tam_grupo/4);
    // lock()
    for (int i = 0; i < mesas; i++)
        sem_wait(&mesas_livres);
    if (mesas <= num_mesas && open) {
        num_mesas -= mesas;
        // unlock()
        return 0;
    }
    // unlock()
    return -1;
}

void garcom_tchau(int tam_grupo) {
    int mesas = ceil(tam_grupo/4);
    for (int i = 0; i < mesas; i++)
        sem_post(&mesas_livres);
    sem_post(&garcons_livres);
}

void garcom_chamar() {
    sem_wait(&garcons_livres);
}

void fazer_pedido(pedido_t* pedido) {
    queue_push_back(smart_deck,pedido);
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
        {
            pizzaiolo_colocar_forno(pizza);
        }
        pthread_mutex_unlock(&pa_de_pizza);
        sem_wait(&pizza->assada);
        pthread_mutex_lock(&pa_de_pizza);
        {
            pizzaiolo_retirar_forno(pizza);
        }
        pthread_mutex_lock(&espaco_vazio);
        pizza_no_balcao = pizza;
        pthread_mutex_unlock(&pa_de_pizza); 
        {
            garcom_chamar();
            sem_post(&pedido_pronto);
        }
        pthread_mutex_unlock(&espaco_vazio);
    }
    return NULL;

}

void * garcons_buscam_pizza_deck(void * arg) {
    while (1) {
        sem_wait(&pedido_pronto); 
        if (cozinha_fechada) 
            break;
        pizza_t* pizza =  pizza_no_balcao;
        garcom_entregar(pizza);
        sem_post(&garcons_livres);
    }
    return NULL;
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

