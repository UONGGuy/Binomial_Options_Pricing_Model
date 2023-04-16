#include "BOPM.h"
#include <array>
#include <iostream>

int main()
{
    std::array<double, 7> strikes{ { 44, 46, 48, 50, 52, 54, 56 } };
    std::array<double, 7> sigma{
        { 0.588, 0.666, 0.695, 0.692, 0.661, 0.600, 0.490 } };

    double S = 50.0;
    double T = 1;
    int N = 1;
    double r = 0.01;

    std::array<double, 7> calls_expect{
        { 16.638, 17.602, 17.547, 16.808, 15.43, 13.308, 9.8849 } };
    std::array<double, 7> puts_expect{
        { 10.2, 13.144, 15.069, 16.31, 16.913, 16.771, 15.328 } };

    std::cout << "TEST EUROPEAN\n\n";
    European callEU(Option::call, strikes[0], T, S, r);
    callEU.printProperties();
    callEU.printBinomialTree(sigma[0], 3);
    std::cout << '\n';

    European putEU(Option::put, strikes[0], T, S, r);
    putEU.printProperties();
    putEU.printBinomialTree(sigma[0], 3);

    for (auto i{ 0 }; i < strikes.size(); ++i)
    {
        European callEU(Option::call, strikes[i], T, S, r);
        double callValue{ callEU.getBinomialValue(sigma[i], N) };
        std::cout << "CALL:\t" << "Calculated: " << callValue << "\tExpected: " << calls_expect[i] << '\n';

        European putEU(Option::put, strikes[i], T, S, r);
        double putValue{ putEU.getBinomialValue(sigma[i], N) };
        std::cout << "PUT:\t" << "Calculated: " << putValue << "\tExpected: " << puts_expect[i] << '\n';
    }

    std::cout << '\n';

    return 0;
}
