#include "accounts.h"
#include "transactions.h"
#include <iostream>
#include <future>
#include <thread>
#include <random>
#include <memory>
#include <format>
#include <random>
#include <chrono>
#include <queue>

int main() 
{
    std::vector<std::unique_ptr<Account>> accounts;
    TransactionManager tmanager;
    TransactionLogs tlogs;

    int mode = 0;
    std::cout << "For interactive mode, select 1. For simulation mode, select 2. \n";
    std::cin >> mode;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if(mode == 1)
    {
        std::cout << "Welcome to the bank! \nLet us begin by setting up an account- \n";
        std::cout << "Enter account holder name: \n";
        std::string name;
        std::getline(std::cin, name);

        int acc_type;
        std::cout << "Enter account type (Press 1 for checking account, 2 for savings account)\n";
        std::cin >> acc_type;

        std::cout << "Enter initial balance: \n";
        double initial_bal;
        std::cin >> initial_bal;

        long int acc_num_cur = 10000;

        if(acc_type == 1)
            accounts.emplace_back(std::make_unique<Checking>(1000, acc_num_cur, name, initial_bal));
        else if(acc_type == 2)
            accounts.emplace_back(std::make_unique<Savings>(5, 5000, acc_num_cur, name, initial_bal));

        std::cout << std::format("Your account was successfully created! Your account number is- {} \n \n", acc_num_cur);
        acc_num_cur++;
        int option;

        while(true)
        {
            std::cout << "\nOptions:\n";
            std::cout << "1. Create a new account\n";
            std::cout << "2. Make a transaction\n";
            std::cout << "3. View transaction logs\n";
            std::cout << "4. Exit\n";
            std::cout << "Choose an option: \n";
            std::cin >> option;

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (option == 1)
            {
                std::cout << "Enter account holder name: \n";
                std::string name;
                std::getline(std::cin, name);

                int acc_type;
                std::cout << "Enter account type (Press 1 for checking account, 2 for savings account)\n";
                std::cin >> acc_type;

                std::cout << "Enter initial balance: \n";
                double initial_bal;
                std::cin >> initial_bal;

                if(acc_type == 1)
                    accounts.emplace_back(std::make_unique<Checking>(1000, acc_num_cur, name, initial_bal));
                else if(acc_type == 2)
                    accounts.emplace_back(std::make_unique<Savings>(5, 5000, acc_num_cur, name, initial_bal));

                std::cout << std::format("Your account was successfully created! Your account number is- {} \n \n", acc_num_cur);
                acc_num_cur++;
            } 

            else if (option == 2) 
            {
                int transaction_option;
                std::cout << "Press 1 to make a deposit, 2 to make a withdrawal and 3 to make a transaction between two accounts\n";
                std::cin >> transaction_option;

                if(transaction_option == 1)
                {
                    std::cout << "Enter account number- \n";
                    int source;
                    std::cin >> source;
                    int source_location = source % 10000;
                    
                    int deposit_amount;
                    std::cout << "Enter deposit amount-\n";
                    std::cin >> deposit_amount;

                    std::unique_ptr<TransactionBase> ptr_trans1 = std::make_unique<SelfTransaction<decltype(*(accounts[source_location]))>> 
                    (*(accounts[source_location]), deposit_amount);

                    tmanager.enqueue_transaction(std::move(ptr_trans1));
                    tlogs.record_transaction(source, source, deposit_amount);
                }

                else if(transaction_option == 2)
                {
                    std::cout << "Enter account number- \n";
                    int source;
                    std::cin >> source;
                    int source_location = source % 10000;
                    
                    int withdrawal_amount;
                    std::cout << "Enter withdrawal amount- \n";
                    std::cin >> withdrawal_amount;

                    std::unique_ptr<TransactionBase> ptr_trans1 = std::make_unique<SelfTransaction<decltype(*(accounts[source_location]))>> 
                    (*(accounts[source_location]), (-1)*withdrawal_amount);

                    tmanager.enqueue_transaction(std::move(ptr_trans1));
                    tlogs.record_transaction(source, source, withdrawal_amount);
                }
                
                else if(transaction_option == 3)
                {
                    std::cout << "Enter source account number- \n";
                    int source;
                    std::cin >> source;

                    std::cout << "Enter target account number- \n";
                    int target;
                    std::cin >> target;

                    double transfer_amount;
                    std::cout << "Enter transfer amount-\n";
                    std::cin >> transfer_amount;

                    int source_location = source % 10000;
                    int target_location = target % 10000;
                    
                    std::unique_ptr<TransactionBase> ptr_trans1 = std::make_unique<Transaction<decltype(*(accounts[source_location])), decltype(*(accounts[target_location]))>> 
                    (*(accounts[source_location]), *(accounts[target_location]), transfer_amount);
                    
                    tmanager.enqueue_transaction(std::move(ptr_trans1));
                    tlogs.record_transaction(source, target, transfer_amount);
                }
                
            } 
            
            else if (option == 3) 
            {
                tlogs.display_transactions();
            }
            
            
            else if (option == 4) 
            {
                for(auto& acc : accounts)
                {
                    std::cout << *acc << std::endl;
                }
                std::cout << "Exiting...\n";
                break;
            } 
            
            else 
            {
                std::cout << "Invalid option. Please choose a valid option.\n";
            }
        }
    }

    else if(mode == 2)
    {
        accounts.emplace_back(std::make_unique<Checking>(1000, 10000, "Jack", 45236));
        accounts.emplace_back(std::make_unique<Checking>(500, 10001, "Henry", 22345));
        accounts.emplace_back(std::make_unique<Savings>(7, 9999, 10002, "Gauss", 63462));
        std::vector<int> elements = {10000, 10001, 10002};
        int num_transactions = 1;
        while(num_transactions <= 50)
        {
            std::random_device rd;
            std::mt19937 generator(rd());
            std::shuffle(elements.begin(), elements.end(), generator);
            int source_account_num = elements[0];
            int target_account_num = elements[1];
            std::uniform_int_distribution<int> distribution(1, 100);

            double transfer_amount = distribution(generator);
            int source_location = source_account_num % 10000;
            int target_location = target_account_num % 10000;
                    
            std::unique_ptr<TransactionBase> ptr_trans1 = std::make_unique<Transaction<decltype(*(accounts[source_location])), decltype(*(accounts[target_location]))>> 
            (*(accounts[source_location]), *(accounts[target_location]), transfer_amount);

            tmanager.enqueue_transaction(std::move(ptr_trans1));
            tlogs.record_transaction(source_account_num, target_account_num, transfer_amount);

            num_transactions++;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        tlogs.display_transactions();
        for(auto& acc : accounts)
        {
            std::cout << *acc << std::endl;
        }
    }
    

    return 0;
}