// Deal.II
#ifndef _INCLUDE_ELA_STD_H_
#define _INCLUDE_ELA_STD_H_

#include <deal.II/base/conditional_ostream.h>
#include <deal.II/base/function.h>
#include <deal.II/base/index_set.h>
#include <deal.II/base/logstream.h>
#include <deal.II/base/multithread_info.h>
#include <deal.II/base/quadrature_lib.h>
#include <deal.II/base/timer.h>
#include <deal.II/base/utilities.h>

#include <deal.II/distributed/grid_refinement.h>
#include <deal.II/distributed/tria.h>

#include <deal.II/dofs/dof_handler.h>
#include <deal.II/dofs/dof_tools.h>

#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_system.h>
#include <deal.II/fe/fe_values.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_tools.h>

#include <deal.II/lac/affine_constraints.h>
#include <deal.II/lac/dynamic_sparsity_pattern.h>
#include <deal.II/lac/full_matrix.h>
#include <deal.II/lac/solver_bicgstab.h>
#include <deal.II/lac/solver_cg.h>
#include <deal.II/lac/solver_gmres.h>
#include <deal.II/lac/sparsity_tools.h>
#include <deal.II/lac/trilinos_block_sparse_matrix.h>
#include <deal.II/lac/trilinos_parallel_block_vector.h>
#include <deal.II/lac/trilinos_precondition.h>
#include <deal.II/lac/trilinos_solver.h>
#include <deal.II/lac/trilinos_sparse_matrix.h>

#include <deal.II/numerics/data_out.h>
#include <deal.II/numerics/error_estimator.h>
#include <deal.II/numerics/vector_tools.h>

#include <deal.II/physics/transformations.h>

#include "forces_and_lame_parameters.h"
#include "mytools.h"
#include "postprocessing.h"
#include "process_parameter_file.h"

// STL
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>


namespace Elasticity
{
  using namespace dealii;

  /**
   * @brief Class for solving linear elasticity problems using the FEM.
   *
   * @tparam dim Space dimension
   *
   * This class solves linear elasticity problems in parallel
   * with the FEM and is used to illustrate the differences
   * between the standard FEM and MsFEM solutions.
   *
   * Based on the
   * <a href="https://www.dealii.org/9.2.0/doxygen/deal.II/step_8.html">
   * step-8</a>
   * and
   * <a href="https://www.dealii.org/9.2.0/doxygen/deal.II/step_40.html">
   * step-40</a> deal.ii tutorials.
   */
  template <int dim>
  class ElaStd
  {
  public:
    /**
     * @brief Construct a new ElaStd object.
     *
     * @param global_parameters Parameters that many classes need.
     * @param parameters_std Parameters that only this class needs.
     */
    ElaStd(const GlobalParameters<dim> &global_parameters,
           const ParametersStd &        parameters_std);

    /**
     * @brief Function that runs the problem.
     *
     * This function first refines the problem
     * and then runs all the member functions
     * that are necessary to solve the problem and
     * repeats this for a number of times specified
     * in the parameter file.
     */
    void
    run();

  private:
    /**
     * @brief Sets up the system.
     *
     * This function sets up the system, i.e. it creates the needed sparse
     * matrices with the correct sparsity pattern and the vectors.
     *
     * Moreover, it creates the constraint vector that contains
     * the global Dirichlet boundary conditions
     * and the hanging node constraints.
     */
    void
    setup_system();

    /**
     * @brief Assembles the system.
     *
     * This function assembles the #system_matrix and the #system_rhs
     * for linear elasticity problems.
     */
    void
    assemble_system();

    /**
     * @brief Solves the problem.
     *
     * This function solves the problem.
     *
     * In #parameters_std, it can be specified if a direct
     * or iterative (CG-method with AMG preconditioner) shall
     * be used.
     */
    void
    solve();

    /**
     * @brief Adaptively refines grid.
     */
    void
    refine_grid();

    /**
     * @brief Outputs the solutions in pvtu files.
     *
     * This function outputs the global solution.
     *
     * For this, it creates vtu files for every subdomain
     * (corresponding to the respective processor) and combines them into
     * a single pvtu file.
     */
    void
    output_results(const unsigned int cycle) const;

    MPI_Comm                                  mpi_communicator;
    parallel::distributed::Triangulation<dim> triangulation;
    FESystem<dim>                             fe;
    DoFHandler<dim>                           dof_handler;
    IndexSet                                  locally_owned_dofs;
    IndexSet                                  locally_relevant_dofs;
    AffineConstraints<double>                 constraints;
    TrilinosWrappers::SparseMatrix            system_matrix;
    TrilinosWrappers::SparseMatrix            preconditioner_matrix;
    TrilinosWrappers::MPI::Vector             locally_relevant_solution;
    TrilinosWrappers::MPI::Vector             system_rhs;
    const GlobalParameters<dim>               global_parameters;
    const ParametersStd                       parameters_std;
    bool                                      processor_is_used;
    /**< True if this processor is assigned at least one coarse cell. */

    ConditionalOStream pcout;
    TimerOutput        computing_timer;
  };
} // namespace Elasticity

#endif // _INCLUDE_ELA_STD_H_