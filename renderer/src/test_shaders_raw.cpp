#include <iostream>


int main()
{
    try {

    } catch (const std::exception &e) {
        std::cerr << "Exception in main: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}