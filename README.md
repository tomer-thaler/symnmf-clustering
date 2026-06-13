# SymNMF Clustering

A clustering project based on **Symmetric Non-negative Matrix Factorization (SymNMF)**. The project builds a graph-based representation of a dataset, factorizes its normalized similarity matrix, derives cluster assignments from the factorization, and compares the resulting clustering with K-means using the silhouette score.

The implementation combines:

- Python for the command-line interface, matrix initialization, and analysis.
- C for the numerical implementation.
- The Python C API for exposing the C functions to Python.

## Table of Contents

- [Algorithm overview](#algorithm-overview)
- [Project structure](#project-structure)
- [Requirements](#requirements)
- [Build and installation](#build-and-installation)
- [Input format](#input-format)
- [Running the project](#running-the-project)
- [Available goals](#available-goals)
- [Output format](#output-format)
- [Analysis: SymNMF vs. K-means](#analysis-symnmf-vs-k-means)
- [Implementation details](#implementation-details)
- [Error handling and assumptions](#error-handling-and-assumptions)
- [Troubleshooting](#troubleshooting)
- [Reference](#reference)

## Algorithm overview

Given a dataset of `N` points

\[
X = \{x_1, x_2, \ldots, x_N\}, \qquad x_i \in \mathbb{R}^d
\]

and a requested number of clusters `k`, the algorithm performs the following steps.

### 1. Similarity matrix

Construct the similarity matrix \(A \in \mathbb{R}^{N \times N}\):

\[
a_{ij} =
\begin{cases}
\exp\left(-\frac{\lVert x_i-x_j\rVert_2^2}{2}\right), & i \ne j \\
0, & i=j
\end{cases}
\]

### 2. Diagonal degree matrix

For every point, calculate

\[
d_i = \sum_{j=1}^{N} a_{ij}
\]

The degree matrix \(D\) is a diagonal matrix whose diagonal entries are
\(d_1,\ldots,d_N\).

### 3. Normalized similarity matrix

Calculate

\[
W = D^{-1/2} A D^{-1/2}
\]

### 4. Symmetric non-negative matrix factorization

Find a non-negative matrix \(H \in \mathbb{R}^{N \times k}\) that minimizes

\[
\min_{H \ge 0} \lVert W-HH^T\rVert_F^2
\]

#### Initialization

Let `m` be the average of all entries in `W`. Initialize every entry of `H` uniformly at random in

\[
\left[0,\;2\sqrt{\frac{m}{k}}\right]
\]

The Python program uses:

```python
np.random.seed(0)
np.random.uniform(...)
```

This makes the initialization reproducible.

#### Update rule

Starting from \(H^{(0)}\), repeatedly update

\[
H_{ij}^{(t+1)} = H_{ij}^{(t)}
\left(
1-\beta+
\beta\frac{(WH^{(t)})_{ij}}{(H^{(t)}(H^{(t)})^T H^{(t)})_{ij}}
\right)
\]

where

\[
\beta=0.5.
\]

The update stops when either:

- `max_iter = 300` iterations have been reached, or
- \(\lVert H^{(t+1)}-H^{(t)}\rVert_F^2 < 10^{-4}\).

### 5. Cluster assignment

Each row of `H` contains the association scores of one data point with all clusters. The assigned cluster is the index of the largest value in that row:

```python
cluster = argmax(H[i])
```

## Project structure

```text
.
├── symnmf.py          # Python command-line interface
├── symnmf.c           # C implementation and standalone C program
├── symnmf.h           # C function declarations
├── symnmfmodule.c     # Python C API wrapper
├── analysis.py        # SymNMF/K-means silhouette-score comparison
├── setup.py           # Builds the Python C extension
├── Makefile           # Builds the standalone C executable
└── *.c / *.h          # Optional additional implementation modules
```

The C extension exposes the following operations to Python:

- `symnmf`
- `sym`
- `ddg`
- `norm`

## Requirements

The original target environment is the course's Linux/Nova environment.

Required software:

- Python 3
- A C compiler
- GNU Make for the standalone C executable
- Python development/build tools
- NumPy
- scikit-learn, for `silhouette_score` in `analysis.py`

Install the Python dependencies with:

```bash
python3 -m pip install numpy scikit-learn setuptools
```

Running the project from a virtual environment is recommended but not required.

## Build and installation

Run all commands from the project root directory.

### 1. Build the Python C extension

```bash
python3 setup.py build_ext --inplace
```

The command should finish without errors or warnings. It creates a platform-specific extension file in the project directory, usually a `.so` file on Linux or a `.pyd` file on Windows.

Do **not** compile the Python extension manually with `gcc`. The extension must be built through `setup.py`.

### 2. Build the standalone C executable

```bash
make
```

This creates an executable named:

```text
symnmf
```

The Makefile compilation command is required to use the following flags:

```text
gcc -ansi -Wall -Wextra -Werror -pedantic-errors
```

### Quick setup on Linux/Nova

```bash
python3 -m pip install numpy scikit-learn setuptools
python3 setup.py build_ext --inplace
make
```

### Windows setup

Build the Python extension with:

```powershell
py -m pip install numpy scikit-learn setuptools
py setup.py build_ext --inplace
```

Building a C extension on Windows requires the Microsoft C/C++ build tools used by the installed Python version. If the build reports that **Microsoft Visual C++ 14.0 or greater is required**, install Visual Studio Build Tools with the **Desktop development with C++** workload.

The supplied Makefile is intended for a GCC-compatible Linux environment. To build and run the standalone C executable on Windows, use WSL/Linux or another compatible GCC and GNU Make environment.

## Input format

The input is a `.txt` file containing `N` data points in \(\mathbb{R}^d\).

- Every point must have the same dimension.
- The number of requested clusters must satisfy `k < N`.
- The original specification does not separately define the coordinate delimiter; use the same text format as the supplied course datasets and the format expected by the project's input parser.

Example filename:

```text
input_1.txt
```

## Running the project

### Python interface

```bash
python3 symnmf.py <k> <goal> <file_name>
```

Arguments:

- `k` - number of requested clusters; must be an integer smaller than the number of data points.
- `goal` - operation to perform: `symnmf`, `sym`, `ddg`, or `norm`.
- `file_name` - path to the input `.txt` dataset.

Examples:

```bash
python3 symnmf.py 2 symnmf input_1.txt
python3 symnmf.py 2 sym input_1.txt
python3 symnmf.py 2 ddg input_1.txt
python3 symnmf.py 2 norm input_1.txt
```

On Windows, the equivalent command is:

```powershell
py symnmf.py 2 symnmf input_1.txt
```

### Standalone C interface

The C program supports the matrix-building goals but does not run the full `symnmf` optimization goal from the command line.

```bash
./symnmf <goal> <file_name>
```

Supported C goals:

- `sym`
- `ddg`
- `norm`

Examples:

```bash
./symnmf sym input_1.txt
./symnmf ddg input_1.txt
./symnmf norm input_1.txt
```

## Available goals

| Goal | Result |
|---|---|
| `symnmf` | Runs the complete SymNMF process and prints the final association matrix `H`. Available through `symnmf.py`. |
| `sym` | Calculates and prints the similarity matrix `A`. |
| `ddg` | Calculates and prints the diagonal degree matrix `D`. |
| `norm` | Calculates and prints the normalized similarity matrix `W`. |

## Output format

All matrices are printed as comma-separated values:

- One matrix row per output line.
- No additional text before or after the matrix.
- Every value is printed to exactly four decimal places.

Example:

```text
0.0600,0.0100
0.0100,0.0500
0.0100,0.0400
0.0200,0.0400
0.0500,0.0200
```

The required formatting is equivalent to:

```c
printf("%.4f", value);
```

Examples of the expected rounding/formatting:

```text
8.88885                  -> 8.8888
5.92237098749999997906   -> 5.9224
2.231                    -> 2.2310
```

## Analysis: SymNMF vs. K-means

`analysis.py` runs both SymNMF and K-means on the same dataset and compares their clustering quality using `sklearn.metrics.silhouette_score`.

Run it with:

```bash
python3 analysis.py <k> <file_name>
```

Example:

```bash
python3 analysis.py 5 input_k5_d7.txt
```

Expected output format:

```text
nmf: 0.1162
kmeans: 0.1147
```

For SymNMF, a data point's cluster is determined by the index of the maximum entry in its row of `H`.

The silhouette coefficient for a point is

\[
\frac{b-a}{\max(a,b)}
\]

where:

- `a` is the point's mean distance from the other points in its own cluster.
- `b` is the smallest mean distance from the point to any other cluster.

A higher average silhouette score generally indicates better-separated and more internally coherent clusters.

## Implementation details

- `symnmf.py` reads the command-line arguments and input data.
- When the goal is `symnmf`, Python initializes `H`, calculates or obtains `W`, and passes them to the C extension.
- The numerical matrix operations are implemented in C.
- `symnmfmodule.c` wraps the C functions and converts data between Python and C representations.
- `symnmf.h` declares every C function used by `symnmfmodule.c` and implemented in the C source files.
- `setup.py` builds the Python extension imported by `symnmf.py`.
- `Makefile` builds the standalone C command-line program and all of its dependencies.

## Analysis

`analysis.py` compares SymNMF with K-means on the same dataset.

- It runs SymNMF, converts the `H` matrix into cluster labels with `argmax` per row, and computes the silhouette score.
- It runs K-means, assigns each point to its nearest centroid, and computes the silhouette score.
- The script prints both scores so you can compare clustering quality directly.

## Error handling and assumptions

The original project requirements define the following behavior:

- On any error, print exactly:

  ```text
  An Error Has Occurred
  ```

  and terminate the program.

- Command-line argument validation is not required by the assignment specification.
- All supplied data points may be assumed to be distinct.
- Use `double` for vector and matrix values in C.
- Use Python floating-point values for vector and matrix values in Python.
- Every dynamically allocated C resource must be freed.
- Both SymNMF and K-means use:

  ```text
  epsilon = 1e-4
  max_iter = 300
  ```

- No official test files are included; custom datasets may be created for local testing.
- The original assignment was required to run on the Nova environment and prohibited container-based execution.

## Troubleshooting

### `ModuleNotFoundError` for the C extension

Make sure the extension was built successfully and that the Python command is being run from the project root:

```bash
python3 setup.py build_ext --inplace
python3 symnmf.py 2 symnmf input_1.txt
```

### `Microsoft Visual C++ 14.0 or greater is required`

Install Visual Studio Build Tools with **Desktop development with C++**, restart the terminal, and run:

```powershell
py setup.py build_ext --inplace
```

### `make` is not recognized on Windows

The standalone C build expects GNU Make and GCC. Run it in WSL/Linux:

```bash
make
./symnmf sym input_1.txt
```

### Build fails because warnings are treated as errors

The Makefile uses `-Werror`, so every compiler warning must be fixed before the C executable can be built.

## Reference

Da Kuang, Chris Ding, and Haesun Park. *Symmetric Nonnegative Matrix Factorization for Graph Clustering*. Proceedings of the 2012 SIAM International Conference on Data Mining, 2012.
