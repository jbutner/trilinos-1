// ////////////////////////////////////////////////////////////////
// InitFinDiffReducedHessian_StepSetOptions.cpp

// disable VC 5.0 warnings about debugger limitations
#pragma warning(disable : 4786)	

#include <assert.h>
#include <math.h>

#include "../../include/std/InitFinDiffReducedHessian_StepSetOptions.h"
#include "Misc/include/StringToBool.h"

// Define the options
namespace {

	const int local_num_options = 4;

	enum local_EOptions {
		INITIALIZATION_METHOD,
		MAX_COND,
		MIN_DIAG,
		STEP_SCALE
	};

	const char* local_SOptions[local_num_options]	= {
		"initialization_method",
		"max_cond",
		"min_diag",
		"step_scale"
	};

}

namespace ReducedSpaceSQPPack {

InitFinDiffReducedHessian_StepSetOptions::InitFinDiffReducedHessian_StepSetOptions(
			  InitFinDiffReducedHessian_Step* target
			, const char opt_grp_name[] )
	:	OptionsFromStreamPack::SetOptionsFromStreamNode(
			  opt_grp_name, local_num_options, local_SOptions )
		, OptionsFromStreamPack::SetOptionsToTargetBase<
			InitFinDiffReducedHessian_Step >( target )
{}

void InitFinDiffReducedHessian_StepSetOptions::set_option(
	int option_num, const std::string& option_value )
{
	typedef InitFinDiffReducedHessian_Step target_t;
	switch( (local_EOptions)option_num ) {
	    case INITIALIZATION_METHOD:
		{
			const std::string &option = option_value.c_str();
			if( option == "SCALE_IDENTITY" )
				target().initialization_method( target_t::SCALE_IDENTITY );
			else if( option == "SCALE_DIAGONAL" )
				target().initialization_method( target_t::SCALE_DIAGONAL );
			else if( option == "SCALE_DIAGONAL_ABS" )
				target().initialization_method( target_t::SCALE_DIAGONAL_ABS );
			else
				throw std::invalid_argument( "Error, incorrect value for "
					"\"initialization_method\"." );
			break;
		}
	    case MAX_COND:
			target().max_cond(::fabs(::atof(option_value.c_str())));
			break;
		case MIN_DIAG:
			target().min_diag(::abs(::atoi(option_value.c_str())));
			break;
		case STEP_SCALE:
			target().step_scale(::fabs(::atof(option_value.c_str())));
			break;
		default:
			assert(0);	// Local error only?
	}
}

}	// end namespace ReducedSpaceSQPPack 