// @HEADER
// ***********************************************************************
// 
//                    Teuchos: Common Tools Package
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

#include "Teuchos_ArrayRCP.hpp"
#include "Teuchos_GlobalMPISession.hpp"
#include "Teuchos_CommandLineProcessor.hpp"
#include "Teuchos_VerboseObject.hpp"
#include "Teuchos_StandardCatchMacros.hpp"
#include "Teuchos_Version.hpp"
#include "Teuchos_Assert.hpp"
#include "Teuchos_TestingHelpers.hpp"


//
// Define local macros to make defining tests easier for this particular test
// code.
//
// Note, macros with these types of names should only exist in a *.cpp file
// after all #includes are done!
//


#define TEST_EQUALITY_CONST( v1, v2 ) \
  TEUCHOS_TEST_EQUALITY_CONST( v1, v2, out, success )

#define TEST_EQUALITY( v1, v2 ) \
  TEUCHOS_TEST_EQUALITY( v1, v2, out, success )

#define TEST_ITER_EQUALITY( iter1, iter2 ) \
  TEUCHOS_TEST_ITER_EQUALITY( iter1, iter2, out, success )

#define TEST_ARRAY_ELE_EQUALITY( a, i, val ) \
   TEUCHOS_TEST_ARRAY_ELE_EQUALITY( a, i, val, false, out, local_success )

#define TEST_COMPARE( v1, comp, v2 ) \
  TEUCHOS_TEST_COMPARE( v1, comp, v2, out, success )

#define TEST_COMPARE_ARRAYS( a1, a2 ) \
  { \
    const bool result = compareArrays(a1,#a1,a2,#a2,out); \
    if (!result) success = false; \
  }

#define TEST_THROW( code, ExceptType  ) \
  TEUCHOS_TEST_THROW( code, ExceptType, out, success  )

#define TEST_NOTHROW( code  ) \
  TEUCHOS_TEST_NOTHROW( code, out, success  )


// Temporarily uncomment any or all of these macros to see compilation
// failures for code that is rightfully not supposed to compile (which is a
// wonderful thing)! The fact that this code does not compile show that the
// design of the Teuchos::ArrayRCP class supports full support of
// const projection in all of its forms when dealing with arrays of objects.
//#define SHOW_COMPILE_FAILURE_1
//#define SHOW_COMPILE_FAILURE_2
//#define SHOW_COMPILE_FAILURE_3


//
// Iterator testing function
//

template<class T>
bool test_ArrayRCP_iterators(
  const Teuchos::ArrayRCP<T> &ptr,
  Teuchos::FancyOStream &out
  )
{

  using Teuchos::ArrayRCP;
  using Teuchos::null;
  using Teuchos::arcp;
  using Teuchos::arcp_reinterpret_cast;

  bool success = true;

  out
    << "\n***"
    << "\n*** Testing iterators and accessors for ptr = " << ptr
    << "\n***\n";

  Teuchos::OSTab tab(out);

  const int size = ptr.size();

  // Pointer ++
 
  {
    out << "\nChecking ++itr and < ...\n";
    ArrayRCP<T> itr = ptr;
    for( int i = 0; itr < ptr+size; ++i, ++itr )
      TEST_FOR_EXCEPT( !(*itr == ptr[i]) );
  }
 
  {
    out << "\nChecking itr++ and <= ...\n";
    ArrayRCP<T> itr = ptr;
    for( int i = 0; itr <= ptr+size-1; ++i, itr++ )
      TEST_FOR_EXCEPT( !(*itr == ptr[i]) );
  }
 
  {
    out << "\nChecking itr+=1 and != ...\n";
    ArrayRCP<T> itr = ptr;
    for( int i = 0; itr != ptr+size; ++i, itr+=1 )
      TEST_FOR_EXCEPT( !(*itr == ptr[i]) );
  }
 
  {
    out << "\nChecking itr=itr+1 and == ...\n";
    ArrayRCP<T> itr = ptr;
    for( int i = 0; !( itr == ptr+size ); ++i, itr=itr+1 )
      TEST_FOR_EXCEPT( !(*itr == ptr[i]) );
  }

  // Pointer --
 
  {
    out << "\nChecking --itr and >= ...\n";
    ArrayRCP<T> itr = ptr+size-1;
    for( int i = size-1; itr >= ptr; --i, --itr )
      TEST_FOR_EXCEPT( !(*itr == ptr[i]) );
  }
 
  {
    out << "\nChecking itr-- and > ...\n";
    ArrayRCP<T> itr = ptr+size-1;
    for( int i = size-1; itr+1 > ptr; i--, itr-- )
      TEST_FOR_EXCEPT( !(*itr == ptr[i]) );
  }
 
  {
    out << "\nChecking itr-=1 and != ...\n";
    ArrayRCP<T> itr = ptr+size-1;
    for( int i = size-1; itr+1 != ptr; i--, itr-=1 )
      TEST_FOR_EXCEPT( !(*itr == ptr[i]) );
  }
 
  {
    out << "\nChecking itr=itr-1 and == ...\n";
    ArrayRCP<T> itr = ptr+size-1;
    for( int i = size-1; !( itr+1 == ptr ); i--, itr=itr-1 )
      TEST_FOR_EXCEPT( !(*itr == ptr[i]) );
  }

  // Iterator - Iterator
 
  {
    out << "\nChecking ptr.end() - ptr.begin() == ptr.size() ...\n";
    TEUCHOS_ASSERT_EQUALITY( ptr.end() - ptr.begin(), ptr.size() );
  }

  // Iterator ++
 
  {
    out << "\nChecking iterator ++itr and < ...\n";
    typename ArrayRCP<T>::const_iterator itr = ptr.begin();
    for( int i = 0; itr < ptr.end(); ++i, ++itr )
      TEST_FOR_EXCEPT( !(*itr == ptr[i]) );
  }
 
  {
    out << "\nChecking iterator itr++ and <= ...\n";
    typename ArrayRCP<T>::const_iterator itr = ptr.begin();
    for( int i = 0; itr <= ptr.end()-1; ++i, itr++ )
      TEST_FOR_EXCEPT( !(*itr == ptr[i]) );
  }
 
  {
    out << "\nChecking iterator itr+=1 and != ...\n";
    typename ArrayRCP<T>::const_iterator itr = ptr.begin();
    for( int i = 0; itr != ptr.end(); ++i, itr+=1 )
      TEST_FOR_EXCEPT( !(*itr == ptr[i]) );
  }
 
  {
    out << "\nChecking iterator itr=itr+1 and == ...\n";
    typename ArrayRCP<T>::const_iterator itr = ptr.begin();
    for( int i = 0; !( itr == ptr.end() ); ++i, itr=itr+1 )
      TEST_FOR_EXCEPT( !(*itr == ptr[i]) );
  }

  // Iterator --
 
  {
    out << "\nChecking iterator --itr and >= ...\n";
    typename ArrayRCP<T>::const_iterator itr = ptr.begin()+size-1;
    for( int i = size-1; itr >= ptr.begin(); --i, --itr )
      TEST_FOR_EXCEPT( !(*itr == ptr[i]) );
  }
 
  {
    out << "\nChecking iterator itr-- and > ...\n";
    typename ArrayRCP<T>::const_iterator itr = ptr.begin()+size-1;
    for( int i = size-1; itr+1 > ptr.begin(); i--, itr-- )
      TEST_FOR_EXCEPT( !(*itr == ptr[i]) );
  }
 
  {
    out << "\nChecking iterator itr-=1 and != ...\n";
    typename ArrayRCP<T>::const_iterator itr = ptr.begin()+size-1;
    for( int i = size-1; itr+1 != ptr.begin(); i--, itr-=1 )
      TEST_FOR_EXCEPT( !(*itr == ptr[i]) );
  }
 
  {
    out << "\nChecking iterator itr=itr-1 and == ...\n";
    typename ArrayRCP<T>::const_iterator itr = ptr.begin()+size-1;
    for( int i = size-1; !( itr+1 == ptr.begin() ); i--, itr=itr-1 )
      TEST_FOR_EXCEPT( !(*itr == ptr[i]) );
  }

  return success;

}


//
// Main testing function for a specific ArrayRCP
//


template<class T>
bool test_ArrayRCP(
  const Teuchos::ArrayRCP<T> &ptr,
  Teuchos::FancyOStream &out
  )
{

  using Teuchos::ArrayView;

  bool success = true, result;
 
  out
    << "\n***"
    << "\n*** Testing ptr = " << ptr
    << "\n***\n";

  Teuchos::OSTab tab(out);

  const int n = ptr.size();
 
  {
    out << "\nInitializing data ...\n";
    for( int i = 0; i < n; ++i )
      ptr[i] = i;
  }

  TEST_FOR_EXCEPT( !(&*ptr == ptr.get()) );

  result = test_ArrayRCP_iterators(ptr,out);
  if (!result) success = false;


  //
  out << "\nG) Test views ...\n";
  //

  {
    out << "\nTest full non-const subview ...\n";
    const ArrayView<T> av2 = ptr(0,n);
    TEST_COMPARE_ARRAYS( av2, ptr );
  }

  {
    out << "\nTest full shorthand non-const subview ...\n";
    const ArrayView<T> av2 = ptr();
    TEST_COMPARE_ARRAYS( av2, ptr );
  }

  {
    out << "\nTest full const subview ...\n";
    const ArrayView<const T> cav2 = ptr.getConst()(0,n);
    TEST_COMPARE_ARRAYS( cav2, ptr );
  }

  {
    out << "\nTest full non-const to const subview ...\n";
    const ArrayView<const T> cav2 = ptr(0,n);
    TEST_COMPARE_ARRAYS( cav2, ptr );
  }

  {
    out << "\nTest full short-hand const subview ...\n";
    const ArrayView<const T> cav2 = ptr.getConst()();
    TEST_COMPARE_ARRAYS( cav2, ptr );
  }

  {
    out << "\nTest implicit conversion from ArrayRCP<T> to ArrayView<T> ...\n";
    const ArrayView<T> av2 = ptr;
    TEST_COMPARE_ARRAYS( av2, ptr );
  }

  {
    out << "\nTest implicit conversion from ArrayRCP<const T> to ArrayView<const T> ...\n";
    const ArrayView<const T> av2 = ptr.getConst();
    TEST_COMPARE_ARRAYS( av2, ptr );
  }

  return success;

}


//
// Main driver program
//


int main( int argc, char* argv[] ) {

  using Teuchos::CommandLineProcessor;
  using Teuchos::null;
  using Teuchos::RCP;
  using Teuchos::rcp;
  using Teuchos::ArrayRCP;
  using Teuchos::arcp;
  using Teuchos::arcp_reinterpret_cast;
	
	bool success = true, result;
 
  Teuchos::GlobalMPISession mpiSession(&argc,&argv);
  //const int procRank = Teuchos::GlobalMPISession::getRank();
 
  Teuchos::RCP<Teuchos::FancyOStream>
    out = Teuchos::VerboseObjectBase::getDefaultOStream();
 
	try {

		// Read options from the commandline
    int num_ints = 10;
    int num_doubles = 10;
    CommandLineProcessor clp(false); // Don't throw exceptions
    clp.setOption( "num-ints", &num_ints, "Number of ints to allocate space for" );
    clp.setOption( "num-doubles", &num_doubles, "Number of doubles to allocate space for" );
		CommandLineProcessor::EParseCommandLineReturn parse_return = clp.parse(argc,argv);
		if( parse_return != CommandLineProcessor::PARSE_SUCCESSFUL ) {
			*out << "\nEnd Result: TEST FAILED" << std::endl;
			return parse_return;
		}

    const int sizeOfDouble = sizeof(double);
    const int sizeOfInt = sizeof(int);

    const int total_bytes = num_doubles*sizeOfDouble + num_ints*sizeOfInt;

    *out << std::endl << Teuchos::Teuchos_Version() << std::endl;
 
    *out << "\nTesting basic ArrayRCP functionality ...\n";

    ArrayRCP<char>
      char_ptr1 = arcp<char>(total_bytes);

    *out << "\nchar_ptr1 = " << char_ptr1 << "\n";

    TEST_FOR_EXCEPT( !(char_ptr1.size() == total_bytes) );
    TEST_FOR_EXCEPT( !(char_ptr1.lowerOffset() == 0) );
    TEST_FOR_EXCEPT( !(char_ptr1.upperOffset() == total_bytes-1) );
    TEST_FOR_EXCEPT( !(char_ptr1.count() == 1) );
    result = test_ArrayRCP(char_ptr1,*out);
    if (!result) success = false;

    ArrayRCP<char>
      char_ptr2 = null;

    *out << "\nchar_ptr2 = " << char_ptr2 << "\n";

    TEST_FOR_EXCEPT( !(char_ptr2.size() == 0) );
    TEST_FOR_EXCEPT( !(char_ptr2.get() == NULL) );
    TEST_FOR_EXCEPT( !(char_ptr2.count() == 0) );

    ArrayRCP<char>
      char_ptr2b(char_ptr1); // excplicitly test copy constructor

    *out << "\nchar_ptr2b = " << char_ptr2b << "\n";

    TEST_FOR_EXCEPT( !(char_ptr2b.size() == total_bytes) );
    TEST_FOR_EXCEPT( !(char_ptr2b.lowerOffset() == 0) );
    TEST_FOR_EXCEPT( !(char_ptr2b.upperOffset() == total_bytes-1) );
    TEST_FOR_EXCEPT( !(char_ptr2b.count() == 2) );
    result = test_ArrayRCP(char_ptr2b,*out);
    if (!result) success = false;

    char_ptr2b = null;

    TEST_FOR_EXCEPT( !(char_ptr2b.size() == 0) );
    TEST_FOR_EXCEPT( !(char_ptr2.get() == NULL) );
    TEST_FOR_EXCEPT( !(char_ptr2b.count() == 0) );
    TEST_FOR_EXCEPT( !(char_ptr1.count() == 1) );

    ArrayRCP<char>
      char_ptr3 = char_ptr1.persistingView(total_bytes/2,total_bytes/2);

    TEST_FOR_EXCEPT( !(char_ptr1.count() == 2) );
    TEST_FOR_EXCEPT( !(char_ptr3.count() == 2) );
    TEST_FOR_EXCEPT( !(char_ptr3.lowerOffset() == 0) );
    TEST_FOR_EXCEPT( !(char_ptr3.upperOffset() == total_bytes/2-1) );
    result = test_ArrayRCP(char_ptr3,*out);
    if (!result) success = false;

    *out << "\nchar_ptr3 = " << char_ptr3 << "\n";

    *out << "\nBreak up char_ptr1 into views of double and int data\n";

    int offset = 0;
 
    ArrayRCP<double> double_ptr1 = arcp_reinterpret_cast<double>(
      char_ptr1.persistingView(offset,sizeOfDouble*num_doubles)
      );

    *out << "\ndouble_ptr1 = " << double_ptr1 << "\n";

    TEST_FOR_EXCEPT( !(char_ptr1.count() == 3) );
    TEST_FOR_EXCEPT( !(double_ptr1.count() == 3) );
    TEST_FOR_EXCEPT( !(double_ptr1.size() == num_doubles) );

    result = test_ArrayRCP(double_ptr1,*out);
    if (!result) success = false;
 
    offset += sizeOfDouble*num_doubles;

    ArrayRCP<int> int_ptr1 = arcp_reinterpret_cast<int>(
      char_ptr1.persistingView(offset,sizeOfInt*num_ints)
      );

    *out << "\nint_ptr1 = " << int_ptr1 << "\n";

    TEST_FOR_EXCEPT( !(char_ptr1.count() == 4) );
    TEST_FOR_EXCEPT( !(int_ptr1.count() == 4) );
    TEST_FOR_EXCEPT( !(int_ptr1.size() == num_ints) );

    result = test_ArrayRCP(int_ptr1,*out);
    if (!result) success = false;

    *out << "\nCreating a constant view of double_ptr1\n";
 
    ArrayRCP<const double>
      double_ptr2 = double_ptr1.getConst();

    result = test_ArrayRCP_iterators(double_ptr2,*out);
    if (!result) success = false;

#ifdef SHOW_COMPILE_FAILURE_1
    // This will not compile since this function tries to use operator[] to
    // change data but it can't since it returns a reference to a const
    // double!
    for( int i = 0; i < double_ptr2.size(); ++i ) {
      double_ptr2[i] = 1.0; // Error, you can change the value!
    }
#endif

    *out << "\nCreating an array of RCP objects!\n";

    ArrayRCP<RCP<double> >
      rcp_ptr1 = arcp<RCP<double> >(num_doubles);

    for( int i = 0; i < num_doubles; ++i )
      rcp_ptr1[i] = rcp(new double(i));

    result = test_ArrayRCP_iterators(rcp_ptr1,*out);
    if (!result) success = false;

    *out << "\nCreating a const view of rcp_ptr1\n";

    ArrayRCP<const RCP<double> >
      rcp_ptr2 = rcp_ptr1.getConst();

    result = test_ArrayRCP_iterators(rcp_ptr2,*out);
    if (!result) success = false;

    *out << "\nCreating an ARCP<double*> object doubleptr_ptr1 and dynamically allocation each element\n";

    ArrayRCP<double*>
      doubleptr_ptr1 = arcp<double*>(total_bytes);

    for( int i = 0; i < doubleptr_ptr1.size(); ++i )
      doubleptr_ptr1[i] = new double(i);

    result = test_ArrayRCP_iterators(doubleptr_ptr1,*out);
    if (!result) success = false;

    *out << "\nCreating an ARCP<double*const> view of a doubleptr_ptr1\n";
 
    ArrayRCP<double*const>
      doubleptr_ptr2 = doubleptr_ptr1.getConst();

    result = test_ArrayRCP_iterators(doubleptr_ptr2,*out);
    if (!result) success = false;

#ifdef SHOW_COMPILE_FAILURE_2
    // This will not compile since this function tries to use operator[] to
    // change data but it can't since it returns a reference to a double*const
    // object!
    for( int i = 0; i < doubleptr_ptr2.size(); ++i ) {
      *doubleptr_ptr2[i] = 1.0; // Fine, you can change the value that is being pointed to for this entry!
      doubleptr_ptr2[i] = NULL; // Error, you can't change the pointer entry!
    }
#endif

    *out << "\nCreating an ARCP<const double * const> view of a doubleptr_ptr1\n";
 
    ArrayRCP<const double*const>
      doubleptr_ptr3 = Teuchos::arcp_implicit_cast<const double*const>(doubleptr_ptr1);

    result = test_ArrayRCP_iterators(doubleptr_ptr3,*out);
    if (!result) success = false;

#ifdef SHOW_COMPILE_FAILURE_3
    // This will not compile since this function tries to use operator[] to
    // change data but it can't since it returns a reference to a double*const
    // object!
    for( int i = 0; i < doubleptr_ptr3.size(); ++i ) {
      *doubleptr_ptr3[i] = 1.0; // Error, you can't change the value that is being pointed to!
      doubleptr_ptr3[i] = NULL; // Error, you can't change the pointer either!
    }
#endif

    for( int i = 0; i < doubleptr_ptr1.size(); ++i )
      delete doubleptr_ptr1[i];

    *out << "\nWrapping std::vector<T> objects as ArrayRefCount objects ...\n";

    ArrayRCP<char>
      vchar_ptr1 = arcp(rcp(new std::vector<char>(total_bytes)));

    *out << "\nvchar_ptr1 = " << vchar_ptr1 << "\n";
 
    result = test_ArrayRCP(vchar_ptr1,*out);
    if (!result) success = false;
 
    ArrayRCP<const char> vchar_ptr2 =
      arcp(
        Teuchos::rcp_implicit_cast<const std::vector<char> >(
          Teuchos::get_std_vector(vchar_ptr1)
          )
        );
 
    *out << "\nvchar_ptr2 = " << vchar_ptr2 << "\n";

    result = test_ArrayRCP_iterators(vchar_ptr2,*out);
    if (!result) success = false;

#ifndef __sun
    // RAB: 2006/07/12: The sun compiler declares this call to
    // get_std_vector(...) to be ambiguous (which is nonsense based on
    // everything I know about C++)!
    TEST_FOR_EXCEPT( Teuchos::get_std_vector(vchar_ptr2)->size() != static_cast<size_t>(total_bytes) );
#endif
 
    // ToDo: Fill in the rest of the tests!
 
    *out << "\nAll tests for ArrayRCP seem to check out!\n";
 
	}
  TEUCHOS_STANDARD_CATCH_STATEMENTS(true,std::cerr,success);
 
  if(success)
    *out << "\nEnd Result: TEST PASSED" << std::endl;	
 
  return ( success ? 0 : 1 );
 
}
