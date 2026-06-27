#pragma once
#include<iostream>
#include<string>
#include<chrono>
#include<optional>

std::string set_command_handler(const std::string &key, const std::string &value, 
    std::optional<std::string> flag = std::nullopt, std::optional<std::string> number = std::nullopt);