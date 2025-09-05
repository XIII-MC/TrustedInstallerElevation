#ifndef TRUSTEDINSTALLERELEVATION_INPUTUTILS_H
#define TRUSTEDINSTALLERELEVATION_INPUTUTILS_H

#include <string>
#include <iostream>


namespace InputUtils {

    inline std::string getUserInput(const std::string &prompt, const std::string& defaultValue) {

        std::string input;

        if (!defaultValue.empty()) {

            std::cout << prompt << " (default: " << defaultValue << "): ";

        } else {

            std::cout << prompt << ": ";

        }

        // Get the user input
        std::getline(std::cin, input);

        // Return the default value if input is empty
        if (input.empty()) {

            return defaultValue;

        }

        return input;

    }

} // namespace InputUtils

#endif // TRUSTEDINSTALLERELEVATION_INPUTUTILS_H
