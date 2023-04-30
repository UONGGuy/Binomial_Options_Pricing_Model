#include "BOPM.h"
#include <boost/math/special_functions/binomial.hpp>
#include <boost/math/special_functions/erf.hpp>
#include <boost/math/special_functions/math_fwd.hpp>
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

/* 
 * Option base class 
 */

Option::Option(Type type, double K, double T, double S, double r, double c)
    : type{ type }, K{ K }, T{ T }, S{ S }, r{ r }, c{ c }
{}

void Option::printProperties() const
{
    std::cout << "Type: " << this->type << '\n';
    std::cout << "Style: " << this->style << '\n';
    std::cout << "Strike: " <<  K << '\n';
    std::cout << "Time to expiry: " << T << " years\n";
    std::cout << "Current spot price: " << S << '\n';
    std::cout << "Risk-free interest rate: " << r << '\n';
    std::cout << "Dividend yield: " <<  c << '\n';
}

inline double Option::getPayoffValue(double S_t) const
{
    switch(type) {
        case Option::call:      return std::max(0.0, S_t - K);
        case Option::put:       return std::max(0.0, K - S_t);
        default:                break;
    }
    assert(0 && "Option::getPayoffValue(double) invalid Option type (call/put)");
    return -1.0;
}

double Option::getBinomialValue(double sigma, int N) const
{
    std::cout << "Option base class function getBinomialValue(double, int) called, invalid value used.\n";

    return -1.0;
}

void Option::printBinomialTree(double sigma, int N) const
{
    double up{ Option::getUpFactor(sigma, N) };
    double down{ 1 / up };
    double p_up{ Option::getUpProb(sigma, N) };
    double p_down{ 1 - p_up };

    for (int t{ 0 }; t <= N; ++t) {
        std::cout << "Timestep " << t << ":\n";
        std::vector<double> binTreeLevel{ Option::getBinTreeLevel(sigma, N, t) };

        for (const auto& node : binTreeLevel) {
            std::cout << node << "\t\t";
        }
        std::cout << "\n\n";
    }
}

bool Option::checkEarlyExercise(double sigma, int N) const
{
    std::cout << "Option base class fnction checkEarlyExercise(sigma, int) const called, invalid value used.\n";

    return false;
}

std::ostream& operator<<(std::ostream& out, const Option::Type& optType)
{
    switch(optType) {
        case Option::call:      out << "Call";  return out;
        case Option::put:       out << "Put";   return out;
        default:                out << "???";   break;
    }
    assert(0 && "Output operator<< of Option::Type invalid input");
    return out;
}

std::ostream& operator<<(std::ostream& out, const Option::Style& optStyle)
{
    switch(optStyle) {
        case Option::EU:        out << "European";      return out;
        case Option::US:        out << "American";      return out;
        default:                out << "???";           break;
    }
    assert(0 && "Output operator<< of Option::Style invalid input");
    return out;
}

inline double Option::getUpFactor(double sigma, int N) const
{
    return exp(sigma * sqrt(T / N));
}

inline double Option::getUpProb(double sigma, int N) const
{
    double up{ Option::getUpFactor(sigma, N) };
    double down{ 1 / up };
//    return (exp((r - c) * T / N) - down) / (up - down);
    return ((1 + r - down - c) / (up - down));
}

inline std::vector<double> Option::getBinTreeLevel(double sigma, int N, int t, bool exercise) const
{
    double up{ Option::getUpFactor(sigma, N) };
    double down{ 1 / up };

    std::vector<double> binTreeLevel(t + 1);

    for (int node{ 0 }; node <= t; ++node) {
        if (!exercise) {
            binTreeLevel[node] = S * pow(up, node) * pow(down, t - node);
        }
        else {
            binTreeLevel[node] = Option::getPayoffValue(S * pow(up, node) * pow(down, t - node));
        }
    }

    return binTreeLevel;
}

/* 
 * European
 */

European::European(Type type, double K, double T, double S, double r, double c)
    : Option(type, K, T, S, r, c)
{
    this->style = EU;
}

double European::getBinomialValue(double sigma, int N) const
{
    double optValue{ 0.0 };
    double up{ Option::getUpFactor(sigma, N) };
    double down{ 1 / up };
    double p_up{ Option::getUpProb(sigma, N) };
    double p_down{ 1 - p_up };

    assert(down < 1 + r && 1 + r < up && "getBinomialValue(double, int) BOPM no arbitrage violated, d < r < u does not hold.");
    assert(0 <= p_up && p_up <= 1 && "getBinomialValue(double, int) BOPM probability not between 0 and 1, invalid inputs.");

    for (int node{ 0 }; node <= N; ++node) {
        double exerciseValue{ getPayoffValue(S * pow(up, node) * pow(down, N - node)) };
        optValue += exerciseValue
            * boost::math::binomial_coefficient<double>(N, node)
            * pow(p_up, node) * pow(p_down, N - node);
    }

    return optValue / pow(1 + r, N);
}

bool European::checkEarlyExercise(double sigma, int N) const
{
    std::cout << "Early exercise not optimal.\n";

    return false;
}

double European::getBlackScholesValue(double sigma) const
{
    double d1 = (log(S / K) + (r - c + pow(sigma, 2) / 2) * T) / (sigma * sqrt(T));
    double d2 = d1 - sigma * sqrt(T);

    // N(d) = P(N(0, 1) <= d) by calculating normal CDF
    double N1 = 0.5 * boost::math::erfc(-(d1 - 0) / (1 * sqrt(2)));
    double N2 = 0.5 * boost::math::erfc(-(d2 - 0) / (1 * sqrt(2)));
    
    double C0 = S * exp(-c * T) * N1 - K * exp(-r * T) * N2;

    if (type == Option::Type::call) {
        return C0;
    }
    return C0 + K * exp(-r * T) - S * exp(-c * T);
}

/* 
 * American 
 */

American::American(Type type, double K, double T, double S, double r, double c)
    : Option(type, K, T, S, r, c)
{
    this->style = US;
}

double American::getBinomialValue(double sigma, int N) const
{
    double optValue{ 0.0 };
    double up{ Option::getUpFactor(sigma, N) };
    double down{ 1 / up };
    double p_up{ Option::getUpProb(sigma, N) };
    double p_down{ 1 - p_up };

    assert(down < 1 + r && 1 + r < up && "getBinomialValue(double, int) BOPM no arbitrage violated, d < r < u does not hold.");
    assert(0 <= p_up && p_up <= 1 && "getBinomialValue(double, int) BOPM probability not between 0 and 1, invalid inputs.");

    if (type == call && c == 0.0) {
        // American call, no dividend == European call
        for (int node{ 0 }; node <= N; ++node) {
            double exerciseValue{ getPayoffValue(S * pow(up, node) * pow(down, N - node)) };
            optValue += exerciseValue
                * boost::math::binomial_coefficient<double>(N, node)
                * pow(p_up, node) * pow(p_down, N - node);
        }
        optValue /= pow(1 + r, N);
    }
    else {
        std::vector<double> RNPrices{ Option::getBinTreeLevel(sigma, N, N, true) };
        for (int t{ N }; t > 0; --t) {
            std::vector<double> BinTreeLevelPrev_ex{ Option::getBinTreeLevel(sigma, N, t - 1, true) };

            for (int node{ 0 }; node < t; ++node) {
                double exerciseDown{ (RNPrices[node]) };
                double exerciseUp{ (RNPrices[node + 1]) };
                RNPrices[node] = std::max((p_down * exerciseDown + p_up * exerciseUp) / (1 + r), BinTreeLevelPrev_ex[node]);
            }

        }
        optValue = RNPrices[0];
    }

    return optValue;
}

bool American::checkEarlyExercise(double sigma, int N) const
{
    bool earlyExercise{ false };
    double optValue{ 0.0 };
    double up{ Option::getUpFactor(sigma, N) };
    double down{ 1 / up };
    double p_up{ Option::getUpProb(sigma, N) };
    double p_down{ 1 - p_up };

    assert(down < 1 + r && 1 + r < up && "getBinomialValue(double, int) BOPM no arbitrage violated, d < r < u does not hold.");
    assert(0 <= p_up && p_up <= 1 && "getBinomialValue(double, int) BOPM probability not between 0 and 1, invalid inputs.");

    if (type == call && c == 0.0)
    {
        std::cout << "Early exercise not optimal.\n";
    }
    else
    {
        std::cout << "Early exercise starred values (*).\n\n";

        std::vector<double> RNPrices{ Option::getBinTreeLevel(sigma, N, N, true) };
        for (int t{ N }; t > 0; --t)
        {
            std::cout << "Timestep " << t << ":\n";

            std::vector<double> BinTreeLevelPrev_ex{ Option::getBinTreeLevel(sigma, N, t - 1, true) };
            for (int node{ 0 }; node < t; ++node)
            {
                double exerciseDown{ (RNPrices[node]) };
                double exerciseUp{ (RNPrices[node + 1]) };
                if ((p_down * exerciseDown + p_up * exerciseUp) / (1 + r) < BinTreeLevelPrev_ex[node])
                {
                    earlyExercise = true;
                    RNPrices[node] = BinTreeLevelPrev_ex[node];
                    std::cout << "(*)";
                }
                else
                    RNPrices[node] = (p_down * exerciseDown + p_up * exerciseUp) / (1 + r);
                std::cout << RNPrices[node] << "\t\t";
            }
            std::cout << "\n\n";
        }
    }

    if (!earlyExercise) {
        std::cout << "Early exercise not optimal.\n";
    }

    return earlyExercise;
}

