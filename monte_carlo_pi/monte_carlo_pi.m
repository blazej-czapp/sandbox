# This is in Octave/Matlab

# Estimating the value of PI/4 by sampling n points and calculating the proportion
# of how many fall inside and outside of a quarter-disc inscribed in a square.

n = input('How many points to use for estimation: ');

# plotting the first quadrant of a circle of radius 1
x = 0:0.01:1;
y = sqrt(1 - x .^ 2);

# n random samples
p = rand(2, n);

#setenv('GNUTERM', 'X11');
hold on
axis equal
plot(p(1, :), p(2, :), '.');
plot(x, y, 'r-', 'linewidth', 2);
hold off

# counting how many points end up under the curve (i.e. inside the circle)
hit = sum(p(1,:) .^ 2 + p(2, :) .^ 2 < 1);

# multiply the hit ratio by 4 to get PI
pi = hit / n * 4;

printf('PI estimate: %f\n', pi);
pause();