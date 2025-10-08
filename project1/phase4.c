#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#define NUM_ACCOUNTS 2
#define NUM_THREADS 10
#define TRANSACTIONS_PER_TELLER 10

const double INITIAL_BALANCE = 5000;

// Shared data structure
typedef struct {
    int account_id ;
    double balance ;
    int transaction_count;
    pthread_mutex_t lock;
} Account;

Account accounts[NUM_ACCOUNTS];

int safe_transfer(int from_id, int to_id, double amount) {
    // Invalid amount
    if (amount < 0 || from_id == to_id) {
        return 0;
    }

    int first = (from_id < to_id) ? from_id : to_id;
    int second = (from_id < to_id) ? to_id : from_id;

    if (pthread_mutex_trylock(&accounts[first].lock) != 0) {
        // printf("Failed to acquire \'from\' account lock\n");
        return 0;
    }

    // Create opportunity for a deadlock
    // usleep(100);

    // Detect deadlock
    if (pthread_mutex_trylock(&accounts[second].lock) != 0) {
        // printf("Failed to acquire \'to\' account lock\n");
        // printf("Deadlock detected\n");
        pthread_mutex_unlock(&accounts[first].lock);
        return 0;
    }

    if (from_id < to_id) {
        accounts[first].balance -= amount;
        accounts[second].balance += amount;
    }
    else {
        accounts[first].balance += amount;
        accounts[second].balance -= amount;
    }

    // printf("Account %u Balance: %.2f\n", first, accounts[first].balance);
    // printf("Account %u Balance: %.2f\n", second, accounts[second].balance);

    accounts[first].transaction_count++;
    accounts[second].transaction_count++;

    pthread_mutex_unlock(&accounts[second].lock);
    pthread_mutex_unlock(&accounts[first].lock);

    return 1;
}

// Thread function
void* teller_thread(void * arg) {
    int teller_id = *(int*) arg; // Cast void * to int * and dereference

    int result = 1;
    unsigned int seed;
    int random_account;
    int random_account_2;

    // Perform multiple transactions
    for (int i = 0; i < TRANSACTIONS_PER_TELLER; i++) {

        // Select random account
        if (result == 1) {
            usleep(10);
            seed = time(NULL) + pthread_self();
            random_account = rand_r(&seed) % NUM_ACCOUNTS; 

            // get another random account that is not the same as the first random account
            do {
                random_account_2 = rand_r(&seed) % NUM_ACCOUNTS;
            } while (random_account == random_account_2);
        }  

        double amount = rand_r(&seed) % 125941 / (double)1000;

        result = accounts[random_account].balance >= amount ? safe_transfer(random_account, random_account_2, amount) : 0;

        if (result == 0) {
            seed = time(NULL) + pthread_self();
            i--;

            continue;
        }

        printf("Thread %d: Transfering $%.2f from account %u to account %u \n", teller_id, amount, random_account, random_account_2);
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

    printf("Initial Balances:\n");
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf("....Account %d: $%.2f\n", i, accounts[i].balance);
    }
    
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

    printf("\nFinal Account Balances\n");

    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        // close mutex's as well
        pthread_mutex_destroy(&accounts[i].lock);

        printf("....Account %d: $%.2f\n", i, accounts[i].balance);
        // printf("Account %d Transactions: %d\n", i, accounts[i].transaction_count);
    }

    printf("\n");

    return 0;
}