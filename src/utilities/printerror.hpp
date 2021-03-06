#pragma once

#include <sstream>

void printError(const std::exception& error, std::size_t depth = 0);

void printErrorToStream(
    const std::exception& error,
    std::stringstream& stream,
    std::size_t depth = 0);
