#include <iostream>
#include <vector>
#include <utility>
#include <numeric>
#include <algorithm>
#include <map>
#include <chrono>

/**
 * A DSL(ish) to compute the number of components necessary to produce more complex
 * products in Factorio. We want minimal code syntax to express the recipes, ideally no function
 * calls or e.g. quotes around strings. The key requirement is to list high level *named* components
 * (e.g. green circuits) as such, without breaking them up into constituent parts. We're using lvalue
 * and rvalue references to distinguish between the two.
 */
class Component {
    using rational = double;
    // don't obsucate key ideas with proper memory management
    using Substrates = std::vector<std::pair<rational, const Component*>>;

public:
    mutable Substrates m_substrates;

    Component() {}
    Component(Substrates subs) : m_substrates(std::move(subs)) {}

    Component operator*(rational multiplier) const & {
        return Component({{multiplier, this}});
    }

    Component operator*(rational multiplier) const && {
        // std::for_each(m_substrates.begin(), m_substrates.end(), [&multiplier](Substrates::value_type& value) {
        //     value.first *= multiplier;
        // });
        // return *this;
        Substrates combined;
        std::transform(m_substrates.begin(), m_substrates.end(), std::back_inserter(combined), [&multiplier](const Substrates::value_type& value) {
            return std::make_pair(value.first * multiplier, value.second);
        });
        return Component(combined);
    }

    Component operator/(rational multiplier) const & {
        return *this * (1/multiplier);
    }

    // Need to use an operator to be able to query with infix notation (we don't want function call syntax).
    // Picked division since `rocket / blue_circuit` is kind of intuitive (how many blue circuits in a rocket?).
    rational operator/(const Component& component) const & {
        return count_of(component);
    }

    Component operator/(rational multiplier) const && {
        return std::move(*this) * (1/multiplier);
    }

    Component operator+(const Component& other) const & {
        return Component({{1, this}, {1, &other}});
    }

    Component operator+(const Component&& other) const & {
        Substrates combined;
        combined.push_back({1, this});
        std::copy(other.m_substrates.begin(), other.m_substrates.end(), std::back_inserter(combined));
        return Component(combined);
    }

    Component operator+(const Component& other) && {
        // we're an rvalue, so just reuse storage
        m_substrates.emplace_back(1, &other);
        return *this;
    }

    Component operator+(const Component&& other) && {
        // we're an rvalue, so just reuse storage
        std::copy(other.m_substrates.begin(), other.m_substrates.end(), std::back_inserter(m_substrates));
        return *this;
    }

private:
    rational count_of(const Component& target) const {
        return std::accumulate(m_substrates.begin(), m_substrates.end(), 0.f,
            [&target](rational runningTotal, const Substrates::value_type& sub) {
                rational localTotal = 0;
                if (sub.second == &target) {
                    localTotal += sub.first;
                }

                return runningTotal + localTotal + sub.first * sub.second->count_of(target);
            });
    }
};

// green_fabricator produces 2 of green_circuit every 2s
// becomes
// ProducesProxy producesProxy = green_fabricator+(2);
// OfProxy ofProxy = producesProxy+(green_circuit);
// ofProxy+every(int);
// OfProxy::operator+ stores the complete yield recprd in Producer::yields
// + used completely arbitrarily, could be any binary operator

#define produces +
#define of +
#define every +

#define calculate ++
#define producing +

class Producer {
public:
    struct ComponentProxy {
        // every keyword
        int operator+(double seconds) {
            if (querying) {
                // production: k components over n seconds (k/n)
                // request: l components over m seconds (l/m)
                // to common denominator: k*m/n*m, l*n/n*m
                // required count: l*n/k*m
                return num * Producer::yields[component].seconds /
                       (Producer::yields[component].num * seconds);
            } else {
                Producer::yields[component] = {num, seconds};
                return 0;
            }
        }

        const int num;
        const Component* component;
        const bool querying;
    };

    struct CountProxy {
        // of keyword
        ComponentProxy operator+(const Component& component) {
            return ComponentProxy{num, &component, querying};
        }

        const int num;
        const bool querying; // same operators/keywords are used when defining and querying
                             // producers so propagate the fact we're querying through the proxies
                             // based on the initial "count" keyword, e.g.:
                             // count green_fabricator producing 4 of green_circuit every 4s;
    };

    struct QueryProxy {
        // producing
        CountProxy operator+(int num) {
            return CountProxy{num, true};
        }
        const Producer& producer;
    };

    struct Yield {
        int num;
        double seconds;
    };
public:
    // produces keyword
    CountProxy operator+(int num) {
        return CountProxy{num, false};
    }

    // count keyword
    QueryProxy operator++() const {
        return QueryProxy{*this};
    }

public:
    inline static std::map<const Component*, Yield> yields;
};


// get the 's' literal suffix without all the faff of std::chrono
inline long double operator"" _s(long double val) {
    return val;
}
inline long double operator"" _s(unsigned long long val) {
    return static_cast<long double>(val);
}

// some more sugar, less interesting (how to get rid of the semicolon?)
#define let auto
// #define be =

inline Component raw;
inline Producer green_fabricator;
inline Producer stone_furnace;
