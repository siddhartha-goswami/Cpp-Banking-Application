#include <string>
#include <vector>
#include <shared_mutex>
#include <memory>

class TransactionLogs
{
    public:
    std::vector<std::unique_ptr<TransactionBase>> transaction_list;
    mutable std::shared_mutex transact_m;

    public:

    template<typename FromAcc, typename ToAcc>
    void record_transaction(Transaction<FromAcc, ToAcc>& transaction)
    {
        std::unique_lock<std::shared_mutex> lock(transact_m);
        transaction_list.push_back(transaction);
    }

    template<typename FromAcc, typename ToAcc>
    void display_transactions() const
    {
        std::shared_lock<std::shared_mutex> lock(transact_m);
        for(auto& transaction : transaction_list)
        {
            std::cout << "Source account: \n" << transaction->source_acc << std::endl;
            std::cout << "Target account: \n" << transaction->target_acc << std::endl;
            std::cout << "Transaction amount: " << transaction->transaction_amount << std::endl;
        }
    }
};