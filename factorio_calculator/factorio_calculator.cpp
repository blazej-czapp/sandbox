#include <iostream>
#include <vector>
#include <utility>
#include <numeric>
#include <algorithm>

/**
 * A DSL(ish) to compute the number of components necessary to produce more complex
 * products in Factorio. We want minimal code syntax to express the recipes, ideally no function
 * calls or e.g. quotes around strings. The key requirement is to list high level *named* components
 * (e.g. green circuits) as such, without breaking them up into constituent parts. We're using lvalue
 * and rvalue references to distinguish between the two.
 */
class Component {
    using rational = float;
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
        return count(component);
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

    rational count(const Component& target) const {
        return std::accumulate(m_substrates.begin(), m_substrates.end(), 0.f,
            [&target](rational runningTotal, const Substrates::value_type& sub) {
                rational localTotal = 0;
                if (sub.second == &target) {
                    localTotal += sub.first;
                }

                return runningTotal + localTotal + sub.first * sub.second->count(target);
            });
    }
};

// some more sugar, less interesting (how to get rid of the semicolon?)
#define let auto
// #define be =

int main() {
    let raw = Component();
    let copper_ore = raw;
    let copper_plate = copper_ore;
    let iron_ore = raw;
    let iron_plate = iron_ore;
    let iron_stick = iron_plate*0.5;
    let copper_wire = copper_plate*0.5;
    let green_circuit = copper_wire + iron_plate*2;
    let stone = raw;
    let steel_plate = raw; // we'll never smelt steel directly, so just assume it comes from the bus

//    let rail = (stone + steel_plate + iron_stick)*0.5;
//    let red_circuit = raw;
//    let green_circuit = raw;
//    let stone_brick = stone;
//    let electric_furnace = steel_plate*10 + red_circuit*5 + stone_brick*10;
//    let productivity_module = green_circuit*5 + red_circuit*5;
//
//    let purple_science = (rail*30 + electric_furnace + productivity_module)/3/(21/1.25)*6;
//    let low_density_structure = copper_plate*20 + plastic*5 + steel_plate*2;
//

    // --------------------------------------
    // dedicated oil processing for the rocket - no light oil cracking, will need a separate gas production
    // on top of this for plastic, but the ratios will be different :/
    // 100 oil -> 63.75 light oil + 55 gas
    let crude_oil = raw;
    let light_oil = crude_oil*(100/63.75);
    let petroleum_gas = crude_oil*(100/55);
    // --------------------------------------

    let coal = raw;

    let plastic_bar = (coal + petroleum_gas*20)/2;
    let red_circuit = copper_wire*4 + green_circuit*2 + plastic_bar*2;
    let water = raw;
    let sulfur = (petroleum_gas*30 + water*30)/2;
    let sulfuric_acid = (iron_plate + sulfur*5 + water*100)/50;
    let blue_circuit = red_circuit*2 + green_circuit*20 + sulfuric_acid*5; // 20s

    let solid_fuel = light_oil*10;
    let rocket_fuel = light_oil*10 + solid_fuel*10;
    let low_density_structure = copper_plate*20 + plastic_bar*5 + steel_plate*2;
    let speed_module_1 = red_circuit*5 + green_circuit*5;
    let rocket_control_unit = blue_circuit + speed_module_1;
    let rocket_part = low_density_structure*10 + rocket_fuel*10 + rocket_control_unit*10;
    let rocket = rocket_part*100;

    let target = rocket; // 5 rockets per hour consumes 5000 blue circuits and 35000 red circuits
    std::cout << (rocket/iron_plate)/3600 << std::endl;
    std::cout << target.count(copper_plate)/3600 << std::endl;
    std::cout << target.count(blue_circuit)/3600 << std::endl;
    std::cout << target.count(red_circuit)/3600 << std::endl;
    std::cout << target.count(petroleum_gas)/3600 << std::endl;
    std::cout << target.count(crude_oil) << std::endl;
}
