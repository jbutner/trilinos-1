#! /usr/bin/env python

# @HEADER
# ************************************************************************
#
#                PyTrilinos: Python Interface to Trilinos
#                   Copyright (2005) Sandia Corporation
#
# Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
# license for use of this work by or on behalf of the U.S. Government.
#
# This library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of the
# License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
# USA
# Questions? Contact Bill Spotz (wfspotz@sandia.gov)
#
# ************************************************************************
# @HEADER

from optparse import *

parser = OptionParser()
parser.add_option("-t", "--testharness", action="store_true",
                  dest="testharness", default=False,
                  help="test local build modules; prevent loading system-installed modules")
parser.add_option("-v", "--verbosity", type="int", dest="verbosity", default=2,
                  help="set the verbosity level [default 2]")
options,args = parser.parse_args()
if options.testharness:
   import setpath
   import Epetra
   import Galeri
   import Anasazi
else:
   try:
      import setpath
      import Epetra
      import Galeri
      import Anasazi
   except ImportError:
      from PyTrilinos import Epetra
      from PyTrilinos import Galeri
      from PyTrilinos import Anasazi
      print "Using system-installed Epetra, Galeri, Anasazi"

import numpy

################################################################################

def main():
   comm = Epetra.PyComm()
   nx = 10
   ny = nx
   galeriList = {"n"  : nx * ny,  # for Linear map
                 "nx" : nx,       # for Laplace2D, which requires nx
                 "ny" : ny        # and ny
                 }
   map    = Galeri.CreateMap("Linear", comm, galeriList)
   name   = "Laplace2D"
   matrix = Galeri.CreateCrsMatrix("Laplace2D", map, galeriList)
   print "Problem name: %s\n" % name

   printer = Anasazi.BasicOutputManager()

   nev         = 4
   blockSize   = 5
   numBlocks   = 8
   maxRestarts = 100
   tol         = 1.0e-8
   ivec = Epetra.MultiVector(map, blockSize)
   ivec.Random()

   # Create the eigenproblem
   myProblem = Anasazi.BasicEigenproblem(matrix, ivec)

   # Inform the eigenproblem that matrix is symmetric
   myProblem.setHermitian(True)

   # Set the number of eigenvalues requested
   myProblem.setNEV(nev)

   # All done defining problem
   if not myProblem.setProblem():
      print "Anasazi.BasicEigenProblem.setProblem() returned an error"
      return -1

   # Define the parameter list
   myPL = {"Which"                 : "LM",
           "Block Size"            : blockSize,
           "Num Blocks"            : numBlocks,
           "Maximum Restarts"      : maxRestarts,
           "Convergence Tolerance" : tol }

   # Create the solver manager
   mySolverMgr = Anasazi.BlockDavidsonSolMgr(myProblem, myPL)

   # Solve the problem
   returnCode = mySolverMgr.solve()

   # Get the eigenvalues and eigenvectors
   sol = myProblem.getSolution()
   evals = sol.Evals()
   assert(isinstance(evals, numpy.ndarray))
   evecs = sol.Evecs()
   assert(isinstance(evecs, Epetra.MultiVector))
   index = sol.index
   assert(isinstance(index, Anasazi.VectorInt))

   # Check the eigensolutions
   lhs = Epetra.MultiVector(map, sol.numVecs)
   matrix.Apply(evecs, lhs)
   print "Eig#  Value     Error"
   print "----  --------  ----------"
   failures = 0
   for i in range(nev):
      # Verify that the eigensolution is non-complex
      assert(index[i] == 0)
      rhs   = evecs[i] * evals[i].real
      diff  = lhs[i] - rhs
      error = diff.Norm2()[0]
      print "%4d%10.4f  %10.4e" % (i, evals[i].real, error)
      if (error > 1.0e-7):
         failures += 1

   return failures

################################################################################

if __name__ == "__main__":

   failures = main()
   print
   if failures == 0:
      print "End result: TEST PASSED"
   else:
      print "Eigensolution errors are too large"
      print "End result: TEST FAILED"
