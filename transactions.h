#include <string>
#include <mutex>
#include <future>
#include <condition_variable>
#include <stdlib.h>

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
                bool withdraw_success = source_acc.withdraw(transaction_amount);
                bool deposit_success = target_acc.deposit(transaction_amount);
                bool transaction_success = withdraw_success && deposit_success;
                std::cout << transaction_success;
                completion_promise.set_value(transaction_success);
            }).detach();

            return completion_fut;
        }
};