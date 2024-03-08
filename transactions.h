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

class TransactionBase
{
    public:
    virtual ~TransactionBase() = default;
    virtual std::future<bool> execute_transaction() = 0;
};

template<typename FromAcc, typename ToAcc>
class Transaction : public TransactionBase
{
    public:
        FromAcc& source_acc;
        ToAcc& target_acc;
        double transaction_amount;

    public:
        Transaction(FromAcc& from, ToAcc& to, double const t_amount)
        : source_acc(from), target_acc(to), transaction_amount(t_amount) {}

        std::future<bool> execute_transaction()
        {

            std::promise<bool> completion_promise;
            std::future<bool> completion_fut = completion_promise.get_future();
            std::thread([&, completion_promise = std::move(completion_promise)]() mutable
            {
                std::scoped_lock lock(source_acc.account_balance_m, target_acc.account_balance_m);
                std::this_thread::sleep_for(std::chrono::seconds(10));
                bool withdraw_success = source_acc.withdraw(transaction_amount);
                bool deposit_success = target_acc.deposit(transaction_amount);
                bool transaction_success = withdraw_success && deposit_success;
                completion_promise.set_value(transaction_success);
            }).detach();

            return completion_fut;
        }
};

template<typename Acc>
class SelfTransaction : public TransactionBase
{
    public:
        Acc& source_acc;
        double transaction_amount;

    public:
        SelfTransaction(Acc& source, double const t_amount)
        : source_acc(source), transaction_amount(t_amount) {}

        std::future<bool> execute_transaction()
        {

            std::promise<bool> completion_promise;
            std::future<bool> completion_fut = completion_promise.get_future();
            std::thread([&, completion_promise = std::move(completion_promise)]() mutable
            {
                std::scoped_lock lock(source_acc.account_balance_m);
                std::this_thread::sleep_for(std::chrono::seconds(10));
                bool deposit_success;
                if(transaction_amount >= 0)
                {
                    deposit_success = source_acc.deposit(transaction_amount);
                }

                else
                {
                    deposit_success = source_acc.withdraw(transaction_amount);
                }

                completion_promise.set_value(deposit_success);
            }).detach();

            return completion_fut;
        }
};

class TransactionManager
{
    public:
    std::queue<std::unique_ptr<TransactionBase>> transaction_queue;
    std::mutex queue_mutex;
    std::condition_variable queue_condition;

    TransactionManager()
    {
        std::thread exec_thread(transaction_executer, this);
        exec_thread.detach();
    }

    void enqueue_transaction(std::unique_ptr<TransactionBase> transaction)
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        transaction_queue.push(std::move(transaction));
        queue_condition.notify_one();
    }

    void transaction_executer()
    {
        while(true)
        {
            std::unique_lock<std::mutex> trans_queue_lock(queue_mutex);
            queue_condition.wait(trans_queue_lock, [this](){return !transaction_queue.empty();});

            auto transaction = std::move(transaction_queue.front());
            transaction_queue.pop();

            trans_queue_lock.unlock();

            std::future<bool> res_fut = transaction->execute_transaction();
            if(res_fut.get())
            {
                std::cout << "\nTransaction successful" << std::endl;
            }
        }
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
            std::cout << "Transaction number- " << transaction_number << "\n \n";
            std::cout << "Source account: " << transaction.source_acc_number << "\n";
            std::cout << "Target account: " << transaction.target_acc_number << "\n";
            std::cout << "Transaction amount: " << transaction.transaction_amount << "\n \n";
            transaction_number++;
        }
    }
};


