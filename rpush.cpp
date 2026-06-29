#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <unordered_map>

class StorageEngine {
public:
    std::string rpush(const std::vector<std::string>& args);

private:
    std::unordered_map<std::string, std::string> store;
    std::unordered_map<std::string, std::deque<std::string>> lists;
};

std::string StorageEngine::rpush(const std::vector<std::string>& args) {

    if (args.size() < 2) {
        return "-ERR wrong number of arguments for 'rpush'\r\n";
    }

    const std::string& key = args[0];

    if (store.count(key)) {
        return "-WRONGTYPE Operation against a key holding the wrong kind of value\r\n";
    }

    for (size_t i = 1; i < args.size(); i++) {
        lists[key].push_back(args[i]);
    }

    return ":" + std::to_string(lists[key].size()) + "\r\n";
}

std::string dispatch(const std::string& cmd,
                     const std::vector<std::string>& args,
                     StorageEngine& engine) {
    if (cmd == "RPUSH") {
        return engine.rpush(args);
    }
    return "-ERR unknown command\r\n";
}

int main() {
    StorageEngine engine;

    
    std::vector<std::string> args1 = {"mylist", "a"};
    std::cout << "STAGE 1\n";
    std::cout << "RPUSH mylist a  →  " << dispatch("RPUSH", args1, engine);
    
    
    std::cout << "\nSTAGE 2\n";
    std::vector<std::string> args2 = {"mylist", "b"};
    std::cout << "RPUSH mylist b  →  " << dispatch("RPUSH", args2, engine);
    

    std::vector<std::string> args3 = {"mylist", "c"};
    std::cout << "RPUSH mylist c  →  " << dispatch("RPUSH", args3, engine);
    

    
    std::cout << "\nSTAGE 3\n";
    std::vector<std::string> args4 = {"mylist2", "x", "y", "z"};
    std::cout << "RPUSH mylist2 x y z  →  " << dispatch("RPUSH", args4, engine);
   

    std::vector<std::string> args5 = {"mylist2", "p", "q"};
    std::cout << "RPUSH mylist2 p q    →  " << dispatch("RPUSH", args5, engine);
    
    return 0;
}