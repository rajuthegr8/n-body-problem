# n-body-problem
Te repository contains the code simulating the n body problem written in C and optimized with openmp.

## Running the code
The following command generates a traj.bin with the tragectories of the 1000 particles in a confined box with elastic collision and only gravity acting on them.

```bash
gcc many-body-sim-program.c -fopenmp -o many-body-sim-program
```
To vizualize it run the following command

```bash
python3 graphics.py
```

## Result
![me](https://github.com/rajuthegr8/n-body-problem/blob/master/animation.gif)




