# ************************************************************************
# 
#                Pliris: Parallel Dense Solver Package
#                 Copyright (2004) Sandia Corporation
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

#--------------------------------------
# Basic definitions for the makefile
#
#---------------------------------------

#---------------------------------------
# Platform name
#---------------------------------------
  ARCH=CPLANT
#---------------------------------------
# Directories
#---------------------------------------
  TOPdir = $(HOME)/LU_MPI
  SRCdir = $(TOPdir)/src
  MPI_loc = /usr/local/mpich-1.2.2.2
  INCdir = $(MPI_loc)/include
  LIBdir = $(MPI_loc)/lib
  MATHLIB = /usr/local/intel/mkl/LIB
  LIBpaths = -L$(MATHLIB) -L$(LIBdir)
#---------------------------------------
# MPI Definitions
#---------------------------------------
 MPI_INCLUDES = -I/apps/mpich/include
 MPI_LIB_PATH = -L/apps//mpich/lib

# Home directory
#---------------------------------------
 

#---------------------------------------
# Compilers and their flags
#---------------------------------------

  CC = icc
  F77 = pgf90
  CCFLAGS =        
  FFLAGS = -C    
  AR = ar  
  RANLIB = ranlib

#---------------------------------------
# Libraries needed Math, MPI, Basic
#  Linear Algebra Routines
#---------------------------------------
  
  LIBS = -lm  -lmpich  -L/opt/gm/lib -lgm -lpthread
  BLASlib =  -L/opt/intel/mkl60/lib/32  -lmkl_p4 -lguide -lpthread
#   -lmkl32_p3 -lpthread


#---------------------------------------
#---------------------------------------
 

