#ifndef BOPM_H
#define BOPM_H

#include <iostream>
#include <vector>

class Option 
{
public:
    // call/put
    enum Type
    {
        call,
        put,
        max_opt_types
    };

    // Option style
    enum Style
    {
        EU,
        US,
        max_opt_styles
    };

    // constructor
    Option(Type type, double K, double T, double S, double r, double q=0.0);
    
    // displays option properties
    void printProperties() const;
    // return exercise value of option given current price S_t 
    double getExerciseValue(double S_t) const;
    // return BOPM price given underlying volatility sigma and 
    // binomial tree depth N
    virtual double getBinomialValue(double sigma, int N) const;
    // print BOPM to command line
    void printBinomialTree(double sigma, int N) const;
    // check early exercise if optimal
    virtual bool checkEarlyExercise(double sigma, int N) const;
    
    // overload operator<< to return option type
    friend std::ostream& operator<<(std::ostream& out, const Option::Type& optType);
    // overload operator<< to return option style
    friend std::ostream& operator<<(std::ostream& out, const Option::Style& optType);

protected:
    Type type;          // call/put
    Style style;        // option style
    double K;           // strike
    double T;           // time to expiration
    double S;           // spot price
    double r;           // risk-free interest rate
    double q=0.0;           // dividend yield

    // Calculate factor price increases by
    double getUpFactor(double sigma, int N) const;
    // Calculate risk-free probability of price increase
    double getUpProb(double sigma, int N) const;
    // Return vector of Bin Model prices at timestep t of a N level tree
    std::vector<double> getBinTreeLevel(double sigma, int N, int t, bool exercise=false) const;
};

class European: public Option 
{
public:
    // constructor
    European(Type type, double K, double T, double S, double r, double q=0.0);

    double getBinomialValue(double sigma, int N) const override;
    bool checkEarlyExercise(double sigma, int N) const override;
};

class American: public Option 
{
public:
    // constructor
    American(Type type, double K, double T, double S, double r, double q=0.0);

    double getBinomialValue(double sigma, int N) const override;
    bool checkEarlyExercise(double sigma, int N) const override;
};

#endif
