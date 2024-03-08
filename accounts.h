#include <string>
#include <iostream>
#include <utility>
#include <mutex>
#include <chrono>

class Account
{
    public:
    int account_number;
    std::string account_holder_name;
    double account_balance;
    mutable std::mutex account_balance_m;

    template<typename FromAcc, typename ToAcc>
    friend class Transaction;

    public:

    Account(int acc_num, std::string acc_holder, double acc_bal) : 
    account_number(acc_num), account_holder_name(acc_holder), account_balance(acc_bal) {}

    Account(Account& other_acc) = delete;
    Account& operator=(Account& other_acc) = delete;
    
    Account(Account&& other_acc) noexcept : account_number(std::move(other_acc.account_number)),
    account_holder_name(std::move(other_acc.account_holder_name)), account_balance(other_acc.account_balance)
    {
        other_acc.account_balance = 0.0;
        other_acc.account_number = 0;
    }

    Account& operator=(Account&& other_acc) noexcept
    {
        if(this != &other_acc)
        {
            account_number = other_acc.account_number;
            account_holder_name = std::move(other_acc.account_holder_name);
            account_balance = other_acc.account_balance;
            other_acc.account_balance = 0.0;
        }

        return *this;
    }

    virtual ~Account() = default;

    friend std::ostream& operator<<(std::ostream& os, Account& account)
    {
        os<< "Account number: " << account.account_number <<std::endl 
        <<"Account holder name: " << account.account_holder_name <<std::endl 
        <<"Account balance: " << account.account_balance <<std::endl;

        return os;
    }

    bool deposit(double amount)
    {
        //std::lock_guard lock_account(account_balance_m);
        if(amount >= 0)
        {
            account_balance += amount;
            return true;
        }

        else
        {
            std::cout << "Cannot deposit negative amount" << std::endl;
            return false;
        }
    }

    virtual bool withdraw(double amount) = 0;

    double get_balance() const
    {
        std::lock_guard lock_account(account_balance_m);
        return account_balance;
    }
};

class Checking : public Account
{
    private:
    double overdraft_limit;

    template<typename FromAcc, typename ToAcc>
    friend class Transaction;

    public:

    template<typename... Args>
    Checking(double overdraft_lim, Args&&... args) :
    overdraft_limit(overdraft_lim), Account(std::forward<Args>(args)...) {}

    bool withdraw(double amount) override
    {
        //std::lock_guard lock_account(account_balance_m);
        if(amount <= account_balance)
        {
            account_balance -= amount;
            return true;
        }

        else
        {
            std::cout << "Insufficient funds" << std::endl;
            return false;
        }
    }
};

class Savings : public Account
{
    private:
    double interest_rate;
    double withdraw_limit;

    template<typename FromAcc, typename ToAcc>
    friend class Transaction;

    public:

    template<typename... Args>
    Savings(double rate, double withdraw_lim, Args&&... args) :
    interest_rate(rate), withdraw_limit(withdraw_lim), Account(std::forward<Args>(args)...) {}

    bool withdraw(double amount) override
    {
        //std::lock_guard lock_account(account_balance_m);
        if(amount <= account_balance && amount <= withdraw_limit)
        {
            account_balance -= amount;
            return true;
        }

        else
        {
            if(amount > account_balance)
            std::cout << "Insufficient funds" << std::endl;

            else if(amount > withdraw_limit)
            std::cout << "Withdraw limit exceeded" << std::endl;

            return false;
        }
    }

    void calc_add_interest()
    {
        std::lock_guard lock_account(account_balance_m);
        account_balance += account_balance * interest_rate/100;
    }
};