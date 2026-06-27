#include <iostream>
#include <vector>

int int_parser(const char *, int &);
void resp_to_text(const char *, std::vector<std::string> &);
std::string text_to_resp(const std::string);
std::string bulk_string_resp(const std::string &value);
std::string integer_resp(int value);
std::string longlong_resp(long long value);