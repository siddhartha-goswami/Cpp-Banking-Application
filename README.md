# Cpp_Banking_Application
Banking application in C++

Overview-

This application in C++ implements a number of C++ features to simulate a bank. Users can create three different types of accounts- checking, savings and fixed deposits, perform transactions between different accounts, view the logs of transactions, add interest to their accounts and search for transactions from the logs. Each type of account offers different features such as transaction availability or interest rate. Accounts of checking or savings type can support deposits, withdrawals or inter-account transactions. Transaction requests can be carried out concurrently and transactions are logged to view as a transaction history. Users can filter and view transactions through their username or account numbers. Lastly, users can view a summary of account details before exiting the application.

How it works-

The application has two modes- interactive and simulation. In interactive mode, users create accounts of their choice by using an abstract factory pattern. Users can then create transactions involving deposits or withdrawals from individual accounts or transactions between multiple accounts based on the type traits of accounts. Each transaction is added to the queue of transactions, and each time a transaction is enqueued, a thread is notified of the incoming transaction request. The transaction is executed and a success/failure message is delivered. The application also logs transactions to a file. Users can also add interest to eligible accounts. Users have a search functionality where regex is used to find transactions involving specific account usernames or numbers and the relevant transactions are displayed in a file. In simulation mode, the application adds a set number of transactions to the queue and the threads work to execute all valid transaction requests on the queue. After all transactions are completed, a log of transactions and a summary of accounts is provided to the console output.

Key concepts of C++ used in the project-

Object oriented programming- Accounts are defined using a class and inheritance based model to create specialized accounts

Abstract factory pattern- The abstract factory pattern is used to create accounts of different types

Concurrency- Transactions are carried out concurrently using multiple threads and a queue of transactions along with a number of techniques such as mutex, condition variables, atomic flags and memory access ordering

Type traits and concepts- These are used for compile-time template programming on class templates and template functions

RAII- RAII classes are used to perform release of resources upon destruction

Regular expressions- Regex is used to search and filter specific transactions based on username or account number