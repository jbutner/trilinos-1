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

#ifndef THYRA_DEFAULT_MULTIPLIED_LINEAR_OP_DECL_HPP
#define THYRA_DEFAULT_MULTIPLIED_LINEAR_OP_DECL_HPP

#include "Thyra_MultipliedLinearOpBase.hpp"
#include "Thyra_SingleScalarLinearOpBase.hpp"
#include "Teuchos_ConstNonconstObjectContainer.hpp"
#include "Teuchos_Handleable.hpp"


namespace Thyra {


/** \brief Concrete composite <tt>LinearOpBase</tt> subclass that creates an
 * implicitly multiplied linear operator out of one or more constituent
 * <tt>LinearOpBase</tt> objects.
 *
 * This class represents a multiplied linear operator <tt>M</tt> of the form:

 \verbatim
 
 M = Op[0] * Op[1] * ... * Op[numOps-1]

 \endverbatim

 * where <tt>Op[]</tt> is an array of <tt>numOps</tt> <tt>LinearOp</tt>
 * objects.  Of course the operator <tt>M</tt> is not constructed explicitly
 * but instead just applies the constituent linear operators accordingly using
 * temporaries.
 *
 * In other words, this class defines <tt>apply()</tt> as:

 \verbatim

 y = alpha*M*x + beta*y
   = alpha * ( Op[0] * ( Op[1] * ( .... ( Op[numOps-1] * x ) ... ) ) ) + beta * y

 \endverbatim

 * for the case where <tt>M_trans==NOTRANS</tt> and as:

 \verbatim

 y = alpha*M'*x + beta*y
   = alpha * ( Op[numOps-1]' * ( .... ( Op[1]' * ( Op[0]' * x ) ) ... ) ) + beta * y

 \endverbatim

 * for the case where <tt>real_trans(M_trans)!=NOTRANS</tt> (where the
 * transpose <tt>'</tt> either defines <tt>TRANS</tt> or <tt>CONJTRANS</tt>).
 *
 * Constructing a multiplied operator is easy.  For example, suppose one wants
 * to construct the multiplied operator <tt>D = A * B' * C</tt>.  To do so one
 * would do:

 \code

 template<class Scalar>
 void constructD(
    const RCP<const Thyra::LinearOpBase<Scalar> >   &A
    ,const RCP<const Thyra::LinearOpBase<Scalar> >  &B
    ,const RCP<const Thyra::LinearOpBase<Scalar> >  &C
    ,RCP<const Thyra::LinearOpBase<Scalar> >        *D
    )
 {
   typedef RCP<const Thyra::LinearOpBase<Scalar> > LOB;
   *D = Teuchos::rcp(
     new Thyra::DefaultMultipliedLinearOp<Scalar>(
       3, Teuchos::arrayArg<LOB>(A,adjoin(B),C)()
       )
     );
 }

 \endcode

 * Rather than calling the constructor directly, consider using the non-member helper
 * functions described \ref Thyra_Op_Vec_MultipliedLinearOp_helpers_grp "here".
 *
 * \ingroup Thyra_Op_Vec_ANA_Development_grp
 */
template<class Scalar>
class DefaultMultipliedLinearOp
  : virtual public MultipliedLinearOpBase<Scalar>,        // Public interface
    virtual protected SingleScalarLinearOpBase<Scalar>,   // Implementation detail
    virtual public Teuchos::Handleable<LinearOpBase<Scalar> >
{
public:

#ifdef THYRA_INJECT_USING_DECLARATIONS
  using SingleScalarLinearOpBase<Scalar>::apply;
#endif

  /* */
  TEUCHOS_GET_RCP(LinearOpBase<Scalar>);

  /** @name Constructors/initializers/accessors */
  //@{

  /** \brief Constructs to uninitialized.
   *
   * <b>Postconditions:</b><ul>
   * <li><tt>this->numOps()==0</tt>
   * </ul>
   */
  DefaultMultipliedLinearOp();

  /** \brief Initialize given a list of non-const linear operators.
   *
   * @param  numOps  [in] Number of constituent operators.
   * @param  Ops     [in] Array (length <tt>numOps</tt>) of
   *                 constituent linear operators and their
   *                 aggregated default definitions of the
   *                 non-transposed operator.
   *
   * <b>Preconditions:</b><ul>
   * <li><tt>numOps > 0</tt>
   * <li><tt>Ops != NULL</tt>
   * <li><tt>Ops[k].op().get()!=NULL</tt>, for <tt>k=0...numOps-1</tt>
   * </ul>
   *
   * <b>Postconditions:</b><ul>
   * <li><tt>this->numOps()==numOps</tt>
   * <li><tt>this->getOp(k).op().get()==Ops[k].op().get()</tt>, for <tt>k=0...numOps-1</tt>
   * </ul>
   */
  void initialize(
    const ArrayView<const RCP<LinearOpBase<Scalar> > > &Ops );

  /** \brief Initialize given a list of const linear operators.
   *
   * @param  numOps  [in] Number of constituent operators.
   * @param  Ops     [in] Array (length <tt>numOps</tt>) of
   *                 constituent linear operators and their
   *                 aggregated default definitions of the
   *                 non-transposed operator.
   *
   * <b>Preconditions:</b><ul>
   * <li><tt>numOps > 0</tt>
   * <li><tt>Ops != NULL</tt>
   * <li><tt>Ops[k].op().get()!=NULL</tt>, for <tt>k=0...numOps-1</tt>
   * </ul>
   *
   * <b>Postconditions:</b><ul>
   * <li><tt>this->numOps()==numOps</tt>
   * <li><tt>this->getOp(k).op().get()==Ops[k].op().get()</tt>, for <tt>k=0...numOps-1</tt>
   * </ul>
   */
  void initialize(
    const ArrayView<const RCP<const LinearOpBase<Scalar> > > &Ops );

  /** \brief Set to uninitialized.
   *
   * <b>Postconditions:</b><ul>
   * <li><tt>this->numOps()==0</tt>
   * </ul>
   */
  void uninitialize();

  //@}

  /** @name Overridden from MultipliedLinearOpBase */
  //@{

  /** \brief . */
  int numOps() const;
  /** \brief . */
  bool opIsConst(const int k) const;
  /** \brief . */
  RCP<LinearOpBase<Scalar> > getNonconstOp(const int k);
  /** \brief . */
  RCP<const LinearOpBase<Scalar> > getOp(const int k) const;

  //@}

  /** @name Overridden from LinearOpBase */
  //@{

  /** \brief Returns <tt>this->getOp(0).range() if <t>this->numOps() > 0</tt>
   * and returns <tt>Teuchos::null</tt> otherwise.
   */
  RCP< const VectorSpaceBase<Scalar> > range() const;

  /** \brief Returns <tt>this->getOp(this->numOps()-1).domain()</tt> if
   * <t>this->numOps() > 0</tt> and returns <tt>Teuchos::null</tt> otherwise.
   */
  RCP< const VectorSpaceBase<Scalar> > domain() const;

  /** \brief . */
  RCP<const LinearOpBase<Scalar> > clone() const;

  //@}

  /** @name Overridden from Teuchos::Describable */
  //@{
                                                
  /** \brief Prints just the name <tt>DefaultMultipliedLinearOp</tt> along with
   * the overall dimensions and the number of constituent operators.
   */
  std::string description() const;

  /** \brief Prints the details about the constituent linear operators.
   *
   * This function outputs different levels of detail based on the value passed in
   * for <tt>verbLevel</tt>:
   *
   * ToDo: Finish documentation!
   */
  void describe(
    Teuchos::FancyOStream &out,
    const Teuchos::EVerbosityLevel verbLevel
    ) const;

  //@}

protected:

  /** @name Overridden from SingleScalarLinearOpBase */
  //@{

  /** \brief Returns <tt>true</tt> only if all constituent operators support
   * <tt>M_trans</tt>.
   */
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

public:

  /** \name Deprecated. */
  //@{


  /** \brief Deprecated. */
  DefaultMultipliedLinearOp(
    const int numOps_in,
    const RCP<LinearOpBase<Scalar> > Ops[]
    )
    {
      initialize(numOps_in, Ops);
    }

  /** \brief Deprecated. */
  DefaultMultipliedLinearOp(
    const int numOps_in,
    const RCP<const LinearOpBase<Scalar> > Ops[]
    )
    {
      initialize(numOps_in, Ops);
    }

  /** \brief Deprecated. */
  void initialize(
    const int numOps_in,
    const RCP<LinearOpBase<Scalar> > Ops[]
    )
    {
      initialize(Teuchos::arrayView(Ops, numOps_in));
    }

  /** \brief Deprecated. */
  void initialize(
    const int numOps_in,
    const RCP<const LinearOpBase<Scalar> > Ops[]
    )
    {
      initialize(Teuchos::arrayView(Ops, numOps_in));
    }

  //@}

private:

  Array<Teuchos::ConstNonconstObjectContainer<LinearOpBase<Scalar> > > Ops_;

  void assertInitialized() const;
  void validateOps();
  void setupDefaultObjectLabel();

  // Not defined and not to be called
  DefaultMultipliedLinearOp(const DefaultMultipliedLinearOp&);
  DefaultMultipliedLinearOp& operator=(const DefaultMultipliedLinearOp&);

};


/** \brief Nonmember constructor.
 *
 * \relates DefaultMultipliedLinearOp
 */
template<class Scalar>
RCP<DefaultMultipliedLinearOp<Scalar> >
defaultMultipliedLinearOp()
{
  return Teuchos::rcp(new DefaultMultipliedLinearOp<Scalar>);
}


/** \brief Form an implicit multiplication of two linear operators: <tt>M = A
 * * B</tt>.
 *
 * \relates DefaultMultipliedLinearOp
 */
template<class Scalar>
RCP<LinearOpBase<Scalar> >
nonconstMultiply(
  const RCP<LinearOpBase<Scalar> > &A,
  const RCP<LinearOpBase<Scalar> > &B,
  const std::string &M_label = ""
  );


/** \brief Form an implicit multiplication of two linear operators: <tt>M = A
 * * B</tt>.
 *
 * \relates DefaultMultipliedLinearOp
 */
template<class Scalar>
RCP<const LinearOpBase<Scalar> >
multiply(
  const RCP<const LinearOpBase<Scalar> > &A,
  const RCP<const LinearOpBase<Scalar> > &B,
  const std::string &M_label = ""
  );


/** \brief Form an implicit multiplication of three linear operators: <tt>M =
 * A * B * C</tt>.
 *
 * \relates DefaultMultipliedLinearOp
 */
template<class Scalar>
RCP<const LinearOpBase<Scalar> >
multiply(
  const RCP<const LinearOpBase<Scalar> > &A,
  const RCP<const LinearOpBase<Scalar> > &B,
  const RCP<const LinearOpBase<Scalar> > &C,
  const std::string &M_label = ""
  );


// /////////////////////////////////
// Inline members


template<class Scalar>
inline
void DefaultMultipliedLinearOp<Scalar>::assertInitialized() const
{
#ifdef TEUCHOS_DEBUG
  TEST_FOR_EXCEPT( !( numOps() > 0 ) );
#endif
}


}	// end namespace Thyra


#endif	// THYRA_DEFAULT_MULTIPLIED_LINEAR_OP_DECL_HPP