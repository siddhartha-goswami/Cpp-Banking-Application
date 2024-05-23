#ifndef ACC_H
#define ACC_H

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
    std::string accType = "";

    template<typename FromAcc, typename ToAcc>
    friend class Transaction;

    public:

    Account() {}

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

    virtual bool deposit(double amount)
    {
        return false;
    }

    virtual bool withdraw(double amount)
    {
        return false;
    }

    double get_balance() const
    {
        std::lock_guard lock_account(account_balance_m);
        return account_balance;
    }

    void set_details(int acc_num, std::string name, double initial_bal)
    {
        this->account_number = acc_num;
        this->account_holder_name = name;
        this->account_balance = initial_bal;
    }
};

class Checking : public Account
{
    private:
    double overdraft_limit = 1000;
    std::string accType = "Savings";

    template<typename FromAcc, typename ToAcc>
    friend class Transaction;

    public:

    Checking() {}

    template<typename... Args>
    Checking(Args&&... args) : Account(std::forward<Args>(args)...) {}

    bool withdraw(double amount) override
    {
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

    bool deposit(double amount) override
    {
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
};

class Savings : public Account
{
    private:
    double interest_rate = 3;
    double withdraw_limit = 1000;
    std::string accType = "Savings";

    template<typename FromAcc, typename ToAcc>
    friend class Transaction;

    public:

    Savings() {}

    template<typename... Args>
    Savings(Args&&... args) : Account(std::forward<Args>(args)...) {}

    bool withdraw(double amount) override
    {
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

    bool deposit(double amount) override
    {
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

    void calc_add_interest()
    {
        std::lock_guard lock_account(account_balance_m);
        account_balance += account_balance * interest_rate/100;
    }
};

class FixedDeposit : public Account
{
    private:
    double interest_rate = 7;
    std::string accType = "FixedDeposit";

    template<typename FromAcc, typename ToAcc>
    friend class Transaction;

    public:

    FixedDeposit() {}

    template<typename... Args>
    FixedDeposit(Args&&... args) : Account(std::forward<Args>(args)...) {}

    bool deposit(double amount)
    {
        return false;
    }

    bool withdraw(double amount) override
    {
        return false;
    }

    void calc_add_interest()
    {
        std::lock_guard lock_account(account_balance_m);
        account_balance += account_balance * interest_rate/100;
    }
};

#endif