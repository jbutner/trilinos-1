// ***********************************************************************
// 
//      Tifpack: Tempated Object-Oriented Algebraic Preconditioner Package
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


/*! \file Tifpack_UnitTestILUT.cpp

\brief Tifpack Unit test for the ILUT template.
*/


#include <Teuchos_ConfigDefs.hpp>
#include <Tifpack_ConfigDefs.hpp>
#include <Teuchos_UnitTestHarness.hpp>
#include <Tifpack_Version.hpp>
#include <iostream>

#include <Tifpack_UnitTestHelpers.hpp>
#include <Tifpack_Factory.hpp>

namespace {
using Tpetra::global_size_t;
typedef tif_utest::Node Node;

//this macro declares the unit-test-class:
TEUCHOS_UNIT_TEST_TEMPLATE_3_DECL(TifpackFactory, Test0, Scalar, LocalOrdinal, GlobalOrdinal)
{
//we are now in a class method declared by the above macro, and
//that method has these input arguments:
//Teuchos::FancyOStream& out, bool& success

  std::string version = Tifpack::Version();
  out << "Tifpack::Version(): " << version << std::endl;

  global_size_t num_rows_per_proc = 5;

  const Teuchos::RCP<const Tpetra::Map<LocalOrdinal,GlobalOrdinal,Node> > rowmap = tif_utest::create_tpetra_map<LocalOrdinal,GlobalOrdinal,Node>(num_rows_per_proc);

  Teuchos::RCP<const Tpetra::CrsMatrix<Scalar,LocalOrdinal,GlobalOrdinal,Node> > crsmatrix = tif_utest::create_test_matrix<Scalar,LocalOrdinal,GlobalOrdinal,Node>(rowmap);

  Tifpack::Factory factory;

  Teuchos::RCP<Tifpack::Preconditioner<Scalar,LocalOrdinal,GlobalOrdinal,Node> > prec_ilut = factory.create("ILUT", crsmatrix);
  TEUCHOS_TEST_EQUALITY(prec_ilut != Teuchos::null, true, out, success);

  Teuchos::RCP<Tifpack::Preconditioner<Scalar,LocalOrdinal,GlobalOrdinal,Node> > prec_point = factory.create("RELAXATION", crsmatrix);
  TEUCHOS_TEST_EQUALITY(prec_point != Teuchos::null, true, out, success);

  Teuchos::RCP<Tifpack::Preconditioner<Scalar,LocalOrdinal,GlobalOrdinal,Node> > prec_cheby = factory.create("CHEBYSHEV", crsmatrix);
  TEUCHOS_TEST_EQUALITY(prec_cheby != Teuchos::null, true, out, success);
}

#define UNIT_TEST_GROUP_SCALAR_ORDINAL(Scalar,LocalOrdinal,GlobalOrdinal) \
  TEUCHOS_UNIT_TEST_TEMPLATE_3_INSTANT( TifpackFactory, Test0, Scalar, LocalOrdinal,GlobalOrdinal)

UNIT_TEST_GROUP_SCALAR_ORDINAL(double, int, int)
UNIT_TEST_GROUP_SCALAR_ORDINAL(float, short, int)

}//namespace <anonymous>
