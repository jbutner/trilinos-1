// ///////////////////////////////////////////////////////////
// cast_iq.hpp
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

#ifndef GIP_CAST_IQ_H
#define GIP_CAST_IQ_H

#include <stdexcept>
#include <typeinfo>

#include "AlgorithmState.hpp"
#include "IterQuantityAccess.hpp"

namespace GeneralIterationPack {

///
/** Lookup an iteration quantity by name and cast it
  * to an <tt>IterQuantityAccess<T></tt> of the given type \c T.
  * If the iteration quantity of that name does not
  * exist then a <tt>AlgorithmState::DoesNotExist</tt> exception
  * will be thrown.  If the type of the iteration quantity
  * is not of the type <tt>IterQuantityAcess<T></tt> (as determined
  * by <tt>dynamic_cast<T></tt>) then the exception \c InvalidTypeCastException:
  * will be thrown with a helpful error message.
  *
  * Note that using this function always cost <tt>O(s.num_iter_quantities())</tt>
  * everytime it is called.  Therefore, the developer should consider using the
  * class \c CastIQMember instead if it is appropriate.
  */
template<class T>
IterQuantityAccess<T>& cast_iq(
	AlgorithmState& state, const std::string& iq_name );

///
template<class T>
const IterQuantityAccess<T>& cast_iq(
	const AlgorithmState& state, const std::string& iq_name );

///
/** Lookup an iteration quantity using its id and cast it
 * to an <tt>IterQuantityAccess<T></tt> of the given type \c T.
 *
 * @param  state  [in] 
 *
 * If the iteration quantity with that id does not
 * exist then a <tt>AlgorithmState::DoesNotExist</tt> exception
 * will be thrown.  If the type of the iteration quantity
 * is not of the type <tt>IterQuantityAcess<T></tt> (as determined
 * by <tt>dynamic_cast<T></tt>) then the exception \c InvalidTypeCastException:
 * will be thrown with a helpful error message.
 *
 * Note that using this function always cost just <tt>O(1)</tt> everytime
 * it is called.
 */
template<class T>
IterQuantityAccess<T>& cast_iq(
	AlgorithmState& state, const AlgorithmState::iq_id_type iq_id, const std::string& iq_name );

///
template<class T>
const IterQuantityAccess<T>& cast_iq(
	const AlgorithmState& state, const AlgorithmState::iq_id_type iq_id, const std::string& iq_name );

// Helper function

void imp_cast_iq_throw_error(
	const std::string&                 iq_name
	,const std::string&                iq_is_type_name
	,const std::string&                iq_want_type_name
	);

void imp_cast_iq_throw_error(
	const AlgorithmState::iq_id_type   iq_id
	,const std::string&                iq_name
	,const std::string&                iq_is_type_name
	,const std::string&                iq_want_type_name
	);

// ///////////////////
// Inline definitions

template<class T>
//inline
IterQuantityAccess<T>& cast_iq(
	AlgorithmState& state, const std::string& iq_name )
{
	IterQuantity
	 	&iq = state.iter_quant( iq_name );
	IterQuantityAccess<T>
		*p = dynamic_cast<IterQuantityAccess<T>*>( &iq );
			// will throw exception if iq_name does not exist
	if( !p )
		imp_cast_iq_throw_error( iq_name, typeid(iq).name(), typeid(T).name() );
	return *p;	
}

template<class T>
//inline
const IterQuantityAccess<T>& cast_iq(
	const AlgorithmState& state, const std::string& iq_name )
{
	const IterQuantity
	 	&iq = state.iter_quant( iq_name );
	const IterQuantityAccess<T>
		*p = dynamic_cast<const IterQuantityAccess<T>*>( &iq );
			// will throw exception if iq_name does not exist
	if( !p )
		imp_cast_iq_throw_error( iq_name, typeid(iq).name(), typeid(T).name() );
	return *p;	
}

template<class T>
//inline
IterQuantityAccess<T>& cast_iq(
	AlgorithmState& state, const AlgorithmState::iq_id_type iq_id, const std::string& iq_name )
{
	IterQuantity
	 	&iq = state.iter_quant( iq_id );
	IterQuantityAccess<T>
		*p = dynamic_cast<IterQuantityAccess<T>*>( &iq );
			// will throw exception if iq_name does not exist
	if( !p )
		imp_cast_iq_throw_error( iq_id, iq_name, typeid(iq).name(), typeid(T).name() );
	return *p;	
}

template<class T>
//inline
const IterQuantityAccess<T>& cast_iq(
	const AlgorithmState& state, const AlgorithmState::iq_id_type iq_id, const std::string& iq_name )
{
	IterQuantity
	 	&iq = state.iter_quant( iq_id );
	const IterQuantityAccess<T>
		*p = dynamic_cast<const IterQuantityAccess<T>*>( &iq );
			// will throw exception if iq_name does not exist
	if( !p )
		imp_cast_iq_throw_error( iq_id, iq_name, typeid(iq).name(), typeid(T).name() );
	return *p;
}

}	// namespace GeneralIterationPack

#endif	// GIP_CAST_IQ_H
