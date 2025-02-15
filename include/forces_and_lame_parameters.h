#ifndef _INCLUDE_FORCES_AND_LAME_PARAMETERS_H_
#define _INCLUDE_FORCES_AND_LAME_PARAMETERS_H_

#include <deal.II/base/function.h>

#include <deal.II/fe/mapping_q1_eulerian.h>

#include <map>

/**
 * @file forces_and_lame_parameters.h
 *
 * @brief Forces and parameters
 *
 */


namespace Elasticity
{
  using namespace dealii;

  /****************************************************************************/
  /* Forces and parameters */

  /**
   * @brief Class for the density of the surface force in N/m^2.
   *
   * @tparam dim Space dimension
   */
  template <int dim>
  class SurfaceForce : public Function<dim>
  {
  public:
    /**
     * @brief Construct a new Surface Force object.
     *
     * @param force_value The value of the surface force.
     */
    SurfaceForce(const double force_value)
      : Function<dim>()
      , force_value(force_value)
    {}

    /**
     * @brief Returns the value of the surface force at the point p.
     *
     * @param p Point
     * @param component Component from the output vector to be returned
     * @return double
     */
    virtual double
    value(const Point<dim> &p, const unsigned int component = 0) const override;

  private:
    const double force_value;
  };


  /**
   * @brief Class for the density of the body force in N/m^3
   *
   * @tparam dim Space dimension
   */
  template <int dim>
  class BodyForce : public Function<dim>
  {
  public:
    /**
     * @brief Construct a new Body Force object.
     *
     * @param rho Mass density
     */
    BodyForce(const double rho)
      : Function<dim>()
      , rho(rho)
      , force_value(grav * rho)
    {}

    /**
     * @brief Returns one component of the body force at the point p.
     *
     * @param p Point
     * @param component Component from the output vector to be returned
     * @return double
     */
    virtual double
    value(const Point<dim> &p, const unsigned int component) const override;

    /**
     * @brief Creates a list of body force values for one component.
     *
     * @param points Vector of points
     * @param values Vector of doubles to be overridden with body force values
     * @param component The body force component to be used.
     */
    virtual void
    value_list(const std::vector<Point<dim>> &points,
               std::vector<double> &          values,
               const unsigned int             component) const;

    /**
     * @brief Returns the body force at the point p.
     *
     * @param point Point
     * @param value Vector to be overridden with the body force.
     */
    virtual void
    vector_value(const Point<dim> &point, Vector<double> &value) const override;

    /**
     * @brief Creates a list of body force values.
     *
     * @param points Vector of points
     * @param value_list Vector of points to be overridden
     *                   with body force values
     */
    virtual void
    vector_value_list(const std::vector<Point<dim>> &points,
                      std::vector<Vector<double>> &  value_list) const override;

  private:
    /**
     * Gravitational acceleration
     */
    const double grav = 9.81;

    /**
     * Mass density (of steel)
     */
    const double rho;

    /**
     * Body force density: #grav * #rho
     */
    const double force_value;
  };


  /**
   * @brief Class for the Lamé parameters of linear elasticity
   *
   * @tparam dim Space dimension
   */
  template <int dim>
  class LamePrm : public Function<dim>
  {
  public:
    LamePrm();
    /**
     * @brief Constructor
     *
     * @param global_parameters Parameters needed for many classes.
     *
     * This functions constructs lambda and uses #global_parameters to
     * implement the structure of the material.
     *
     * The structure can be oscillating, i.e. lambda depends on a trigonometric
     * function (in our case the sine function).
     *
     * Moreover, the structure can be layered with layers in
     * x-,y- and z-direction.
     *
     * All of this and also the mean value of lambda must be specified in the
     * parameter file.
     */
    LamePrm(const unsigned int &               n_x_layers,
            const unsigned int &               n_y_layers,
            const unsigned int &               n_z_layers,
            const double &                     mean,
            const std::vector<unsigned int> &  index_set,
            const std::map<std::string, bool> &material_structure,
            const Point<dim> &                 init_p1,
            const Point<dim> &                 init_p2);

    LamePrm(const double &                     fr_tmp,
            const double &                     mean,
            const std::map<std::string, bool> &material_structure,
            const Point<dim> &                 init_p1,
            const Point<dim> &                 init_p2);

    LamePrm(const LamePrm<dim> &other) = default;

    /**
     * @brief Returns the value of lambda at the point p.
     *
     * @param p Point
     * @param component Component from the output vector to be returned
     * @return double
     */
    virtual double
    value(const Point<dim> &p, const unsigned int component = 0) const override;

  private:
    unsigned int n_x_layers;
    unsigned int n_y_layers;
    unsigned int n_z_layers;
    /**
     * @see Globalparameters::lambda_fr
     */
    double fr;

    /**
     * @brief The sort of structure of the material
     *
     * The entry "oscillations" is true if the material parameters oscillate.
     *
     * The entry "horizontal layers" is true if the material consists of
     * horizontal layers/layers in x-direction.
     *
     * The entry "vertical layers" is true if the material consists of
     * vertical layers.
     *
     * The entry "y-layers" is true if the material consists of
     * layers in y-directions.
     *
     * Only one entry can be true
     */
    std::map<std::string, bool> material_structure;
    /**
     * @see Globalparameters::init_p1
     */
    Point<dim> init_p1;

    /**
     * @see Globalparameters::init_p2
     */
    Point<dim> init_p2;

    /**
     * The inverse of the size of the layers in the structure
     * if the material is layered.
     */
    double layer_size_inv_x;
    double layer_size_inv_y;
    double layer_size_inv_z;

    /**
     * Contains the lambda values for each layer
     * if the material structure is layered.
     */
    std::vector<double> values;
  };

  // exernal template instantiations
  extern template class BodyForce<2>;
  extern template class SurfaceForce<2>;
  extern template class LamePrm<2>;

  extern template class BodyForce<3>;
  extern template class SurfaceForce<3>;
  extern template class LamePrm<3>;
} // namespace Elasticity

#endif // _INCLUDE_FORCES_AND_LAME_PARAMETERS_H_