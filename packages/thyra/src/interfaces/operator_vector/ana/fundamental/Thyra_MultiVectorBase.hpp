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

#ifndef THYRA_MULTI_VECTOR_BASE_HPP
#define THYRA_MULTI_VECTOR_BASE_HPP

#include "Thyra_MultiVectorBaseDecl.hpp"
#include "Thyra_LinearOpBase.hpp"


namespace Thyra {


// Provide access to the columns as VectorBase objects


template<class Scalar>
RCP<const VectorBase<Scalar> >
MultiVectorBase<Scalar>::colImpl(Index j) const
{
  return const_cast<MultiVectorBase*>(this)->nonconstColImpl(j);
}


// Overridden methods from LinearOpBase


template<class Scalar>
RCP<const LinearOpBase<Scalar> >
MultiVectorBase<Scalar>::clone() const
{
  return this->clone_mv();
}


//
// Deprecated public function
//


template<class Scalar>
void MultiVectorBase<Scalar>::applyOp(
  const RTOpPack::RTOpT<Scalar> &primary_op,
  const int num_multi_vecs,
  const MultiVectorBase<Scalar>*const multi_vecs_in[],
  const int num_targ_multi_vecs,
  MultiVectorBase<Scalar>*const targ_multi_vecs_inout[],
  RTOpPack::ReductTarget*const reduct_objs_inout[],
  const Index primary_first_ele_offset,
  const Index primary_sub_dim,
  const Index primary_global_offset,
    const Index secondary_first_ele_offset,
  const Index secondary_sub_dim_in
  ) const
{
  Array<Ptr<const MultiVectorBase<Scalar> > > multi_vecs;
  for (int k = 0; k < num_multi_vecs; ++k)
    multi_vecs.push_back(Teuchos::ptr(multi_vecs_in[k]));
  Array<Ptr<MultiVectorBase<Scalar> > > targ_multi_vecs;
  for (int k = 0; k < num_targ_multi_vecs; ++k)
    targ_multi_vecs.push_back(Teuchos::ptr(targ_multi_vecs_inout[k]));
  Array<Ptr<RTOpPack::ReductTarget> > reduct_objs;
  if (reduct_objs_inout) {
    const int secondary_sub_dim =
      (
        secondary_sub_dim_in >= 0
        ? secondary_sub_dim_in
        : ( num_multi_vecs ? multi_vecs[0]->domain() : targ_multi_vecs[0]->domain() )->dim()
        );
    const int num_reduct_objs = ( secondary_sub_dim - secondary_first_ele_offset );
    for (int k = 0; k < num_reduct_objs; ++k)
      reduct_objs.push_back(Teuchos::ptr(reduct_objs_inout[k]));
  }
  mvMultiReductApplyOpImpl(
    primary_op
    ,multi_vecs,targ_multi_vecs
    ,reduct_objs,primary_first_ele_offset,primary_sub_dim,primary_global_offset
    ,secondary_first_ele_offset,secondary_sub_dim_in
    );
}


template<class Scalar>
void MultiVectorBase<Scalar>::applyOp(
  const RTOpPack::RTOpT<Scalar> &primary_op,
  const RTOpPack::RTOpT<Scalar> &secondary_op,
  const int num_multi_vecs,
  const MultiVectorBase<Scalar>*const multi_vecs_in[],
  const int num_targ_multi_vecs,
  MultiVectorBase<Scalar>*const targ_multi_vecs_inout[],
  RTOpPack::ReductTarget* reduct_obj,
  const Index primary_first_ele_offset,
  const Index primary_sub_dim,
  const Index primary_global_offset,
  const Index secondary_first_ele_offset,
  const Index secondary_sub_dim
  ) const
{
  Array<Ptr<const MultiVectorBase<Scalar> > > multi_vecs;
  for (int k = 0; k < num_multi_vecs; ++k)
    multi_vecs.push_back(Teuchos::ptr(multi_vecs_in[k]));
  Array<Ptr<MultiVectorBase<Scalar> > > targ_multi_vecs;
  for (int k = 0; k < num_targ_multi_vecs; ++k)
    targ_multi_vecs.push_back(Teuchos::ptr(targ_multi_vecs_inout[k]));
  mvSingleReductApplyOpImpl(
    primary_op, secondary_op
    ,multi_vecs,targ_multi_vecs
    ,Teuchos::ptr(reduct_obj)
    ,primary_first_ele_offset,primary_sub_dim,primary_global_offset
    ,secondary_first_ele_offset,secondary_sub_dim
    );
}


} // end namespace Thyra


#endif // THYRA_MULTI_VECTOR_BASE_HPP