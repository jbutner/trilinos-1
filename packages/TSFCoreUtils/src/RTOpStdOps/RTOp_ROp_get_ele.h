/* ///////////////////////////////////////////// */
/* RTOp_ROp_get_ele.h */
/* */
/* Copyright (C) 2001 Roscoe Ainsworth Bartlett */
/* */
/* This is free software; you can redistribute it and/or modify it */
/* under the terms of the "Artistic License" (see the web site */
/*   http://www.opensource.org/licenses/artistic-license.html). */
/* This license is spelled out in the file COPYING. */
/* */
/* This software is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* above mentioned "Artistic License" for more details. */

#ifndef RTOP_ROP_GET_ELE_H
#define RTOP_ROP_GET_ELE_H

#include "RTOp.h"

#ifdef __cplusplus
extern "C" {
#endif


/** \file RTOp_ROp_get_ele.h Reduction operator for looking up the value of an element.
  *
  * <tt>targ_obj <- v[0](i)</tt>
  *
  * This operator is only defined to allow one vector argument
  * (<tt>num_vecs == 1</tt>) <tt>v[0]</tt>
  * but it can handle sparse and dense vectors.  Using a reduction
  * operator to lookup and an element will be reasonably efficient for some
  * types of vector subclasses (i.e. dense and sparse serial and parallel vectors)
  * but very slow for others (i.e. out-of-core vectors).  It would
  * be better for vector subclasses to implement this operation directly
  * but if they don't you can use this operation to lookup an element.
  * Of course the user should not abuse this operation in order
  * to access a bunch of elements in a vector.  To do so would
  * be very inefficient.  This is what reduction operators where
  * designed to avoid.
  */
/*@{ */

/* Virtual function table */
extern const struct RTOp_RTOp_vtbl_t RTOp_ROp_get_ele_vtbl;

/* Constructor */
int RTOp_ROp_get_ele_construct( RTOp_index_type i, struct RTOp_RTOp* op );

/* Reinitialize the index of the element being looked for */
int RTOp_ROp_get_ele_set_i( RTOp_index_type i, struct RTOp_RTOp* op );

/* Destructor */
int RTOp_ROp_get_ele_destroy( struct RTOp_RTOp* op );

/* Extract the value of the element */
RTOp_value_type RTOp_ROp_get_ele_val(RTOp_ReductTarget targ_obj);

/*@} */

#ifdef __cplusplus
}
#endif

#endif  /* RTOP_ROP_GET_ELE_H */
