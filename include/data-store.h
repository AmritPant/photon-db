#include <string>

void set_key_type(const std::string &key, const std::string &type);
std::string get_key_type(const std::string &key);
void set_string_value(const std::string &key, const std::string &value);
bool get_string_value(const std::string &key, std::string &value);
int delete_key(const std::string &key);
