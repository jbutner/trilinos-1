// ///////////////////////////////////////////////////////
// MatrixSymPosDefBandedChol.cpp
//
// Copyright (C) 2001 Roscoe Ainsworth Bartlett
//
// This is free software; you can redistribute it and/or modify it
// under the terms of the "Artistic License" (see the web site
//   http://www.opensource.org/licenses/artistic-license.html).
// This license is spelled out in the file COPYING.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// above mentioned "Artistic License" for more details.

#include <assert.h>

#include <sstream>

#include "ConstrainedOptimizationPack/src/MatrixSymPosDefBandedChol.hpp"
#include "LinAlgPack/src/LinAlgPackAssertOp.hpp"
#include "LinAlgPack/src/LinAlgOpPack.hpp"
#include "LinAlgPack/src/BLAS_Cpp.hpp"
#include "MiReleaseResource_ref_count_ptr.h"
#include "MiWorkspacePack.h"

// LAPACK functions

extern "C" {

FORTRAN_FUNC_DECL_UL(void,DPBTRF,dpbtrf)(
	FORTRAN_CONST_CHAR_1_ARG(UPLO)
	,const FortranTypes::f_int       &N
	,const FortranTypes::f_int       &KD
	,FortranTypes::f_dbl_prec        *AB
	,const FortranTypes::f_int       &LDAB
	,FortranTypes::f_int             *INFO
	);

FORTRAN_FUNC_DECL_UL(void,DPBTRS,dpbtrs)(
	FORTRAN_CONST_CHAR_1_ARG(UPLO)
	,const FortranTypes::f_int       &N
	,const FortranTypes::f_int       &KD
	,const FortranTypes::f_int       &NRHS
	,const FortranTypes::f_dbl_prec  AB[]
	,const FortranTypes::f_int       &LDAB
	,FortranTypes::f_dbl_prec        *B
	,const FortranTypes::f_int       &LDB
	,FortranTypes::f_int             *INFO
	);

} // end namespace LAPACK

namespace ConstrainedOptimizationPack {

MatrixSymPosDefBandedChol::MatrixSymPosDefBandedChol(
	size_type                         n
	,size_type                        kd
	,GenMatrixSlice                   *MB
	,const release_resource_ptr_t&    MB_release_resource_ptr
	,BLAS_Cpp::Uplo                   MB_uplo
	,GenMatrixSlice                   *UB
	,const release_resource_ptr_t&    UB_release_resource_ptr
	,BLAS_Cpp::Uplo                   UB_uplo
	,bool                             update_factor
	,value_type                       scale
	)
{
	initialize(n,kd,MB,MB_release_resource_ptr,MB_uplo
			   ,UB,UB_release_resource_ptr,UB_uplo,update_factor,scale);
}

void MatrixSymPosDefBandedChol::initialize(
	size_type                         n
	,size_type                        kd
	,GenMatrixSlice                   *MB
	,const release_resource_ptr_t&    MB_release_resource_ptr
	,BLAS_Cpp::Uplo                   MB_uplo
	,GenMatrixSlice                   *UB
	,const release_resource_ptr_t&    UB_release_resource_ptr
	,BLAS_Cpp::Uplo                   UB_uplo
	,bool                             update_factor
	,value_type                       scale
	)
{
	// Validate input

	if( n == 0 ) {
		if( kd != 0 )
			throw std::invalid_argument(
				"MatrixSymPosDefBandedChol::initialize(...): Error, "
				"kd must be 0 if n == 0" );
		if( MB != NULL )
			throw std::invalid_argument(
				"MatrixSymPosDefBandedChol::initialize(...): Error, "
				"MB must be NULL if n == 0" );
		if( MB_release_resource_ptr.get() != NULL )
			throw std::invalid_argument(
				"MatrixSymPosDefBandedChol::initialize(...): Error, "
				"MB_release_resource_ptr.get() must be NULL if n == 0" );
		if( UB != NULL )
			throw std::invalid_argument(
				"MatrixSymPosDefBandedChol::initialize(...): Error, "
				"UB must be NULL if n == 0" );
		if( UB_release_resource_ptr.get() != NULL )
			throw std::invalid_argument(
				"MatrixSymPosDefBandedChol::initialize(...): Error, "
				"UB_release_resource_ptr.get() must be NULL if n == 0" );
	}
	else {
		if( kd + 1 > n )
			throw std::invalid_argument(
				"MatrixSymPosDefBandedChol::initialize(...): Error, "
				"kd + 1 can not be larger than n" );
		if( MB == NULL && UB == NULL )
			throw std::invalid_argument(
				"MatrixSymPosDefBandedChol::initialize(...): Error, "
				"MB and UB can not both be NULL" );
		if( MB != NULL && ( MB->rows() != kd + 1 || MB->cols() != n ) )
			throw std::invalid_argument(
				"MatrixSymPosDefBandedChol::initialize(...): Error, "
				"MB is not the correct size" );
		if( UB != NULL && ( UB->rows() != kd + 1 || UB->cols() != n ) )
			throw std::invalid_argument(
				"MatrixSymPosDefBandedChol::initialize(...): Error, "
				"UB is not the correct size" );
	}

	// Set the members

	if( n == 0 ) {
		n_                        = 0;
		kd_                       = 0;
		MB_.bind(GenMatrixSlice());
		MB_release_resource_ptr_  = NULL;
		MB_uplo_                  = BLAS_Cpp::lower;
		UB_.bind(GenMatrixSlice());
		UB_release_resource_ptr_  = NULL;
		UB_uplo_                  = BLAS_Cpp::lower;
		scale_                    = 1.0;
	}
	else {
		// Set the members
		n_                        = n;
		kd_                       = kd;
		if(MB) MB_.bind(*MB);
		MB_release_resource_ptr_  = MB_release_resource_ptr;
		MB_uplo_                  = MB_uplo;
		if(UB) UB_.bind(*UB);
		UB_release_resource_ptr_  = UB_release_resource_ptr;
		UB_uplo_                  = UB_uplo;
		factor_updated_           = UB && !update_factor;
		scale_                    = scale;
		// Update the factorization if we have storage
		if( update_factor )
			update_factorization();
	}
}

// Overridden from MatrixWithOp

size_type MatrixSymPosDefBandedChol::rows() const
{
	return n_;
}

size_type MatrixSymPosDefBandedChol::nz() const
{
	return (2 * kd_ + 1) * n_ - ( (kd_+1)*(kd_+1) - (kd_+1) );
}

std::ostream& MatrixSymPosDefBandedChol::output(std::ostream& out) const
{
	return MatrixWithOp::output(out); // ToDo: Implement specialized version later!
}

void MatrixSymPosDefBandedChol::Vp_StMtV(
	VectorSlice* y, value_type a, BLAS_Cpp::Transp M_trans
	, const VectorSlice& x, value_type b) const
{
	assert_initialized();
	LinAlgPack::Vp_MtV_assert_sizes( y->size(), n_, n_, BLAS_Cpp::no_trans, x.size() );
	if( MB_.rows() ) {
		BLAS_Cpp::sbmv(MB_uplo_,n_,kd_,a,MB_.col_ptr(1),MB_.max_rows(),x.raw_ptr(),x.stride()
					   ,b,y->raw_ptr(),y->stride());
	}
	else if( UB_.rows() ) {
		assert(0); // ToDo: Implement when and if needed!
	}
	else {
		assert(0); // This should not happen!
	}
}

void MatrixSymPosDefBandedChol::Vp_StMtV(
	VectorSlice* y, value_type a, BLAS_Cpp::Transp M_trans
	, const SpVectorSlice& x, value_type b) const
{
	assert_initialized();
	MatrixWithOp::Vp_StMtV(y,a,M_trans,x,b); // ToDo: Implement spacialized operation when needed!
}

void MatrixSymPosDefBandedChol::Vp_StPtMtV(
	VectorSlice* y, value_type a
	, const GenPermMatrixSlice& P, BLAS_Cpp::Transp P_trans
	, BLAS_Cpp::Transp M_trans
	, const VectorSlice& x, value_type b) const
{
	assert_initialized();
	MatrixWithOp::Vp_StPtMtV(y,a,P,P_trans,M_trans,x,b); // ToDo: Implement spacialized operation when needed!
}

void MatrixSymPosDefBandedChol::Vp_StPtMtV(
	VectorSlice* y, value_type a
	, const GenPermMatrixSlice& P, BLAS_Cpp::Transp P_trans
	, BLAS_Cpp::Transp M_trans
	, const SpVectorSlice& x, value_type b) const
{
	assert_initialized();
	MatrixWithOp::Vp_StPtMtV(y,a,P,P_trans,M_trans,x,b); // ToDo: Implement spacialized operation when needed!
}

// Overridden from MatrixFactorized

void MatrixSymPosDefBandedChol::V_InvMtV(
	VectorSlice* y, BLAS_Cpp::Transp M_trans
	, const VectorSlice& x) const
{
	namespace wsp = WorkspacePack;
	wsp::WorkspaceStore* wss = WorkspacePack::default_workspace_store.get();

	assert_initialized();

	LinAlgPack::Vp_MtV_assert_sizes( y->size(), n_, n_, BLAS_Cpp::no_trans, x.size() );

	update_factorization();

	wsp::Workspace<value_type>  t_ws(wss,y->size());
	VectorSlice                 t(&t_ws[0],t_ws.size());

	t = x;
	
	FortranTypes::f_int info = 0;
	FORTRAN_FUNC_CALL_UL(DPBTRS,dpbtrs)(
		FORTRAN_CHAR_1_ARG_CALL(UB_uplo_ == BLAS_Cpp::upper ? 'U' : 'L')
		, n_, kd_, 1, UB_.col_ptr(1), UB_.max_rows()
		,&t[0], t.size(), &info );
	if( info > 0 )
		assert(0); // Should not happen!
	if( info < 0 ) {
		std::ostringstream omsg;
		omsg
			<< "MatrixSymPosDefBandedChol::update_factorization(): Error, "
			<< "The " << -info << " argument passed to xPBTRF(...) is invalid!";
		throw std::invalid_argument(omsg.str());
	}
	*y = t;
}

// Private member functions

void MatrixSymPosDefBandedChol::assert_initialized() const
{
	if( n_ == 0 )
		throw std::logic_error("MatrixSymPosDefBandedChol::assert_initialized(): Error, "
							   "not initialized!" );
}

void MatrixSymPosDefBandedChol::update_factorization() const
{
	namespace rcp = MemMngPack;
	using rcp::ref_count_ptr;
	namespace rmp = MemMngPack;

	if( !factor_updated_ ) {
		if(UB_.rows() == 0) {
			// Allocate our own storage for the banded cholesky factor!
			typedef rcp::ref_count_ptr<GenMatrix>                  UB_ptr_t;
			typedef rmp::ReleaseResource_ref_count_ptr<GenMatrix>  UB_rel_ptr_t;
			typedef rcp::ref_count_ptr<UB_rel_ptr_t>               UB_rel_ptr_ptr_t;
			UB_rel_ptr_ptr_t  UB_rel_ptr_ptr = new UB_rel_ptr_t(new GenMatrix);
			UB_rel_ptr_ptr->ptr->resize(kd_+1,n_);
			UB_.bind( (*UB_rel_ptr_ptr->ptr)() );
			UB_release_resource_ptr_ = rcp::rcp_implicit_cast<rmp::ReleaseResource>(UB_rel_ptr_ptr);
		}
		// Update the cholesky factor
		LinAlgOpPack::M_StM( &UB_, scale_, MB_, BLAS_Cpp::no_trans );
		UB_uplo_ = MB_uplo_;
		FortranTypes::f_int info = 0;
		FORTRAN_FUNC_CALL_UL(DPBTRF,dpbtrf)(
			FORTRAN_CHAR_1_ARG_CALL(UB_uplo_ == BLAS_Cpp::upper ? 'U' : 'L')
			, n_, kd_, UB_.col_ptr(1), UB_.max_rows(), &info );
		if( info < 0 ) {
			std::ostringstream omsg;
			omsg
				<< "MatrixSymPosDefBandedChol::update_factorization(): Error, "
				<< "The " << -info << " argument passed to xPBTRF(...) is invalid!";
			throw std::invalid_argument(omsg.str());
		}
		if( info > 0 ) {
			std::ostringstream omsg;
			omsg
				<< "MatrixSymPosDefBandedChol::update_factorization(): Error, "
				<< "The leading minor of order " << info << " passed to xPBTRF(...) is not positive definite!";
			throw std::invalid_argument(omsg.str());
		}
		factor_updated_ = true;
	}
}

} // end namespace ConstrainedOptimizationPack
