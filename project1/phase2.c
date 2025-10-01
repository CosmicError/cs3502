#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define NUM_ACCOUNTS 1
#define NUM_THREADS 10
#define TRANSACTIONS_PER_TELLER 10

const double INITIAL_BALANCE = 1000;

// Shared data structure
typedef struct {
    int account_id ;
    double balance ;
    int transaction_count;
    pthread_mutex_t lock;
} Account;

Account accounts[NUM_ACCOUNTS];
double logs[NUM_ACCOUNTS][NUM_THREADS][TRANSACTIONS_PER_TELLER];

int deposit(int account_id, double amount) {
    if (pthread_mutex_trylock(&accounts[account_id].lock) != 0) {
        printf("Failed to acquire lock\n");
        return 0;
    }

    if (amount < 0 && accounts[account_id].balance + amount < 0) {
        pthread_mutex_unlock(&accounts[account_id].lock);
        // printf("Can't overdraft account with %f.\n", amount);
        return 0;
    }

    accounts[account_id].balance += amount;
    accounts[account_id].transaction_count++;

    pthread_mutex_unlock(&accounts[account_id].lock);

    return 1;
}

// Thread function
void* teller_thread(void * arg) {
    int teller_id = *(int*) arg; // Cast void * to int * and dereference

    int result = 1;
    unsigned int seed;
    int random_account;

    // Perform multiple transactions
    for (int i = 0; i < TRANSACTIONS_PER_TELLER; i++) {

        // Select random account
        if (result == 1) {
            seed = time(NULL) + pthread_self();
            random_account = rand_r(&seed) % NUM_ACCOUNTS; 
        }  

        double amount = rand_r(&seed) % 125941 * (rand_r(&seed) % 2 == 0 ? 1 : -1) / (double)1000;

        result = deposit(random_account, amount);

        if (result == 0) {
            seed = time(NULL) + pthread_self();
            i--;

            continue;
        }

        logs[random_account][teller_id][i] = amount;

        printf("Thread %d: %s %.2f\n", teller_id, (amount < 0)? "Withdrawing" : "Depositing", fabs(amount));
    }

    return NULL ;
}

int main() {
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        accounts[i] = (Account){
            .account_id = i, 
            .balance = INITIAL_BALANCE, 
            .transaction_count = 0
        };

        pthread_mutex_init(&accounts[i].lock, NULL);
    }

    printf("Initial Balance: $%.2f\n", accounts[0].balance);
    
    // Creating threads ( see Appendix \ ref { sec : voidpointer } for void * explanation )
    pthread_t threads [NUM_THREADS];
    int thread_ids [NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, teller_thread, &thread_ids[i]);
    }

    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("\n========================================\n");
    printf("\tFinal Account Balances\n");
    printf("========================================\n");

    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        // close mutex's as well
        pthread_mutex_destroy(&accounts[i].lock);

        printf("Account %d: %.2f\n", i, accounts[i].balance);
    }

    printf("\n========================================\n");
    printf("\tCorrect Account Balances\n");
    printf("========================================\n");

    for (int i = 0; i < NUM_ACCOUNTS; i++) {

        double sum = INITIAL_BALANCE; // starting account balance

        for (int j = 0; j < NUM_THREADS; j++) {
            for (int k = 0; k < TRANSACTIONS_PER_TELLER; k++) {
                sum += logs[i][j][k];
            }
        }

        printf("Account %d: %.2f\n", i, sum);
    }

    printf("\n");

    return 0;
}