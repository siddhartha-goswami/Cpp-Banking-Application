#include "accounts.h"
#include "transactions.h"
// #include "transactionlog.h"
#include <iostream>
#include <future>
#include <thread>
#include <random>
#include <memory>
#include <queue>

int main() 
{
    //TransactionLogs logs;
    std::vector<std::unique_ptr<Account>> accounts;
    std::queue<std::unique_ptr<TransactionBase>> transaction_queue;

    std::cout << "Welcome to the bank! \n";
    int option;
    long int acc_num_cur = 10000;

    while(true)
    {
        std::cout << "\nOptions:\n";
        std::cout << "1. Create a new account\n";
        std::cout << "2. Queue a transaction\n";
        std::cout << "3. Execute transaction queue\n";
        std::cout << "4. View transaction logs\n";
        std::cout << "5. Exit\n";
        std::cout << "Choose an option: ";
        std::cin >> option;

        if (option == 1)
        {
            std::cout << "Enter account holder name: \n";
            std::string name;
            std::cin >> name;

            int acc_type;
            std::cout << "Enter account type (Press 1 for checking account, 2 for savings account)\n";
            std::cin >> acc_type;

            std::cout << "Enter initial balance: \n";
            int initial_bal;
            std::cin >> initial_bal;

            if(acc_type == 1)
            accounts.push_back(std::make_unique<Checking>(100, acc_num_cur, name, initial_bal));

            else if(acc_type == 2)
            accounts.push_back(std::make_unique<Savings>(5, 5000, acc_num_cur, name, initial_bal));

            acc_num_cur++;
        } 

        else if (option == 2) 
        {
            std::cout << "Enter source account number- \n";
            int source;
            std::cin >> source;

            std::cout << "Enter target account number- \n";
            int target;
            std::cin >> target;

            int source_location = source % 10000;
            int target_location = target % 10000;
            
            std::unique_ptr<TransactionBase> ptr_trans = std::make_unique<Transaction<decltype(*(accounts[source_location])), decltype(*(accounts[target_location]))>> 
            (*(accounts[source_location]), *(accounts[target_location]), 10.0);
            
            transactions.push_back(std::move(ptr_trans));
        } 
        
        else if (option == 3) 
        {
            int trans_num = 1;
            for(auto& tr : transactions)
            {
                std::future<bool> res_fut = tr->execute_transaction();
                if(res_fut.get())
                {
                    std::cout << "Transaction " << trans_num << " successful" << std::endl;
                }
            }
        } 
        
        else if (option == 5) 
        {
            // Exit the program
            for(auto& acc : accounts)
            {
                std::cout << *acc;
            }
            std::cout << "Exiting...\n";
            break;
        } 
        
        else 
        {
            std::cout << "Invalid option. Please choose a valid option.\n";
        }
    }

    return 0;
}
