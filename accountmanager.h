#include <vector>
#include <memory>
#include "accounts.h"

class AccountManager
{
    public:
    std::vector<std::unique_ptr<Account>> accounts;

    template<typename AccType>
    void createAccount()
    {
        accounts.emplace_back(std::make_unique<AccType>();
    }

    template<typename...AccTypes>
    void getAccountInfo()
    {

    }
};