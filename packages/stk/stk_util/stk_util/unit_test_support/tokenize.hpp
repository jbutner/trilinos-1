#ifndef STK_IO_UTIL_TOKENIZE_H
#define STK_IO_UTIL_TOKENIZE_H

#include <string>
#include <vector>

namespace stk {
namespace io {
namespace util {

/**
 * Take the 'str' argument and split it using the list of characters
 * in separators as separators. Use tokens to return the result.
 */
void tokenize(const std::string& str, const std::string& separators,
              std::vector<std::string>& tokens);

}
}
}

#endif
