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

#ifndef THYRA_DEFAULT_IDENTITY_LINEAR_OP_DECL_HPP
#define THYRA_DEFAULT_IDENTITY_LINEAR_OP_DECL_HPP

#include "Thyra_IdentityLinearOpBase.hpp"
#include "Thyra_SingleScalarLinearOpBase.hpp"
#include "Teuchos_ConstNonconstObjectContainer.hpp"
#include "Teuchos_arrayArg.hpp"

namespace Thyra {

/** \brief Represents a identity linear operator <tt>M = I</tt>.
 *
 * This class implements:

 \verbatim

 y = alpha*op(M)*x + beta*y

 =>

 y = alpha*x + beta*y

 \endverbatim

 * \ingroup Thyra_Op_Vec_ANA_Development_grp
 */
template<class Scalar>
class DefaultIdentityLinearOp
  : virtual public IdentityLinearOpBase<Scalar>          // Public interface
  , virtual protected SingleScalarLinearOpBase<Scalar>   // Implementation detail
{
public:

#ifdef THYRA_INJECT_USING_DECLARATIONS
  using SingleScalarLinearOpBase<Scalar>::apply;
#endif

  /** @name Constructors/initializers/accessors */
  //@{

  /** \brief Constructs to uninitialized.
   *
   * <b>Postconditions:</b><ul>
   * <li><tt>this->range().get()==NULL</tt>
   * </ul>
   */
  DefaultIdentityLinearOp();

  /** Calls <tt>initialize()</tt>.
   */
  DefaultIdentityLinearOp(
    const Teuchos::RCP<const VectorSpaceBase<Scalar> >   &space
    );

  /** \brief Initialize given a list of non-const linear operators.
   *
   * \param  range  [in] Range vector space.
   * \param  range  [in] Domain vector space.
   *
   * <b>Preconditions:</b><ul>
   * <li><tt>range.get()!=NULL</tt>
   * <li><tt>domain.get()!=NULL</tt>
   * </ul>
   *
   * <b>Postconditions:</b><ul>
   * <li><tt>this->range().get()==range.get()</tt>
   * <li><tt>this->domain().get()==domain.get()</tt>
   * </ul>
   */
  void initialize(
    const Teuchos::RCP<const VectorSpaceBase<Scalar> >   &space
    );

  /** \brief Set to uninitialized.
   *
   * <b>Postconditions:</b><ul>
   * <li><tt>this->range().get()==NULL</tt>
   * </ul>
   */
  void uninitialize();

  //@}

  /** @name Overridden from LinearOpBase */
  //@{
  
  /** \brief Returns <tt>Teuchos::null</tt> if uninitialized. */
  Teuchos::RCP< const VectorSpaceBase<Scalar> > range() const;
  
  /** \brief Returns <tt>Teuchos::null</tt> if uninitialized. */
  Teuchos::RCP< const VectorSpaceBase<Scalar> > domain() const;
  
  /** \brief . */
  Teuchos::RCP<const LinearOpBase<Scalar> > clone() const;
  
  //@}
  
  /** @name Overridden from Teuchos::Describable */
  //@{
                                                
  /** \brief Prints just the name <tt>DefaultIdentityLinearOp</tt> along with the
   * overall dimensions.
   */
  std::string description() const;

  //@}

protected:

  /** @name Overridden from SingleScalarLinearOpBase */
  //@{

  /** \brief Returns <tt>true</tt> . */
  bool opSupported(EOpTransp M_trans) const;

  /** \brief . */
  void apply(
    const EOpTransp                     M_trans
    ,const MultiVectorBase<Scalar>    &X
    ,MultiVectorBase<Scalar>          *Y
    ,const Scalar                     alpha
    ,const Scalar                     beta
    ) const;

  //@}

private:

  Teuchos::RCP<const VectorSpaceBase<Scalar> >  space_;

  // Not defined and not to be called
  DefaultIdentityLinearOp(const DefaultIdentityLinearOp&);
  DefaultIdentityLinearOp& operator=(const DefaultIdentityLinearOp&);

};


/** \brief Create an identity linear operator with given a vector space.
 *
 * \relates DefaultIdentityLinearOp
 */
template<class Scalar>
Teuchos::RCP<const LinearOpBase<Scalar> >
identity(
  const Teuchos::RCP<const VectorSpaceBase<Scalar> > &space,
  const std::string &label = ""
  );


}	// end namespace Thyra


#endif	// THYRA_DEFAULT_IDENTITY_LINEAR_OP_DECL_HPP