/*
  This file is part of TACS: The Toolkit for the Analysis of Composite
  Structures, a parallel finite-element code for structural and
  multidisciplinary design optimization.

  Copyright (C) 2010 University of Toronto
  Copyright (C) 2012 University of Michigan
  Copyright (C) 2014 Georgia Tech Research Corporation
  Additional copyright (C) 2010 Graeme J. Kennedy and Joaquim
  R.R.A. Martins All rights reserved.

  TACS is licensed under the Apache License, Version 2.0 (the
  "License"); you may not use this software except in compliance with
  the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0
*/

#ifndef TACS_ELEMENT_H
#define TACS_ELEMENT_H

/*
  Basic TACSElement definition

  The purpose of this file is to provide an interface for creating and
  storing different instances of the finite elements that will be used
  by TACS.  This is what should be extended when including more
  elements and not the underlying TACS implementation itself.
*/

#include "TACSElementBasis.h"
#include "TACSElementModel.h"
#include "TACSObject.h"

// The TACSElement base class
class TACSElement : public TACSObject {
 public:
  TACSElement(int _componentNum = 0) { componentNum = _componentNum; }
  virtual ~TACSElement() {}

  /**
    Set the component number for this element.

    The component number can be used to identify groups of elements
    for visualization purposes

    @param comp_num The component number assigned to the element
  */
  void setComponentNum(int comp_num) { componentNum = comp_num; }

  /**
    Get the component number for this element

    @return The component number for the element
  */
  int getComponentNum() { return componentNum; }

  /**
    Get a string representation of the element name

    @return The name of the element
  */
  const char *getObjectName() { return "TACSElement"; }

  /**
    Get the number of degrees of freedom per node for this element

    @return The number of degrees of freedom per node
  */
  virtual int getVarsPerNode() = 0;

  /**
    Get the number of nodes associated with this element

    @return The number of nodes for this element
  */
  virtual int getNumNodes() = 0;

  /**
    Get the number of variables owned by the element
  */
  int getNumVariables() { return getNumNodes() * getVarsPerNode(); }

  /**
    Get the node index where a Lagrange multiplier is defined.

    A negative index indicates that no multiplier is defined. The
    index is relative to the ordering in the element.

    @return Index of a Lagrange multiplier node
  */
  virtual int getMultiplierIndex() { return -1; }

  /**
    Get the element basis class

    @return The TACSElementBasis class associated with this element. Possibly
    NULL.
  */
  virtual TACSElementBasis *getElementBasis() { return NULL; }

  /**
    Get the number of quadrature points for the volume/area of the element
  */
  virtual int getNumQuadraturePoints() = 0;

  /**
    Get the quadrature weight for the n-th quadrature point

    @param n The quadrature point index
    @return The quadrature weight value
  */
  virtual double getQuadratureWeight(int n) = 0;

  /**
    Get the parametric location of the n-th quadrature point

    @param n The quadrature point index
    @param pt The parametric location of the quadrature point
    @return The quadrature weight value
  */
  virtual double getQuadraturePoint(int n, double pt[]) = 0;

  /**
    Get the number of faces or edges for the element

    @return The number of faces/edges for the basis
  */
  virtual int getNumElementFaces() = 0;

  /**
    Get the number of quadrature points for the given face

    @param face The face/edge index
    @return The number of quadrature points for the face
  */
  virtual int getNumFaceQuadraturePoints(int face) = 0;

  /**
    Get the quadrature point for the given face/edge

    The quadrature point and weight are in the original parameter space
    (not parametrized along an edge or face). The tangent parameter
    direction(s) correspond to the directions in parameter space along
    the specified surface. In the case when the parameter space is
    of dimention 1, 2, or 3, there are respectively 0, 1 and 2 tagents
    stored in row major order so that for the 3D case:

    tangent = [d1[0], d1[1], d1[2], d2[0], d2[1], d2[2]]

    Note that the tangents obey the right-hand rule so that
    crossProduct(Xd*d1, Xd*d2) gives an outward-facing normal direction.

    @param face The face/edge index
    @param n The quadrautre point index
    @param pt The quadrature point
    @param tangent Parametric direction(s) parallel to the face
    @return The quadrature weight for the face
  */
  virtual double getFaceQuadraturePoint(int face, int n, double pt[],
                                        double tangent[]) = 0;

  /**
    Get the element model class

    @return The TACSElementModel class associated with this element. Possibly
    NULL.
  */
  virtual TACSElementModel *getElementModel() { return NULL; }

  /**
    Retrieve the initial conditions for time-dependent analysis

    By default, the initial displacements, velocities and accelerations
    are zero.

    @param elemIndex The local element index
    @param Xpts The element node locations
    @param vars The values of the element degrees of freedom
    @param dvars The first time derivative of the element DOF
    @param ddvars The second time derivative of the element DOF
  */
  virtual void getInitConditions(int elemIndex, const TacsScalar Xpts[],
                                 TacsScalar vars[], TacsScalar dvars[],
                                 TacsScalar ddvars[]) {
    int num_vars = getNumNodes() * getVarsPerNode();
    memset(vars, 0, num_vars * sizeof(TacsScalar));
    memset(dvars, 0, num_vars * sizeof(TacsScalar));
    memset(ddvars, 0, num_vars * sizeof(TacsScalar));
  }

  /**
    Compute the kinetic and potential energy within the element.

    This can be used to evaluate the Hamiltonian and test whether the
    element satisfies the Lagrangian equations of motion.

    @param elemIndex The local element index
    @param time The simulation time
    @param Xpts The element node locations
    @param vars The values of the element degrees of freedom
    @param dvars The first time derivative of the element DOF
    @param Te The kinetic energy contributed by this element
    @param Pe the potential energy contributed by this element
  */
  virtual void computeEnergies(int elemIndex, double time,
                               const TacsScalar Xpts[], const TacsScalar vars[],
                               const TacsScalar dvars[], TacsScalar *Te,
                               TacsScalar *Pe) {
    *Te = 0.0;
    *Pe = 0.0;
  }

  /**
    Add the contribution from this element to the residual.

    Note that this simply adds, and does not over-write the residual so
    that multiple contributions can be computed.

    @param elemIndex The local element index
    @param time The simulation time
    @param Xpts The element node locations
    @param vars The values of the element degrees of freedom
    @param dvars The first time derivative of the element DOF
    @param ddvars The second time derivative of the element DOF
    @param res The element residual input/output
  */
  virtual void addResidual(int elemIndex, double time, const TacsScalar Xpts[],
                           const TacsScalar vars[], const TacsScalar dvars[],
                           const TacsScalar ddvars[], TacsScalar res[]) = 0;

  /**
    Add the contribution from this element to the residual and Jacobian.

    Note that this simply adds, and does not over-write the Jacobian so
    that multiple contributions can be computed.

    The Jacobian contribution consists of a linear combination of the
    Jacobians with respect to the variables, and their first and second
    time derivatives as follows:

    mat += alpha*d(res)/d(vars) + beta*d(res)/d(dvars) + gamma*d(res)/d(ddvars)

    @param elemIndex The local element index
    @param time The simulation time
    @param alpha The coefficient for the DOF Jacobian
    @param beta The coefficient for the first time derivative DOF Jacobian
    @param gamma The coefficient for the second time derivative DOF Jacobian
    @param Xpts The element node locations
    @param vars The values of the element degrees of freedom
    @param dvars The first time derivative of the element DOF
    @param ddvars The second time derivative of the element DOF
    @param res The element residual input/output
    @param mat The element Jacobian input/output
  */
  virtual void addJacobian(int elemIndex, double time, TacsScalar alpha,
                           TacsScalar beta, TacsScalar gamma,
                           const TacsScalar Xpts[], const TacsScalar vars[],
                           const TacsScalar dvars[], const TacsScalar ddvars[],
                           TacsScalar res[], TacsScalar mat[]);

  /**
    Compute a specific type of element matrix (mass, stiffness, geometric
    stiffness, etc.)

    @param matType The type of element matrix to compute
    @param elemIndex The local element index
    @param time The simulation time
    @param Xpts The element node locations
    @param vars The values of element degrees of freedom
    @param mat The element matrix output
  */
  virtual void getMatType(ElementMatrixType matType, int elemIndex, double time,
                          const TacsScalar Xpts[], const TacsScalar vars[],
                          TacsScalar mat[]);
  /**
    Get array sizes needed for a matrix-free matrix-vector product

    @param matType The type of matrix to use
    @param elemIndex The element index
    @param _data_size The size of the data to store the matrix-vector product
    @param _temp_size The size of the temporary array needed as an argument
  */
  virtual void getMatVecDataSizes(ElementMatrixType matType, int elemIndex,
                                  int *_data_size, int *_temp_size) {
    *_data_size = 0;
    *_temp_size = 0;
  }

  /**
    Compute the element-wise matrix-vector product

    @param matType The type of element matrix to compute
    @param elemIndex The local element index
    @param data The element data required for a matrix-vector product
    @param temp Temporary array
    @param px The input vector
    @param py The output vector with the added matrix-vector product
  */
  virtual void addMatVecProduct(ElementMatrixType matType, int elemIndex,
                                const TacsScalar data[], TacsScalar temp[],
                                const TacsScalar px[], TacsScalar py[]) {}

  /**
    Evaluate a point-wise quantity of interest.

    @param elemIndex The index of the element
    @param quantityType The integer indicating the pointwise quantity
    @param time The simulation time
    @param n The quadrature point index
    @param pt The quadrature point
    @param Xpts The element node locations
    @param vars The values of the element degrees of freedom
    @param dvars The first time derivative of the element DOF
    @param ddvars The second time derivative of the element DOF
    @param detXd The determinant of the Jacobian transformation
    @param quantity The output quantity of interest
    @return Integer indicating the number of defined quantities
  */
  virtual int evalPointQuantity(int elemIndex, int quantityType, double time,
                                int n, double pt[], const TacsScalar Xpts[],
                                const TacsScalar vars[],
                                const TacsScalar dvars[],
                                const TacsScalar ddvars[], TacsScalar *detXd,
                                TacsScalar *quantity) {
    return 0;  // No quantities defined by default
  }

 private:
  int componentNum;
  // Defines order of finite differencing method
  int fdOrder = 2;
};

#endif  // TACS_ELEMENT_H