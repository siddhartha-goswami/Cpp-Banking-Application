#ifndef ACCTRAITS_H
#define ACCTRAITS_H

#include <type_traits>
#include "accounts.h"

template<typename T>
struct allows_transaction : public std::false_type {};

template<>
struct allows_transaction<Checking> : public std::true_type {};

template<>
struct allows_transaction<Savings> : public std::true_type {};

template<>
struct allows_transaction<FixedDeposit> : public std::false_type {};

template<typename T>
struct bears_interest : public std::false_type {};

template<>
struct bears_interest<Checking> : public std::false_type {};

template<>
struct bears_interest<Savings> : public std::true_type {};

template<>
struct bears_interest<FixedDeposit> : public std::true_type {};

template<typename T>
concept BearsInterest = (bears_interest<T>::value);

#endif