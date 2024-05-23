#ifndef ACCMAN_H
#define ACCMAN_H

#include <vector>
#include <memory>
#include "accounts.h"

class AccountFactory
{
    public:
    virtual std::unique_ptr<Account> create_account() = 0;
};

class CheckingAccountFactory : public AccountFactory
{
    public:
    std::unique_ptr<Account> create_account() override
    {
        return std::make_unique<Checking>();
    }
};

class SavingsAccountFactory : public AccountFactory
{
    public:
    std::unique_ptr<Account> create_account() override
    {
        return std::make_unique<Savings>();
    }
};

class FixedDepositFactory : public AccountFactory
{
    public:
    std::unique_ptr<Account> create_account() override
    {
        return std::make_unique<FixedDeposit>();
    }
};

class AccountManager
{
    public:
    std::vector<std::unique_ptr<Account>> accounts;
    std::unique_ptr<AccountFactory> checkingFactory;
    std::unique_ptr<AccountFactory> savingsFactory;
    std::unique_ptr<AccountFactory> fixedDepositFactory;

    AccountManager()
    {
        checkingFactory = std::make_unique<CheckingAccountFactory>();
        savingsFactory = std::make_unique<SavingsAccountFactory>();
        fixedDepositFactory = std::make_unique<FixedDepositFactory>();
    }

    template <typename AccType>
    Account& add_account()
    {
        if constexpr (std::is_same_v<AccType, Checking>)
        {
            accounts.emplace_back(checkingFactory->create_account());
        }
        else if constexpr (std::is_same_v<AccType, Savings>)
        {
            accounts.emplace_back(savingsFactory->create_account());
        }
        else if constexpr (std::is_same_v<AccType, FixedDeposit>)
        {
            accounts.emplace_back(fixedDepositFactory->create_account());
        }

        return *(accounts.back());
    }

    Account& get_account(int acc_num)
    {
        int acc_location = acc_num % 10000;
        return *(accounts[acc_location]);
    }
};

template <typename... AccTypes>
void displayAccInfo(AccTypes... args)
{
    
}

#endif