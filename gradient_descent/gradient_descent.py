import numpy as np
import matplotlib.pyplot as plt

LEARNING_RATE = 0.00001
DERIVATIVE_THRESHOLD = 0.1

x = np.array(np.linspace(0,10,100))
xin = np.array([(x**0), x, (x**2), (x**3)]).T # generate straightforward polynomial features

func = np.array([15, 2, 10.5, -1]) # some arbitrary function linear in the parameters
y = xin.dot(func) + np.random.randn(len(x)) * 10 # evaluate for each data point and add some normally distributed noise

plt.plot(x, y, "bo") # plot the (noisy) datapoints

h = np.zeros(4) # inferred parameters of the original function

while True:
    partial_derivatives = np.sum((xin.dot(h) - y)[:,np.newaxis] * xin, axis = 0) / len(y)
    if abs(partial_derivatives.max()) < DERIVATIVE_THRESHOLD: # break if the derivatives get small enough
        break;
    h = h - LEARNING_RATE * np.array(partial_derivatives)

print("original params: ", func)
print("inferred params: ", h)

yprime = np.sum(xin * h.T, axis = 1)
plt.plot(x, yprime, "m-")
plt.show()
