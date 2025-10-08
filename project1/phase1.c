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

const int INITIAL_BALANCE = 1000;

// Shared data structure
typedef struct {
    int account_id ;
    double balance ;
    int transaction_count;
} Account;

Account accounts[NUM_ACCOUNTS];
double logs[NUM_THREADS][TRANSACTIONS_PER_TELLER];

// Thread function
void* teller_thread(void * arg) {
    int teller_id = *(int*) arg; // Cast void * to int * and dereference
    unsigned int seed = time(NULL) + pthread_self() ;

    // Perform multiple transactions
    for (int i = 0; i < TRANSACTIONS_PER_TELLER; i++) {

        // Select random account
        int random_account = rand_r(&seed) % NUM_ACCOUNTS; 

        // only 1 account so not much to do with diff
        double diff = 100;

        accounts[random_account].balance += diff;
        accounts[random_account].transaction_count++;

        logs[teller_id][i] = diff;
        printf("Thread %d: %s $%f\n", teller_id, (diff < 0)? "Withdrawing" : "Depositing", fabs(diff));

        // THIS WILL HAVE RACE CONDITIONS !

        printf ("Teller %d: Transaction %d\n", teller_id, i) ;

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
    }

    printf("Initial Balance: %.2f\n", accounts[0].balance);
    
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

    printf("Final Account Balances: %.2f\n", accounts[0].balance);

    double sum = INITIAL_BALANCE; // starting account balance
    for (int i = 0; i < NUM_THREADS; i++) {
        for (int j = 0; j < TRANSACTIONS_PER_TELLER; j++) {
            sum += logs[i][j];
        }
    }

    printf("Correct Account Balance: $%.2f\n\n", sum);

    return 0;
}