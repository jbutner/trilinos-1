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

#ifndef THYRA_SERIAL_VECTOR_SPACE_CONVERTED_BASE_HPP
#define THYRA_SERIAL_VECTOR_SPACE_CONVERTED_BASE_HPP

#include "Thyra_SerialVectorSpaceConverterBaseDecl.hpp"
#include "Thyra_MultiVectorBase.hpp"
#include "Thyra_DetachedMultiVectorView.hpp"
#include "Teuchos_Workspace.hpp"
#include "Teuchos_TestForException.hpp"

namespace Thyra {

template<class ScalarFrom, class ScalarTo>
void SerialVectorSpaceConverterBase<ScalarFrom,ScalarTo>::convert(
  const MultiVectorBase<ScalarFrom>    &mv_from
  ,MultiVectorBase<ScalarTo>           *mv_to
  ) const
{
#ifdef TEUCHOS_DEBUG
  TEST_FOR_EXCEPT(mv_to==NULL);
#endif
  ConstDetachedMultiVectorView<ScalarFrom>       emv_from(mv_from);
  DetachedMultiVectorView<ScalarTo>  emv_to(*mv_to);
#ifdef TEUCHOS_DEBUG
  TEST_FOR_EXCEPT(emv_from.subDim() != emv_to.subDim());
  TEST_FOR_EXCEPT(emv_from.numSubCols() != emv_to.numSubCols());
#endif
  for( Index j = 0; j < emv_from.numSubCols(); ++j ) {
    for( Index i = 0; i < emv_from.subDim(); ++i ) {
      emv_to(i,j) = emv_from(i,j); // ToDo: Make this faster using optimized copy functions?
    }
  }
}

} // namespace Thyra

#endif // THYRA_SERIAL_VECTOR_SPACE_CONVERTED_BASE_HPP