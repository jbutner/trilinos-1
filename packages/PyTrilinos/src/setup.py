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

# The first purpose of this script is to provide a PyTrilinos/__init__.py file
# that specifies all of the PyTrilinos modules.  The second purpose is the
# standard setup.py purpose: define the distutils Extension objects and call the
# distutils setup function in order to build the PyTrilinos package.

# Module names: define the names of the modules that could be configured to be a
# part of the PyTrilinos package.  These should be put in dependency order.
modules = [
           "Teuchos",
           #"Thyra",
           "Epetra",
           "TriUtils",
           "EpetraExt",
           "AztecOO",
           "Galeri",
           "Amesos",
           "IFPACK",
           "Komplex",
           "Anasazi",
           "Pliris",
           "ML",
           "NOX",
           "LOCA",
           ]

# System imports
from   distutils.core import *
from   distutils.util import get_platform
from   distutils.sysconfig import get_python_lib
import os
import sys

# Set the python path to find the PyTrilinos utilities
sys.path.append(os.path.join("..", "util"))

# Local imports
from   MakefileVariables   import *
from   PyTrilinosExtension import *
import SharedUtils

# Package name
PyTrilinos = "PyTrilinos"

# Build the __init__.py file
def buildInitFile(filename,depfile,pyTrilinosModules,
                  pyTrilinosVersion,trilinosVersion):
    build = False
    if os.path.isfile(filename):
        if os.path.getmtime(filename) < os.path.getmtime(depfile):
            build = True
    else:
        build = True
    if build:
        print "\ncreating", filename
        print "Enabled modules:"
        for module in pyTrilinosModules:
            print "   ", module
        print
        enabledModules    = str(pyTrilinosModules)
        enabledModulesFmt = enabledModules.replace('[','')
        enabledModulesFmt = enabledModulesFmt.replace(']','')
        enabledModulesFmt = enabledModulesFmt.replace("'",'')
        enabledModulesFmt = enabledModulesFmt.replace(',','\n   ')
        content = """
'''
PyTrilinos: A python interface to selected Trilinos packages.  This
  installation of PyTrilinos was built with the following python-wrapped
  packages enabled:

    %s
'''

# We need symbol names to be recognized across PyTrilinos extension modules.  On
# some systems, this requires that the dynamic library opener be given certain
# flags: runtime load now and runtime load global.  This has to be set before
# any PyTrilinos extension modules are loaded.  Unfortunately, the python module
# we want that contains the named values of these flags is called "dl" on some
# systems, "DLFCN" on others, and does not even exist on some.  The following
# logic loads the module into name "dl" if it exists and extracts the needed
# flags.  If the module we need does not exist, we use our best guess at what
# the flags values are.  After all this, the sys module setdlopenflags()
# function is called.
dl = None
try:
    import dl
except (SystemError, ImportError):
    try:
        import DLFCN as dl
    except (SystemError, ImportError):
        pass
if dl:
    dlopenflags = dl.RTLD_NOW | dl.RTLD_GLOBAL
    del dl
else:
    dlopenflags = 258
import sys
sys.setdlopenflags(dlopenflags)

# Set the __all__ special variable, a list of strings which define all of the
# PyTrilinos public names, namely the Trilinos packages that are enabled.
__all__ = %s

# Versioning.  Set the __version__ special variable and define a version()
# function.
__version__ = '%s'
def version():
    return 'Trilinos version: %s\\nPyTrilinos version: ' + __version__

#
# In order to handle the nested namespaces properly, the import of NOX and LOCA
# and their sub-modules needs to be handled at this level.  If, for example, we
# tried to put the import statement in NOX/__init__.py, we can't fill the
# PyTrilinos.NOX namespace completely before we need to use it.
if ('NOX' in __all__):
    import NOX
    if (NOX.Have_Epetra):
        import NOX.Epetra
if ('LOCA' in __all__):
    import LOCA
    if (NOX.Have_Epetra):
        import LOCA.Epetra

#
# Import utility class
from PropertyBase import PropertyBase
#
# Import the typed tuple factory and classes
from typed_tuple import *
#
# Import the typed dict factory
from typed_dict import *

""" % (enabledModulesFmt, enabledModules, pyTrilinosVersion, trilinosVersion)
        open(filename,"w").write(content)

# Main script
if __name__ == "__main__":

    # Initialization
    initFileName = os.path.join(PyTrilinos, "__init__.py")

    # Command-line arguments
    command = sys.argv[1]
    if command not in ("build","install","clean","uninstall"):
        raise RuntimeError, "Command '%s' not supported" % command

    # Extract the Makefile variables
    print "\nExtracting Makefile variables ...",
    sys.stdout.flush()
    makeMacros = processMakefile("Makefile")
    print "done"

    # Determine what packages are enabled
    enabledModules = [module for module in modules
                      if makeMacros["ENABLE_" + module.upper()] == "true"]

    # Determine the Trilinos version
    trilinosVersion = processMakefile(os.path.join("..","..","..","Makefile"))["PACKAGE_VERSION"]

    # Determine the installation information
    srcdir            = makeMacros["srcdir"]
    pyTrilinosVersion = makeMacros["PACKAGE_VERSION"]
    pySiteDir         = get_python_lib(1)
    install           = makeMacros["INSTALL"]
    mkdir             = makeMacros["mkdir_p"]
    pyTrilinosDir     = os.path.join(pySiteDir, PyTrilinos)

    #########################################################
    # Build/clean/uninstall the PyTrilinos __init__.py file #
    #########################################################

    # Build command
    if command in ("build", "install"):
        # Build the init file
        buildInitFile(initFileName, "Makefile", enabledModules,
                      pyTrilinosVersion, trilinosVersion)

    # Clean command
    if command == "clean":
        # Remove the __init__.py file
        if os.path.isfile(initFileName):
            print "removing", initFileName
            os.remove(initFileName)

    # Uninstall command
    if command == "uninstall":
        # Remove the PyTrilinos package
        print "\nUninstalling PyTrilinos package"
        if os.path.isdir(pyTrilinosDir):
            SharedUtils.runCommand("rm -rf " + pyTrilinosDir)
        else:
            print "nothing needs to be done for", pyTrilinosDir
        # "uninstall" is not a distutils command, so end here
        sys.exit()

    ###################
    # UserArray patch #
    ###################

    # NumPy version 0.9.8 has a bug in UserArray.  If the user is using this
    # version of NumPy, we need to include our patched version of UserArray.py
    # in the distribution
    if command in ("build", "install"):
        from numpy import __version__ as numpy_version
        if numpy_version == "0.9.8":
            userArraySrc = os.path.join(srcdir,"UserArray.patch")
            userArrayTrg = os.path.join(PyTrilinos,"UserArrayFix.py")
            if SharedUtils.needsToBeBuilt(userArrayTrg, [userArraySrc]):
                print "copying %s -> %s" % (userArraySrc, userArrayTrg)
                open(userArrayTrg,"w").write(open(userArraySrc,"r").read())

    #############################################################
    # Use distutils to build/clean/etc... the extension modules #
    #############################################################

    # Build the list of extension modules
    ext_modules = [ ]
    for module in enabledModules:
        ext_modules.extend(makePyTrilinosExtensions(module))

    # Build the list of package names
    extModNames = [mod.name for mod in ext_modules]
    packages = [PyTrilinos]
    for extModName in extModNames:
        if extModName.endswith(".___init__"):
            packages.append(extModName[:-10])

    # Build the list of scripts
    scripts = [ ]
    if makeMacros["ENABLE_TEUCHOS"] == "true":
        scripts.append(os.path.join(srcdir, "scripts", "ParamConvert.py"))

    # Call the distutils setup function.  This defines the PyTrilinos package to
    # distutils and distutils takes over from here.
    setup(name         = PyTrilinos,
          version      = pyTrilinosVersion,
          description  = "Python interface to Trilinos",
          author       = "Bill Spotz",
          author_email = "wfspotz@sandia.gov",
          url          = "http://trilinos.sandia.gov/packages/pytrilinos",
          download_url = "http://trilinos.sandia.gov/downloads/trilinos-8.0.html",
          license      = "GNU Lesser General Public License",
          packages     = packages,
          ext_modules  = ext_modules,
          scripts      = scripts
          )