// @HEADER
// ***********************************************************************
// 
//    Thyra: Interfaces and Support for Abstract Numerical Algorithms
//                 Copyright (2004) Sandia Corporation
// 
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
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
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// ***********************************************************************
// @HEADER


#ifndef THYRA_DEFAULT_SERIAL_DENSE_LINEAR_OP_WITH_SOLVE_DECL_HPP
#define THYRA_DEFAULT_SERIAL_DENSE_LINEAR_OP_WITH_SOLVE_DECL_HPP


#include "Thyra_LinearOpWithSolveBaseDecl.hpp"
#include "Thyra_SingleScalarLinearOpWithSolveBaseDecl.hpp"
#include "RTOpPack_LapackWrappers.hpp"


namespace Thyra {


/* \brief . */
inline RTOpPack::ETransp convertToRTOpPackETransp( const EOpTransp transp )
{
#ifdef TEUCHOS_DEBUG
  TEST_FOR_EXCEPT(transp == CONJ);
#endif
  switch(transp) {
    case NOTRANS:
      return RTOpPack::NOTRANS;
    case TRANS:
      return RTOpPack::TRANS;
    case CONJTRANS:
      return RTOpPack::CONJTRANS;
    default:
      TEST_FOR_EXCEPT(true);
  }
  return RTOpPack::NOTRANS; // will never be called!
}


/** \brief Simple concreate subclass of <tt>LinearOpWithSolveBase</tt> for
 * serial dense matrices implemented using LAPACK.
 *
 * This class uses the helper class <tt>DetachedMultiVectorView</tt> to
 * extract an explicit view of the matrix elements and then uses
 * <tt>Teuchos::LAPACK</tt> to factor <tt>M = L * U</tt> and then do
 * back-solves with the factors <tt>L</tt> and <tt>U</tt>.
 *
 * Even through this class accesses explicit matrix entries and is called
 * <tt>SerialDense</tt>, it is still considered an ANA subclass since it does
 * not have any direct dependance on a specific computing environment or
 * concreate operator/vector/vectorspace implementation.
 *
 * ToDo: Finish Documentation!
 */
template<class Scalar>
class DefaultSerialDenseLinearOpWithSolve :
    virtual public LinearOpWithSolveBase<Scalar>, // Public interface
    virtual protected SingleScalarLinearOpWithSolveBase<Scalar> // Implementation class
{
public:
  
  /** @name Constructors/initializers/accessors */
  //@{

  /** \brief . */
  DefaultSerialDenseLinearOpWithSolve();

  /** \brief . */
  void initialize( const RCP<const MultiVectorBase<Scalar> > &M );

  /** \brief . */
  RCP<const LinearOpBase<Scalar> > getFwdOp() const;

  //@}

  /** @name Overridden from LinearOpBase */
  //@{

  /** \brief . */
  RCP<const VectorSpaceBase<Scalar> > range() const;
  /** \brief . */
  RCP<const VectorSpaceBase<Scalar> > domain() const;

  //@}

protected:
  
  /** @name Overridden from SingleScalarLinearOpBase */
  //@{

  /** \brief . */
  bool opSupported(EOpTransp M_trans) const;
  /** \brief . */
 void apply(
   const EOpTransp M_trans,
   const MultiVectorBase<Scalar> &X,
   MultiVectorBase<Scalar> *Y,
   const Scalar alpha,
   const Scalar beta
   ) const;

  //@}

  /** @name Overridden from SingleScalarLinearOpWithSolveBase */
  //@{

  /** \brief . */
  bool solveSupportsTrans(EOpTransp M_trans) const;
  /** \brief . */
  bool solveSupportsSolveMeasureType(
    EOpTransp M_trans, const SolveMeasureType& solveMeasureType) const;
  /** \brief . */
  void solve(
    const EOpTransp M_trans,
    const MultiVectorBase<Scalar> &B,
    MultiVectorBase<Scalar> *X,
    const int numBlocks,
    const BlockSolveCriteria<Scalar> blockSolveCriteria[],
    SolveStatus<Scalar> blockSolveStatus[]
    ) const;

 //@}

private:

  // /////////////////////////
  // Private data members

  RCP<const MultiVectorBase<Scalar> > M_;
  RTOpPack::ConstSubMultiVectorView<Scalar> LU_;
  Array<Teuchos_Index> ipiv_;

  // /////////////////////////
  // Private member functions

  static void factorize(
    const MultiVectorBase<Scalar> &M,
    const Ptr<RTOpPack::ConstSubMultiVectorView<Scalar> > &LU,
    const Ptr<Array<Teuchos_Index> > &ipiv
    );

  static void backsolve(
    const RTOpPack::ConstSubMultiVectorView<Scalar> &LU,
    const ArrayView<const int> ipiv,
    const EOpTransp transp,
    const MultiVectorBase<Scalar> &B,
    const Ptr<MultiVectorBase<Scalar> > &X
    );

  // Not defined and not to be called
  DefaultSerialDenseLinearOpWithSolve(const DefaultSerialDenseLinearOpWithSolve&);
  DefaultSerialDenseLinearOpWithSolve& operator=(const DefaultSerialDenseLinearOpWithSolve&);

};


/** \brief Nonmember constructor.
 *
 * \relates DefaultSerialDenseLinearOpWithSolve
 */
template<class Scalar>
RCP<DefaultSerialDenseLinearOpWithSolve<Scalar> >
defaultSerialDenseLinearOpWithSolve()
{
  return Teuchos::rcp(new DefaultSerialDenseLinearOpWithSolve<Scalar>);
}


/** \brief Nonmember constructor.
 *
 * \relates DefaultSerialDenseLinearOpWithSolve
 */
template<class Scalar>
RCP<DefaultSerialDenseLinearOpWithSolve<Scalar> >
defaultSerialDenseLinearOpWithSolve( const RCP<const MultiVectorBase<Scalar> > &M )
{
  RCP<DefaultSerialDenseLinearOpWithSolve<Scalar> >
    M_lows = Teuchos::rcp(new DefaultSerialDenseLinearOpWithSolve<Scalar>());
  M_lows->initialize(M);  // With throw if singular
  return M_lows;
}


}	// end namespace Thyra


#endif	// THYRA_DEFAULT_SERIAL_DENSE_LINEAR_OP_WITH_SOLVE_DECL_HPP
