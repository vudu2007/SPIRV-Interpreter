module;
#include <cassert>
#include <string>

export module utils;

export namespace Utils {
    template<typename S>
    class May {
        S* val;
        std::string msg;

        May(S* val, std::string msg): val(val), msg(msg) {}
    public:
        static May some(S val) {
            return May(&val, "");
        }
        static May none(std::string str) {
            return May(nullptr, str);
        }
        static May none() {
            return May(nullptr, "");
        }

        bool is() {
            return val != nullptr;
        }
        S& get() {
            assert(val != nullptr);
            return *val;
        }
        std::string& str() {
            assert(val == nullptr);
            return msg;
        }

        /// @brief Deletes the data stored in the May
        /// May does *not* own the data, but if you want to delete it, this will and turn itself into none
        void del() {
            assert(val != nullptr);
            delete val;
            val = nullptr;
            msg = "deleted";
        }
    };

};
