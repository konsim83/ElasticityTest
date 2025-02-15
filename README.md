# MsFEM Implementation for Linear Elasticity Problems in C++

This code is a MsFEM implementation for linear elasticity problems in C++
using bi-/trilinear Lagrange elements. Since these problems can become very
large, this implementation is MPI parallel and can be used on clusters. For
more detailed information about the implemetation build the documentation.

### Requirements

This project requires:

* A Linux distribution (I used Ubuntu for the development)
* **cmake** v2.8.12 or higher	
* **doxygen**, **mathjax** and **GraphViz** (for the documentation)
* A working installation of **[deal.ii](www.dealii.org)** v9.2.0 or higher 
with **MPI**, **p4est** and all **Trilinos** dependencies must be installed. This
can easily be done through the **[spack](https://spack.readthedocs.io/en/latest/)** 
package manager
* **[Paraview](www.paraview.org)** or **[Visit](https://wci.llnl.gov/simulation/computer-codes/visit/)** 
for the visualization
* If you wish to modify the code **clang-format-6.0** (recommented to indent the code, 
available in most linux distributions) and a working **debugger** is usually a good idea

### Building the Library and Executables

Firstly, you need to clone the repository:

```
git clone https://github.com/JonathanFenske/ElasticityTest ElasticityTest
```

If you wish to have the build files in a folder parallel to the code (which I recommend for a better overview):

```
mkdir ElasticityTest_build
cd ElasticityTest_build
```

Then create the build files:

```
cmake -DDEAL_II_DIR=/path/to/dealii ../ElasticityTest
```
Note that the argument DDEAL_II_DIR is only necessary if the deal.ii library is not installed in the standard
library folder (usr/lib).

Now let N be the number of processors you want to use. If you want to create an executable in debug mode, use:

```
make debug
make -jN
```

If you want to create an executable in release mode which is faster, use:

```
make release
make -jN
```

Finally, to run the executable, type:

```
mpirun -n N source/MsEla -p ../ElasticityTest/parameter_files/PARAMETER_FILE
```

Here, `PARAMETER_FILE` is a placeholder for parameter files. In the folder `ElasticityTest/parameter_files` are 
already existing parameter files, like e.g. `prm_oscillating`. Note that a parameter file is necessary.

The results are then stored in a new folder called `output` and can be visualized with *Paraview* or *Visit*.

### Building the documentation

The documentation can be build with `doxygen`. To do so, enter the folder `ElasticityTest/doc`:

```
cd ElasticityTest/doc
```

and then type

```
doxygen docfile
```

Then a html documentation will be build in the directory `ElasticityTest/doc/documentation/html`. You can then
access the documentation with any html file you want, like e.g. `index.html`.