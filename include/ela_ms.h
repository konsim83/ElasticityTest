// Deal.II
#ifndef _INCLUDE_ELA_MS_H_
#define _INCLUDE_ELA_MS_H_

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

#include <deal.II/dofs/dof_accessor.h>
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

#include "ela_basis.h"
#include "forces_and_lame_parameters.h"
#include "mytools.h"
#include "postprocessing.h"
#include "process_parameter_file.h"

// STL
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>


namespace Elasticity
{
  using namespace dealii;

  /**
   * @brief Class for the coarse scale of the MsFEM for linear elasticity.
   *
   * @tparam dim Space dimension
   *
   * This class computes and outputs the global solution of
   * the given linear elasticity problem.
   *
   * Based on the implementations in the repository
   * https://github.com/konsim83/MPI-MSFEC/ and the step-40
   * tutorial of deal.ii.
   */
  template <int dim>
  class ElaMs
  {
  public:
    /**
     * @brief Construct a new ElaMs object.
     *
     * @param global_parameters Parameters that many classes need.
     * @param parameters_ms Parameters that only this class needs.
     * @param parameters_basis Parameters for the fine-scale part of the MsFEM.
     */
    ElaMs(const GlobalParameters<dim> &global_parameters,
          const ParametersMs &         parameters_ms,
          const ParametersBasis &      parameters_basis);

    /**
     * @brief Function that runs the problem.
     *
     * This function runs all the member functions
     * that are necessary to solve the problem.
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
     * @brief Initializes and computes the basis functions.
     *
     * This function initializes and computes the local basis functions
     * on each cell with the MsFEM by creating an ElaBasis object for
     * each cell and computing the basis functions with these objects.
     */
    void
    initialize_and_compute_basis(unsigned int cycle);

    /**
     * @brief Assembles the system.
     *
     * This function assembles the #system_matrix and the #system_rhs by
     * assembling the contributions of all the ElaBasis objects and adding
     * the Neumann boundary condition.
     */
    void
    assemble_system();

    /**
     * @brief Solves the global problem.
     *
     * This function solves the global problem.
     *
     * In #parameters_ms, it can be specified if a direct
     * or iterative (CG-method with AMG preconditioner) shall
     * be used.
     */
    void
    solve();

    /**
     * @brief Sends global weights to cell.
     *
     * For each cell, this function sends the respective local part of the
     * solution vector to the corresponding ElaBasis object such that the
     * computed solution vector can be applied to the local basis functions
     * of the MsFEM.
     */
    void
    send_global_weights_to_cell();

    /**
     * @brief Adaptively refines grid.
     */
    void
    refine_grid();

    /**
     * @brief Outputs the solutions in pvtu files.
     *
     * This function outputs the global solution with the coarse basis functions
     * as well as with the constructed multiscale basis functions as pvtu files.
     *
     * In the first case, it creates vtu files for every subdomain
     * (corresponding to the respective processor) and combines them into
     * a single pvtu file.
     *
     * In the latter case, it lets each ElaBasis object output the
     * local solution as vtu files and combines all of them into a
     * single pvtu file.
     */
    void
    output_results(unsigned int cycle);

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
    std::map<CellId, ElaBasis<dim>>           cell_basis_map;
    const GlobalParameters<dim>               global_parameters;
    const ParametersMs                        parameters_ms;
    ParametersBasis                           parameters_basis;
    bool                                      processor_is_used;
    /**< True if this processor is assigned at least one coarse cell. */

    ConditionalOStream pcout;
    TimerOutput        computing_timer;
  };
} // namespace Elasticity

#endif // _INCLUDE_ELA_MS_H_