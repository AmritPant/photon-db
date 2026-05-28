#include "../include/resp-parser.h"
#include <string.h>

// *2\r\n$4\r\nECHO\r\n$5\r\nhello\r\n
/*
    - If first character is '*' -> Array
    - if first character is '+' --> String
*/

int int_parser(const char *message, int &position) {
    int value = 0;

    while (message[position] >= '0' && message[position] <= '9') {
        value = value * 10 + (message[position] - '0');
        position++;
    }

    // Skipping the 2 character /r/n
    if (message[position] == '\r' && message[position + 1] == '\n') {
        position = position + 2;
    }

    return value;
}

void resp_to_text(const char *client_message, std::vector<std::string> &parsed_message) {
    // Skipping the first '*' symbol
    int current_position = 1;

    // so accessing the number of array value
    int no_of_elements = int_parser(client_message, current_position);

    // Returning in the case of Empty Array and Null Array
    if (no_of_elements <= 0)
        return;

    // Accessing each individual String
    for (int i = 0; i < no_of_elements; i++) {
        // Skipping '$'
        current_position += 1;
        int string_length = int_parser(client_message, current_position);

        std::string command(client_message + current_position, string_length);
        parsed_message.push_back(command);

        // Skipping 2 /r/n
        current_position += string_length + 2;
    }
}

std::string text_to_resp(const std::string text) {
    std::string resp;
    resp.append("+");
    resp.append(text);
    resp.append("\r\n");

    return resp;
}
