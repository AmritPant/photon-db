

std::unordered_map<std::string, std::string> &get_store() {
    static std::unordered_map<std::string, std::string> store;
    return store;
}