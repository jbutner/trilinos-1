// @HEADER
// ***********************************************************************
//
//                Pliris: Parallel Dense Solver Package
//                 Copyright (2004) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.//
// This library is free software; you can redistribute it and/or modify// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the// License, or (at your option) any later version.
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

#ifndef PLIRIS_CONFIGDEFS_H
#define PLIRIS_CONFIGDEFS_H

#ifndef __cplusplus
#define __cplusplus
#endif // ifndef __cplusplus
                                                                                
/* this section undefines all the things autotools defines for us that we wish it didn't. */
                                                                                
#ifdef PACKAGE
#undef PACKAGE
#endif // ifdef PACKAGE
                                                                                
#ifdef PACKAGE_NAME
#undef PACKAGE_NAME
#endif // ifdef PACKAGE_NAME
                                                                                
#ifdef PACKAGE_BUGREPORT
#undef PACKAGE_BUGREPORT
#endif // ifdef PACKAGE_BUGREPORT
                                                                                
#ifdef PACKAGE_STRING
#undef PACKAGE_STRING
#endif // ifdef PACKAGE_STRING
                                                                                
#ifdef PACKAGE_TARNAME
#undef PACKAGE_TARNAME
#endif // ifdef PACKAGE_TARNAME
                                                                                
#ifdef PACKAGE_VERSION
#undef PACKAGE_VERSION
#endif // ifdef PACKAGE_VERSION
                                                                                
#ifdef VERSION
#undef VERSION
#endif // ifdef VERSION
                                                                                
// end of undoing autoconf's work section
                                                                                
#include "Pliris_config.h"

#ifdef HAVE_STRING
#include <string>
#else
#include <string.h>
#endif
                                                                       
using std::string;

#endif // PLIRIS_CONFIGDEFS_H