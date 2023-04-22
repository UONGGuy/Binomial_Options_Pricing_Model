#include "BOPM.h"
#include <boost/algorithm/string/erase.hpp>
#include <boost/concept_check.hpp>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

void clearBuffer()
{
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

Option::Type getOptionType()
{
    char optType{};

    do {
        std::cin >> optType;
        clearBuffer();
    }
    while(optType != 'C' && optType != 'c' && optType != 'P' && optType != 'p');

    if (optType == 'C' || optType == 'c') {
        return Option::Type::call;
    }

    return Option::Type::put;
}

std::string getOptionStyle()
{
    std::string optStyle{};

    do {
        std::cin >> optStyle;
        clearBuffer();
        boost::algorithm::erase_all(optStyle, " ");
    }
    while(optStyle != "European" && optStyle != "european" && optStyle != "American" && optStyle != "american");

    return optStyle;
}

template<typename T>
T getValue() 
{
    T value{};

    do {
        std::cin >> value;
        clearBuffer();
    }
    while(std::cin.fail());

    return value;
}

struct AddModelParams
{
    double sigma;
    int N;
};

int Menu(Option* option)
{
    int action{};

    do {
        clearBuffer();
        std::cout << "Select one of the following actions:\n";
        std::cout << "- Print option properties (1)\n";
        std::cout << "- Get model calculated value of option (2)\n";
        std::cout << "- Print binomial model tree (3)\n";
        std::cout << "- Check model for optimal early exercise (4)\n";
        std::cout << "- Close program (0)\n";
        std::cout << "Action: ";
        std::cin >> action;
    }
    while (action < 0 || action > 4 || std::cin.fail());

    AddModelParams params;

    switch(action) {
        case 1:
            option->printProperties();
            break;
        case 2:
            std::cout << "Enter option volatility: ";
            params.sigma = getValue<double>();
            std::cout << "Enter number of periods to model: ";
            params.N = getValue<int>();
            option->getBinomialValue(params.sigma, params.N);
            break;
        case 3:
            std::cout << "Enter option volatility: ";
            params.sigma = getValue<double>();
            std::cout << "Enter number of periods to model: ";
            params.N = getValue<int>();
            option->printBinomialTree(params.sigma, params.N);
            break;
        case 4:
            std::cout << "Enter option volatility: ";
            params.sigma = getValue<double>();
            std::cout << "Enter number of periods to model: ";
            params.N = getValue<int>();
            option->checkEarlyExercise(params.sigma, params.N);
            break;
        default:
            break;
    }
    
    return action;
}

int main() 
{
    std::cout << "Binomial Options Pricing Model\n\n";
    std::cout << "Input parameters for option to model:\n";

    std::cout << "Call/Put (C, P): ";
    Option::Type optType{ getOptionType() };

    std::cout << "Option style (e.g. European, American): ";
    std::string optStyle{ getOptionStyle() };

    std::cout << "Strike: ";
    double K{ getValue<double>() };

    std::cout << "Time to expiry: ";
    double T{ getValue<double>() };

    std::cout << "Spot price: ";
    double S{ getValue<double>() };

    std::cout << "Risk-free rate: ";
    double r{ getValue<double>() };

    std::cout << "Dividend per period";
    double q{ getValue<double>() };

    int action{ 10 };

    if (optStyle == "European" || optStyle == "european") {
        European option(optType, K, T, S, r, q);

        while (action) {
            action = Menu(&option);
        }

        return 0;        
    }
    else if(optStyle == "American" || optStyle == "american") {
        American option(optType, K, T, S, r, q);

        while (action) {
            action = Menu(&option);
        }

        return 0;        
    }

    return 0;
}
