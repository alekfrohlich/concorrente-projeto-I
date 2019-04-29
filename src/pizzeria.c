#include "pizzeria.h"
#include "queue.h"
#include "helper.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>


void pizzeria_init(int tam_forno, int n_pizzaiolos, int n_mesas,
                   int n_garcons, int tam_deck, int n_grupos) {

}

void pizzeria_close() {
}

void pizzeria_destroy() {
}

void pizza_assada(pizza_t* pizza) {
}

int pegar_mesas(int tam_grupo) {
    pthread_mutex_lock(&escolhendo_mesa);
    if (ceil(tam_grupo/4) >= n_mesas) {
        n_mesas = ceil(tam_grupo/4);
        pthread_mutex_unlock(&escolhendo_mesa);
        return 0;
    }
    pthread_mutex_unlock*&escolhendo_mesa);
    return -1;
}

void garcom_tchau(int tam_grupo) {
}

void garcom_chamar() {
}

void fazer_pedido(pedido_t* pedido) {
}

int pizza_pegar_fatia(pizza_t* pizza) {
    return -1; // erro: não fui implementado (ainda)!
}
