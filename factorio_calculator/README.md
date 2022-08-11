Silly little C++ DSL for computing amounts of components/materials required for Factorio recipes. Allows declaring component dependencies and then querying compound product inputs.
Also useful (if it can be called that) for computing the number of fabricators needed to achieve given throughput. For example:

```C++
let iron_plate = basic; // can declare any component as basic if we don't want to go all the way to iron ore etc.
let iron_gear_wheel = iron_plate*2;

// declaring fabricator outputs
grey_fabricator produces 2 of iron_gear_wheel every 1s;

// finally, the actual query we're interested in - how many fabricator do we need to achieve given throughput?
// (prints to stdout)
grey_fabricator number producing 400 of iron_gear_wheel every 6s;
```
