/*
// @HEADER
// ***********************************************************************
// 
//              Meros: Segregated Preconditioning Package
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
*/

#ifndef SIMPLESCHURFACTORY_H
#define SIMPLESCHURFACTORY_H

#include "TSFDefs.h"
#include "TSFSmartPtr.h"
#include "TSFLinearOperator.h"
#include "TSFLinearSolver.h"
#include "TSFOperatorSourceBase.h"
#include "RightBlockNSOperatorSource.h"
#include "SimpleOperatorSource.h"
#include "SchurFactoryBase.h"
#include "SchurFactory.h"
#include "SimpleOperatorSource.h"
#include "TSFArray.h"
#include <iostream>
#include <string>

namespace Meros
{

  using namespace TSF;
  using std::string;
  using std::ostream;


  /** \ingroup Preconditioner
   * SimpleSchurFactory
   *
   */

  class SimpleSchurFactory : public SchurFactoryBase
    {
    public:
      /** ctor */
      SimpleSchurFactory(TSFLinearSolver& schurSolver);

      /** virtual destructor */
      virtual ~SimpleSchurFactory(){;}

      /** get Schur complement inverse approximation */
      virtual TSFLinearOperator getSchurInvApprox(const TSFOperatorSource& diagfac) const;

      /** write to a string */
      virtual string toString() const ;

    private:
      TSFLinearSolver schurSolver_;
    };

  /** \relates DiagSchurFactory
   * write to an output stream
   */
  ostream& operator<<(ostream& os, const SimpleSchurFactory& x);

  /** \relates DiagSchurFactory
   * write to a string
   */
  string toString(const SimpleSchurFactory& x);
}


#endif
