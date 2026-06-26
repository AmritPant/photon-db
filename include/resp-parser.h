#include <iostream>
#include <vector>

int int_parser(const char *, int &);
void resp_to_text(const char *, std::vector<std::string> &);
std::string text_to_resp(const std::string);
std::string bulk_text_to_resp(const std::string &);
std::string null_bulk_to_resp();
std::string integer_to_resp(int);
std::string error_to_resp(const std::string &);

//done via nvim in tmux tiling method
