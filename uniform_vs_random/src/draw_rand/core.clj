;; This is inspired by http://bl.ocks.org/4358325 and http://www.empiricalzeal.com/2012/12/21/what-does-randomness-look-like/
;; to show how random is not necessarily uniform.
;; Random mode draws dots at completely random coordinates.
;; Uniform mode divides the area into uniform cells and draws points randomly within those cells, 
;; giving a more "uniform distribution".
;;
;; Author: Błażej Czapp

(ns draw-rand.core
    (:use quil.core)
    (:use clojure.contrib.command-line))

(defn setup [] (frame-rate 1) (background 0) (no-loop))

(defn draw-random [no-of-points]
    (stroke-weight  0)
    (fill 255)
    (dotimes [i no-of-points]
        (ellipse (random (width)) (random (height)) 2 2)
    )
) 

(defn draw-uniform [no-of-points]
    (stroke-weight 0) 
    (fill 255)

    ;; Use the same number of cells in each direction, prefer smallish blocks of 20,
    ;; but increase the size (decrease the number) so that there is at least one cell per point to avoid a situation where
    ;; e.g. for size 600x600 and 50 points, there would be 900 cells with 0.055 points per cell (i.e. zero).
    ;; Interestingly, having large cells brings the uniform plot closer to the random one.
    (let [no-of-cells (min (/ (width) 20) (Math/floor (Math/sqrt no-of-points)))
          cell-size-x (/ (width) no-of-cells)
          cell-size-y (/ (height) no-of-cells)]
        (dotimes [i no-of-cells]
            (dotimes [j no-of-cells]
                (dotimes [k (/ no-of-points (* no-of-cells no-of-cells))]
                    (ellipse 
                        (+ (* i cell-size-x) (random cell-size-x))
                        (+ (* j cell-size-y) (random cell-size-y))
                        2 2)
                )
            )
        )
    )
)

(defn -main [& args]
    (with-command-line args
        "Parameters"
        [[points "number of points to draw" "500"]
        [type "Type of distribution, can be 'random' or 'uniform'" "random"]]
    
        (defsketch example :title (str "Random/Uniform visualiser: " type)
                           :setup setup 
                           :draw (if (= type "random") 
                                     (fn [] (draw-random (read-string points)))
                                     (if (= type "uniform")
                                         (fn [] (draw-uniform (read-string points)))
                                         ((println "Invalid type argument, must be \"uniform\" or \"random\"") (System/exit 0))
                                     )
                                 )
                           :size [600 600])
    )
)
