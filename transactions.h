#include <string>
#include <mutex>
#include <iostream>
#include <future>
#include <condition_variable>
#include <stdlib.h>
#include <thread>
#include <chrono>
#include <vector>
#include <shared_mutex>
#include <memory>
#include <queue>
#include <atomic>

class TransactionBase
{
    public:
    static int current_transaction_number;
    virtual ~TransactionBase() = default;
    virtual bool execute_transaction() = 0;
    virtual int get_transaction_number() = 0;
};

int TransactionBase::current_transaction_number = 1;

template<typename FromAcc, typename ToAcc>
class Transaction : public TransactionBase
{
    public:
        FromAcc& source_acc;
        ToAcc& target_acc;
        double transaction_amount;
        int transaction_number;

    public:
        Transaction(FromAcc& from, ToAcc& to, double const t_amount)
        : source_acc(from), target_acc(to), transaction_amount(t_amount) 
        {
            transaction_number = TransactionBase::current_transaction_number;
            TransactionBase::current_transaction_number++;
        }

        bool execute_transaction()
        {
            std::scoped_lock lock(source_acc.account_balance_m, target_acc.account_balance_m);
            std::this_thread::sleep_for(std::chrono::seconds(5));
            bool withdraw_success = source_acc.withdraw(transaction_amount);
            bool deposit_success = target_acc.deposit(transaction_amount);
            bool transaction_success = withdraw_success && deposit_success;

            return transaction_success;
        }

        int get_transaction_number()
        {
            return this->transaction_number;
        }
};

template<typename Acc>
class SelfTransaction : public TransactionBase
{
    public:
        Acc& source_acc;
        double transaction_amount;
        int transaction_number;

    public:
        SelfTransaction(Acc& source, double const t_amount)
        : source_acc(source), transaction_amount(t_amount) 
        {
            transaction_number = TransactionBase::current_transaction_number;
            TransactionBase::current_transaction_number++;
        }

        bool execute_transaction()
        {
            std::scoped_lock lock(source_acc.account_balance_m);
            bool deposit_success;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            if(transaction_amount >= 0)
            {
                deposit_success = source_acc.deposit(transaction_amount);
            }

            else
            {
                deposit_success = source_acc.withdraw(transaction_amount);
            }

            return deposit_success;
        }

        int get_transaction_number()
        {
            return this->transaction_number;
        }
};

class TransactionManager
{
    public:
    std::vector<std::thread> transaction_threads;
    std::mutex queue_mutex;
    std::mutex cout_mutex;
    std::queue<std::unique_ptr<TransactionBase>> transaction_queue;
    std::condition_variable cv;
    bool stop_flag = false;
    std::atomic<int> num_transactions_tbe;
    std::atomic_flag atf = ATOMIC_FLAG_INIT;

    TransactionManager()
    {
        create_tm();
    }

    void create_tm()
    {
        stop_flag = false;

        for(int i = 0; i < 5; i++)
        {
            transaction_threads.emplace_back([this]()
            {
                while(true)
                {
                    std::unique_ptr<TransactionBase> curr_transaction;
                    
                    {
                        std::unique_lock<std::mutex> ulock(queue_mutex);
                        cv.wait(ulock, [this](){ return !transaction_queue.empty() || 
                        stop_flag;});
                        if (stop_flag && transaction_queue.empty()) 
                        {
                            return; 
                        }
                        curr_transaction = std::move(transaction_queue.front());
                        transaction_queue.pop();
                    }

                    bool transaction_res = curr_transaction->execute_transaction();

                    if(transaction_res)
                    {
                        std::unique_lock<std::mutex> ulock(cout_mutex);
                        std::cout << "Transaction " 
                        << curr_transaction->get_transaction_number() << " was successful\n";
                    }

                    num_transactions_tbe.fetch_sub(1, std::memory_order_release);

                    if(num_transactions_tbe.load(std::memory_order_acquire) == 0)
                    {
                        atf.test_and_set();
                        atf.notify_one();
                    }
                }
            }
            );
        }
    }

    void set_sim_trans(int set_num)
    {
        num_transactions_tbe.store(set_num, std::memory_order_release);
    }


    void complete_all_transactions()
    {
        atf.wait(false);

        {
            std::unique_lock<std::mutex> ulock(queue_mutex);
            stop_flag = true;
        }
        cv.notify_all();
        for(auto& thr : transaction_threads)
        {
            thr.join();
        }
    }

    void enqueue_transaction(std::unique_ptr<TransactionBase> transaction)
    {
        { 
            std::unique_lock<std::mutex> ulock(queue_mutex); 
            transaction_queue.emplace(std::move(transaction)); 
        } 
        cv.notify_one();
    }
};

class TransactionRep
{
    public:
    int source_acc_number;
    int target_acc_number;
    double transaction_amount;

    TransactionRep(int source_acc_num, int targ_acc_num, double trans_am) : source_acc_number(source_acc_num), 
    target_acc_number(targ_acc_num), transaction_amount(trans_am) {}
};


class TransactionLogs
{
    public:
    mutable std::vector<TransactionRep> transaction_list;
    mutable std::shared_mutex transact_m;

    void record_transaction(int source, int target, double trans_amount)
    {
        std::unique_lock<std::shared_mutex> lock(transact_m);
        transaction_list.emplace_back(TransactionRep(source, target, trans_amount));
    }

    void display_transactions() const
    {
        std::shared_lock<std::shared_mutex> lock(transact_m);
        int transaction_number = 1;
        for(TransactionRep& transaction : transaction_list)
        {
            std::cout << std::format("Transaction number- {} \n \n", transaction_number);
            std::cout << std::format("Source account: {} \n", transaction.source_acc_number);
            std::cout << std::format("Target account: {} \n", transaction.target_acc_number);
            std::cout << std::format("Transaction amount: {} \n \n", transaction.transaction_amount);
            transaction_number++;
        }
    }
};