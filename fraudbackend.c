#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ACCOUNTS 10
#define MAX_TRANSACTIONS 100

typedef struct {
    int accNo;
    char name[50];
    double balance;
    char lastLocation[30];
    time_t lastTxnTime;
} Account;

typedef struct {
    int txnId;
    int accNo;
    double amount;
    char location[30];
    time_t timestamp;
} Transaction;

Account accounts[MAX_ACCOUNTS];
Transaction transactions[MAX_TRANSACTIONS];
int txnCount = 0;

void setupAccounts() {
    for (int i = 0; i < MAX_ACCOUNTS; i++) {
        accounts[i].accNo = 100 + i;
        sprintf(accounts[i].name, "User%d", i + 1);
        accounts[i].balance = 100000.0;
        strcpy(accounts[i].lastLocation, "");
        accounts[i].lastTxnTime = 0;
    }
}

void checkFraud(Transaction txn, char alerts[1024]) {
    alerts[0] = '\0';
    Account *acc = NULL;
    
    for (int i = 0; i < MAX_ACCOUNTS; i++)
        if (accounts[i].accNo == txn.accNo) { 
            acc = &accounts[i]; 
            break; 
        }
    
    if (!acc) return;

    // High-value transaction check
    if (txn.amount > 50000) {
        strcat(alerts, "\"High-value transaction\"");
    }

    // Rapid multiple transactions check
    int recentCount = 0;
    for (int i = txnCount - 1; i >= 0; i--) {
        if (transactions[i].accNo == txn.accNo) {
            double diff = difftime(txn.timestamp, transactions[i].timestamp);
            if (diff <= 60) recentCount++;
        }
    }
    
    if (recentCount >= 3) {
        if (strlen(alerts) > 0) strcat(alerts, ",");
        strcat(alerts, "\"Rapid multiple transactions\"");
    }

    // Impossible travel check
    if (strlen(acc->lastLocation) > 0) {
        double diff = difftime(txn.timestamp, acc->lastTxnTime);
        if (strcmp(acc->lastLocation, txn.location) != 0 && diff < 600) {
            if (strlen(alerts) > 0) strcat(alerts, ",");
            strcat(alerts, "\"Impossible travel\"");
        }
    }

    // Update account's last transaction info
    strcpy(acc->lastLocation, txn.location);
    acc->lastTxnTime = txn.timestamp;
}

void performTransaction(int accNo, double amount, const char *location) {
    if (txnCount >= MAX_TRANSACTIONS) { 
        printf("{\"alerts\":[]}"); 
        return; 
    }

    Transaction txn;
    txn.txnId = txnCount + 1;
    txn.accNo = accNo;
    txn.amount = amount;  // Fixed typo
    strcpy(txn.location, location);
    txn.timestamp = time(NULL);

    // Check sufficient balance
    int accountFound = 0;
    for (int i = 0; i < MAX_ACCOUNTS; i++) {
        if (accounts[i].accNo == accNo) {
            accountFound = 1;
            if (accounts[i].balance >= amount) {
                accounts[i].balance -= amount;
                transactions[txnCount++] = txn;
                
                char alerts[1024];
                checkFraud(txn, alerts);
                printf("{\"alerts\":[%s]}", alerts);
            } else {
                printf("{\"alerts\":[\"Insufficient balance\"]}");
            }
            break;
        }
    }
    
    if (!accountFound) {
        printf("{\"alerts\":[\"Account not found\"]}");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("{\"alerts\":[\"Usage: ./fraudBackend <accNo> <amount> <location>\"]}");
        return 1;
    }

    setupAccounts();
    int accNo = atoi(argv[1]);
    double amount = atof(argv[2]);
    char *location = argv[3];

    performTransaction(accNo, amount, location);
    return 0;
}
