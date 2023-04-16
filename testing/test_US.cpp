#include "BOPM.h"
#include <array>
#include <cmath>
#include <iostream>

int main()
{
    double K{ 100 };
    double r = 0.01;
    double sigma{ log(1.07) * sqrt(r / log(1 + r))};
    double S = 100.0;
    int N = 3;
    double T = N / r * log(1 + r);

    double call_expect{ 6.57 };
    double put_expect{ 3.82 };

    std::cout << "TEST AMERICAN\n\n";
    American callUS(Option::call, K, T, S, r);
    callUS.printProperties();
    callUS.printBinomialTree(sigma, 3);

    double callValue{ callUS.getBinomialValue(sigma, N) };
    std::cout << "CALL:\t" << "Calculated: " << callValue << "\tExpected: " << call_expect << "\n\n";

    bool EEcallUS{ callUS.checkEarlyExercise(sigma, N) };
    std::cout << '\n';

    American putUS(Option::put, K, T, S, r);
    putUS.printProperties();
    putUS.printBinomialTree(sigma, 3);

    double putValue{ putUS.getBinomialValue(sigma, N) };
    std::cout << "PUT:\t" << "Calculated: " << putValue << "\tExpected: " << put_expect << "\n\n";

    bool EEputUS{ putUS.checkEarlyExercise(sigma, N) };
    std::cout << '\n';

    return 0;
}
