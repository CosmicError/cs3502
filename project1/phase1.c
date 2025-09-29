#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define NUM_ACCOUNTS 1
const int NUM_THREADS = 10;
const int TRANSACTIONS_PER_TELLER = 10;

// Shared data structure
typedef struct {
    int account_id ;
    double balance ;
    int transaction_count;
} Account;

Account accounts[NUM_ACCOUNTS];

// Thread function
void* teller_thread (void * arg) {
    int teller_id = *(int*) arg; // Cast void * to int * and dereference

    // Perform multiple transactions
    for (int i = 0; i < TRANSACTIONS_PER_TELLER; i++) {

        // Select random account
        // int random_account;
        // do {
        unsigned int seed = time(NULL) + pthread_self() ;
        int random_account = rand_r(&seed) % NUM_ACCOUNTS;
        // } while (accounts[random_account] == NULL);


        // Perform deposit or withdrawal
        switch (random_account % 4)
        {
        case 0:
            accounts[random_account].balance -= 100;
            break;
        case 1:
            accounts[random_account].balance += 100 ;
            break;
        case 2:
            accounts[random_account].balance -= 50;
            break;
        case 3:
            accounts[random_account].balance += 50;
            break;
        }

        // THIS WILL HAVE RACE CONDITIONS !

        printf ("Teller %d: Transaction %d\n", teller_id, i) ;

    }

    return NULL ;
}

int main() {
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        accounts[i] = (Account){
            .account_id = i, 
            .balance = 1000, 
            .transaction_count = 0
        };
    }
    
    // Creating threads ( see Appendix \ ref { sec : voidpointer } for void * explanation )
    pthread_t threads [NUM_THREADS];
    int thread_ids [NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create (&threads[i], NULL, teller_thread, &thread_ids[i]);
    }

    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}