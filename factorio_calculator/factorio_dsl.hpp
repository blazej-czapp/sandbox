#include <iostream>
#include <vector>
#include <utility>
#include <numeric>
#include <algorithm>
#include <map>
#include <chrono>
#include <iomanip>
#include <sstream>

using namespace std::chrono_literals;

void printDecimal(double val, int precision = 2) {
    std::stringstream oldState; // this is so bad and dumb and bad...
    oldState.copyfmt(std::cout);
    std::cout << std::fixed << std::setprecision(precision) << val << std::endl;
    std::cout.copyfmt(oldState);
}

/**
 * A DSL to compute the number of components necessary to produce more complex
 * products in Factorio. We want minimal code syntax to express the recipes, ideally no function
 * calls or e.g. quotes around strings.
 * Internally, we distinguish named components (e.g. green_circuit) from compound/unnamed ones
 * (e.g. iron_plate*3 + copper_plate*2) by overloading operators on lvalue and rvalue references.
 * When building a recipe, named components are kept as nodes in the tree, while unnamed ones are
 * broken down into constituent parts.
 */
class Component {
    using rational = double;
    // don't obsucate key ideas with proper memory management ;)
    using Substrates = std::vector<std::pair<rational, const Component*>>;

public:
    mutable Substrates m_substrates;

    Component() {}
    Component(Substrates subs) : m_substrates(std::move(subs)) {}

    Component operator*(rational multiplier) const & {
        return Component({{multiplier, this}});
    }

    Component operator*(rational multiplier) const && {
        // rvalue, reuse storage
        std::for_each(m_substrates.begin(), m_substrates.end(), [&multiplier](Substrates::value_type& value) {
            value.first *= multiplier;
        });
        return *this;
    }

    Component operator/(rational multiplier) const & {
        return *this * (1/multiplier);
    }

    // "in" keyword
    rational operator/(const Component& component) const & {
        const rational ret = component.count_of(*this);
        printDecimal(ret);
        return ret;
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
        return std::accumulate(m_substrates.begin(), m_substrates.end(), 0.0,
            [&target](rational runningTotal, const Substrates::value_type& sub) {
                rational localTotal = 0;
                if (sub.second == &target) {
                    localTotal += sub.first;
                }

                return runningTotal + localTotal + sub.first * sub.second->count_of(target);
            });
    }
};

// e.g. green_fabricator produces 2 of green_circuit every 2s
// becomes:
// (operator+ used completely arbitrarily, could be any binary operator)
// ProducesProxy producesProxy = green_fabricator+(2);
// OfProxy ofProxy = producesProxy+(green_circuit);
// ofProxy+(int); // every
// OfProxy::operator+ stores the complete yield in Producer::yields

// these are sure to conflict with some variable names when the header is included :)
#define produces +
#define of +
#define every +

#define number
#define producing -
#define in /

class Producer {
public:
    using duration_t = std::chrono::duration<double>;

    struct ComponentProxy {
        // "every" keyword - end of the chain, either produce the result (if querying)
        // or store the recipe (otherwise)
        int operator+(duration_t duration) {
            if (querying) {
                // yield: m components over n seconds === (m/n) per second
                // request: p components over q seconds === (p/q) per second
                // required count = request/yield
                const double exact_count = (num / duration.count())
                                           /
                                           (Producer::yields[component].num / Producer::yields[component].duration.count());
                printDecimal(exact_count);
                return exact_count;
            } else {
                Producer::yields[component] = {num, duration};
                return 0;
            }
        }

        const int num;
        const Component* component;
        const bool querying;
    };

    struct CountProxy {
        // "of" keyword
        ComponentProxy operator+(const Component& component) {
            return ComponentProxy{num, &component, querying};
        }

        const int num;
        const bool querying; // same operators/keywords are used when defining, others when querying
                             // producers so propagate the fact we're querying through the proxies
                             // e.g. green_fabricator count producing 4 of green_circuit every 4s;
                             // "count" actually doesn't do anything, but "producing" means we're querying
    };

    struct Yield {
        int num;
        duration_t duration;
    };

    // "produces" keyword
    CountProxy operator+(int num) {
        return CountProxy{num, false};
    }

    // "producing" keyword
    CountProxy operator-(int num) {
        return CountProxy{num, true};
    }

private:
    inline static std::map<const Component*, Yield> yields;
};

// some more sugar, less interesting (how to get rid of the semicolon?)
#define let auto
// #define be =

// some predefined producers - pure sugar, we could just have a single "producer" object
// a leaf component of a particular recipe; can be anything (not just ore) depending on what's available, e.g.
// green_circuit can be defined as basic if it comes from the bus
inline Component basic;
inline Producer grey_fabricator;
inline Producer blue_fabricator;
inline Producer green_fabricator;
inline Producer stone_furnace;
inline Producer oil_refinery;
inline Producer chemical_plant;
inline Producer electric_mining_drill;
