// • Implement transfer operations requiring two account locks          [DONE]
// • Create a scenario where deadlock is highly likely to occur         [DONE]
// • Detect and report when threads appear stuck (no progress)          [TODO]
// • Use multiple threads performing transfers between same accounts    [DONE]

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
double logs[NUM_THREADS][TRANSACTIONS_PER_TELLER];

int deposit(int account_id, double amount) {
    printf("Locking account %u...\n", account_id);
    pthread_mutex_lock(&accounts[account_id].lock);

    if (pthread_mutex_lock(&accounts[account_id].lock) != 0) {
        printf("Failed to acquire lock");
        return 0;
    }

    printf("Locked account: %u\n", account_id);

    accounts[account_id].balance += amount;
    accounts[account_id].transaction_count++;

    pthread_mutex_unlock(&accounts[account_id].lock);

    printf("Unlocked account: %u\n", account_id);

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

        // only 1 account so not much to do with diff
        double amount = 100;

        printf("%d\n", i);

        result = deposit(random_account, amount);

        printf("Result: %d\n", result);

        if (result == 0) {
            i--;
            continue;
        }

        logs[teller_id][i] = amount;

        printf("Thread %d: %s %f\n", teller_id, (amount < 0)? "Withdrawing" : "Depositing", fabs(amount));

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

        pthread_mutex_init(&accounts[i].lock, NULL);
    }

    printf("Initial Balance: %f\n", accounts[0].balance);
    
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

    printf("Final Account Balances: %f\n", accounts[0].balance);

    double sum = INITIAL_BALANCE; // starting account balance
    for (int i = 0; i < NUM_THREADS; i++) {
        for (int j = 0; j < TRANSACTIONS_PER_TELLER; j++) {
            sum += logs[i][j];
        }
    }

    printf("Correct Account Balance: %f\n\n", sum);

    return 0;
}