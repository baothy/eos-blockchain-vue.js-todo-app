#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

#include <string>

using namespace eosio;
using std::string;

class todo : public contract
{
  public:
    todo(account_name self) : contract(self) {}

    // @abi action
    void get(uint64_t id);

    // @abi action
    void create(const account_name account,
                const string &content);

    // @abi action
    void update(const account_name account,
                const string &content,
                uint64_t id,
                uint64_t isDone);

    // @abi action
    void remove(uint64_t id,
                const account_name account);

  private:
    static uint64_t id;

    struct config
    {
        config() {}
        constexpr static uint64_t key = N(config);
        uint64_t id = 1000;
    };

    void store_config(const config &conf)
    {
        auto it = db_find_i64(_self, _self, N(config), config::key);
        if (it != -1)
        {
            db_update_i64(it, _self, (const char *)&conf, sizeof(config));
        }
        else
        {
            db_store_i64(_self, N(config), _self, config::key, (const char *)&conf, sizeof(config));
        }
    }

    bool get_config(config &conf)
    {
        auto it = db_find_i64(_self, _self, N(config), config::key);
        if (it != -1)
        {
            auto size = db_get_i64(it, (char *)&conf, sizeof(config));
            eosio_assert(size == sizeof(config), "Wrong record size");
            return true;
        }
        return false;
    }

    // @abi table tasks i64
    struct task
    {
        uint64_t id;
        account_name account;
        string content;
        uint64_t isDone;

        uint64_t primary_key() const { return id; }
        EOSLIB_SERIALIZE(task, (id)(account)(content)(isDone))
    };

    typedef eosio::multi_index<N(tasks), task> task_table;
};

EOSIO_ABI(todo, (get)(create)(update)(remove))