/* @HEADER@ */
// ************************************************************************
// 
//                              Sundance
//                 Copyright (2005) Sandia Corporation
// 
// Copyright (year first published) Sandia Corporation.  Under the terms 
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government 
// retains certain rights in this software.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//  
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//                                                                                 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA                                                                                
// Questions? Contact Kevin Long (krlong@sandia.gov), 
// Sandia National Laboratories, Livermore, California, USA
// 
// ************************************************************************
/* @HEADER@ */

#ifndef SUNDANCE_BASISDOFTOPOLOGYBASE_H
#define SUNDANCE_BASISDOFTOPOLOGYBASE_H

#include "SundanceDefs.hpp"
#include "SundanceCellType.hpp"
#include "Teuchos_Array.hpp"
#include "Teuchos_Describable.hpp"

namespace SundanceStdFwk {


using Teuchos::Array;
using SundanceStdMesh::CellType;

/** 
 * Abstract interface for specification of the topology of degree-of-freedom
 * (DOF) assignments on reference cells in any dimension. Currently,
 * only an enumerated set of cell types are supported (see <tt>CellType</tt>).
 *
 * A function \f$g(x)\f$ defined on a cell is represented as:

 \f[
 g(x) = \sum_{i=0}^{N-1} \bar{g}_i \psi_i(x)
 \f]

 * where \f$x\in\Re^{D}\f$ is the spatial spatial coordinate for spatical dim
 * \f$D\f$ that lives in the cell's domain, \f$\psi_i(x)\f$ is the \f$i\f$th
 * basis function (of order = <tt>order()</tt>), \f$\bar{g}_i\f$ is the
 * (constant) coefficient for the \f$i\f$th basis function, and \f$N\f$ is the
 * number of basis functions.  Therefore, given the coefficients of the basis
 * function on the cell \f$\bar{g}_i\f$, one can compute the value of the
 * function \f$g(x)\f$ on the cell at any point \f$x\f$ in the domain of the
 * cell given the above summation formula.  This interface refers to the
 * coefficients \f$\bar{g}_i\f$ as degrees of freedom (<b>DOFs</b>).
 *
 * This interface allows the specification basis functions and basis
 * coefficients (i.e. DOFs) to be associated with any of the facets of a cell,
 * including the cell itself without restriction.  See the function
 * <tt>getLocalDOFs()</tt> for how this mapping of DOFs to facets for a single
 * function defined on the cell.
 *
 * It is important to note that each cell type, i.e. the values of the enum
 * <tt>CellType</tt>, has a "agreed upon" geometry for the "reference cell"
 * and this geometry must be known by the client of the basis family and the
 * concrete implementations of the basis family.
 *
 * This is an interoperability interface and is therefore
 * deliberately minimalist. 
 */
class BasisDOFTopologyBase :
    public Teuchos::Describable
{
public:

  /** 
   * \brief Inform caller as to whether a given combination 
   * of cell types is supported. 
   *
   * \param maximalCellType
   *         [in] maximal-dimension cell type to which the cell is connected.
   *         For example, a triangle in 3D could be connected to a prism
   *         or to a tetrahedron. 
   *
   * \param cellType 
   *         [in] type of cell for which we want DOF information.
   *
   */
  virtual bool supportsCellTypePair(
    const CellType& maximalCellType,
    const CellType& cellType
    ) const = 0 ;



  /** \brief Get a description of the DOF numbering and distribution scheme
   * for this basis function on the given cell type.
   *
   * \param  cellType
   *           [in] Specification of the cell topology
   * \param  dofs
   *           [out] Array of dof numbering information, to be filled in
   *           during the call.  On output,
   *           <tt>dofs.size()==dimension(cellType)</tt>.  See description of
   *           <tt>dofs</tt> below for more details.
   *
   * The DOF description is returned in the nested array <tt>dofs</tt>, and is
   * to be interpreted as follows: The outer dimension of the description
   * array <tt>dofs.size()</tt> is <tt>cellDim</tt>, where <tt>cellDim</tt> is
   * the spatial dimension of the cell.  The DOFs attached to facets are
   * stored in array entries <tt>dofs[s]</tt> where <tt>s=0...cellDim-1</tt>.
   * Those associated with the cell body are stored in
   * <tt>dofs[cellDim-1]</tt>. For cell dofs attached to facets, the dof
   * <tt>facetDofIndex</tt> associated with the facet <tt>facetIndex</tt> of
   * facet dimension <tt>facetDim</tt> is given by:

   \code
   dofs[facetDim][facetIndex][faceDofIndex] 
   \endcode
   
   * For dofs attached to the cell body, the local DOF within the entire cell
   * is given by dof is given by

   \code
   dofs[cellDim][0][dofIndex]
   \endcode 

   * More specifically:<ul>
   *
   * <li><tt>dof[facetDim].size()</tt> gives the number of facets of the facet
   * dimension <tt>facetDim</tt>, where <tt>0 <= facetDim <= cellDim</tt>
   *
   * <li><tt>dof[facetDim][facetIndex].size()</tt> gives the number of degrees
   * of freedom (DOFs) on the facet <tt>facetIndex</tt> with facet dimension
   * <tt>facetDim</tt>, where <tt>0 <= facetDim <= cellDim</tt> and <tt>0 <=
   * facetIndex < numFacets(cellType,facetDim)</tt>.
   *
   * </ul>
   *
   * For example, the Lagrange basis functions of order 0 through 3 on 2D
   * triangles would have the following dof arrays:

   \verbatim

   Order 0:

   { {}, {}, {{0}} }
   
   Order 1:

   { { {0}, {1}, {2} }, {}, {} }
    
   Order 2:

   { { {0}, {1}, {2} }, { {3}, {4}, {5} }, {} }
    
   Order 3:

   { { {0}, {1}, {2} }, { {3,4}, {5,6}, {7,8} }, {9} }

   \endverbatim

   * Above, we have used the ordering given in Hughes' textbook.
   */
  virtual void getReferenceDOFs(
    const CellType& maximalCellType,
    const CellType& cellType,
    Array<Array<Array<int> > >& dofs
    ) const = 0 ;



  /** \brief Return the total number of degrees of freedom associated with
   * this basis on a specified cell type. Note: the count returned
   * by this function includes DOFs owned by facets of the specified
   * reference cell. 
   * 
   *
   * \param cellType 
   *         [in] type of cell for which we want DOF information.
   *
   * \param maximalCellType
   *         [in] maximal-dimension cell type to which the cell is connected.
   *         For example, a triangle in 3D could be connected to a prism
   *         or to a tetrahedron. 
   * 
   * \return Number of DOFs associated with the cell type and its facets.    
   */
  virtual int nReferenceDOFs(
    const CellType& maximalCellType,
    const CellType& cellType
    ) const = 0;



  /** \brief Comparison function allowing use of 
   * OrderedHandle<BasisFamilyBase>
   * in sorted containers. This is needed by the MixedDOFMap ctor when
   * it uses an STL map to group functions having the same bases into chunks. 
   *
   *  Note: this method should normally only be called from within the
   *  comparison operator of OrderedHandle, in which context comparisons
   *  between different derived types have already been resolved by 
   *  comparisons of typeid. Thus, we can require that the lessThan()
   *  function be called only with an argument whose typeid is 
   *  equal to that of <tt>*this.</tt> We recommend that all overriding
   *  implementations check that condition.  
   * 
   * \param other 
   *         [in] Pointer to another basis family object. 
   *         Precondition: <tt>typeid(*this)==typeid(*other).</tt>
   */
  virtual bool lessThan(const BasisDOFTopologyBase* other) const = 0 ;
};


} // namespace SundanceStdFwk


#endif