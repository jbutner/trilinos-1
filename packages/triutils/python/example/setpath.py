# @HEADER
# ************************************************************************
#
#            PyTrilinos.Triutils: Python Interface to Triutils
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
# Questions? Contact Michael A. Heroux (maherou@sandia.gov)
#
# ************************************************************************
# @HEADER

"""
Set the python search path to include the library build directory created by the
python distutils module.
"""

# System includes
import os
import sys
from   distutils.util import get_platform

# Obtain the current directory name
myDir,myName = os.path.split(__file__)

# Construct the the build library directory name
libDir = "lib.%s-%s" % (get_platform(), sys.version[0:3])

# List the packages that should be added to the search path
packages = ["triutils", "epetra"]

# Add the python build directory for each package to the search path
for package in packages:
    fullPath = os.path.normpath(os.path.join(myDir, "..", "..", "..", package,
                                             "python", "src", "build", libDir,
                                             "PyTrilinos"))
    sys.path.insert(1,fullPath)
