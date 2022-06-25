A small Clojure script to visualise how random can be quite different to uniform. It plots inside a square points in two ways:
- random - where both coordinates are completely random
- uniform - where the drawing area is divided into uniform cells and points drawn randomly within those cells, giving a more "uniform distribution"

This has been inspired by this article: http://www.empiricalzeal.com/2012/12/21/what-does-randomness-look-like/ and this implementation: http://bl.ocks.org/4358325

## Usage

    $ lein run -m draw-rand.core --points <number of points> --type <uniform or random>
