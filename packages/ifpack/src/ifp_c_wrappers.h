/*@HEADER
// ***********************************************************************
// 
//       Ifpack: Object-Oriented Algebraic Preconditioner Package
//                 Copyright (2002) Sandia Corporation
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
//@HEADER
*/

#ifndef _IFP_C_WRAPPER_H_
#define _IFP_C_WRAPPER_H_

#define IFP_DENSE 	0
#define IFP_SPARSE 	1

#define IFP_NONE 	0
#define IFP_BJACOBI 	1
#define IFP_BSOR 	2
#define IFP_BSSOR 	3
#define IFP_BILUK 	4

#define IFP_LU 	 	1
#define IFP_INVERSE 	2
#define IFP_SVD 	 	3
#define IFP_DIAG 	10
#define IFP_SOR 	 	12
#define IFP_SSOR 	13

#ifdef __cplusplus
extern "C" {
#endif

void ifp_blockmatrix(
  void    **bmat,
  int     n,
  double  *a,
  int     *ja,
  int     *ia,
  int     nb,
  int     *kvst,
  int     type);

void ifp_freeblockmatrix(
        void    *bmat);

void ifp_preconditioner(
        void    **precon,
  const void    *bmat,
  const int     global,
  const double  gparam1,
  const double  gparam2,
  const int     local,
  const double  lparam1,
  const double  lparam2);

void ifp_freepreconditioner( void *precon);

void ifp_freebiluk( void *precon);

void ifp_matvec(
  void    *bmat,
  int     nr,
  int     nc,
  const double *u,
  int     ldu,
  double *v,
  int     ldv);

void ifp_apply(
  void    *prec,
  int     nr,
  int     nc,
  const double *u,
  int     ldu,
  double *v,
  int     ldv);

void ifp_BJacobi_condest(void *M);

void ifp_biluk_condest(void *M);
void ifp_biluk_stats(void *M);

#ifdef __cplusplus
}
#endif

#endif /* _IFP_C_WRAPPER_H_ */