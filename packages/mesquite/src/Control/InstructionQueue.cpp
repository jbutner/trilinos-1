/* ***************************************************************** 
    MESQUITE -- The Mesh Quality Improvement Toolkit

    Copyright 2004 Sandia Corporation and Argonne National
    Laboratory.  Under the terms of Contract DE-AC04-94AL85000 
    with Sandia Corporation, the U.S. Government retains certain 
    rights in this software.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License 
    (lgpl.txt) along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
    diachin2@llnl.gov, djmelan@sandia.gov, mbrewer@sandia.gov, 
    pknupp@sandia.gov, tleurent@mcs.anl.gov, tmunson@mcs.anl.gov      
   
  ***************************************************************** */
// -*- Mode : c++; tab-width: 3; c-tab-always-indent: t; indent-tabs-mode: nil; c-basic-offset: 3 -*-

/*! \file InstructionQueue.cpp

Member functions of the Mesquite::InstructionQueue class

  \author Thomas Leurent
  \date   2002-05-01
 */

#ifdef USE_STD_INCLUDES
#include <string>
#include <iostream>
#else
#include <string.h>
#include <iostream.h>
#endif

#include "InstructionQueue.hpp"
#include "MeshSet.hpp"
#ifdef ENABLE_INTERRUPT
#include "MesquiteInterrupt.hpp"
#endif

using namespace Mesquite;

MSQ_USE(advance);
MSQ_USE(string);
MSQ_USE(cout);

InstructionQueue::InstructionQueue() :
  autoQualAssess(true),
  nbPreConditionners(0),
  isMasterSet(false),
  masterInstrIndex(0),
  globalPatch(0)
{
}

#undef __FUNC__
#define __FUNC__ "InstructionQueue::add_preconditioner"
/*! \fn InstructionQueue::add_preconditioner(QualityImprover* instr, MsqError &err)
    \brief adds a QualityImprover at the end of the instruction list

    This function cannot be used once the set_master_quality_improver()
    function has been used.
    
    See also insert_preconditioner().
  */
void InstructionQueue::add_preconditioner(QualityImprover* instr,
                                        MsqError &err)
{
  if (isMasterSet) {
    err.set_msg("cannot add preconditionners once the master QualityImprover has been set.");
    return;
  }
  
  instructions.push_back(instr);
  nbPreConditionners++;
}


#undef __FUNC__
#define __FUNC__ "InstructionQueue::remove_preconditioner"
/*! \fn InstructionQueue::remove_preconditioner(size_t index, MsqError &err)
    \brief removes a QualityImprover* from the instruction queue

    \param index is 0-based. An error is set if the index does not correspond
           to a valid element in the queue.
*/
void InstructionQueue::remove_preconditioner(size_t index, MsqError &err)
{
  // checks index is valid
  if ( isMasterSet && index == masterInstrIndex ) {
    err.set_msg("cannot remove master QualityImprover.");
    return;
  } else if (index >= instructions.size() ) {
    err.set_msg("index points beyond end of list.");
    return;
  }
  
  // position the instruction iterator over the preconditionner to delete
  list<PatchDataUser*>::iterator pos;
  pos = instructions.begin();
  advance(pos, index);

  if ( (*pos)->get_algorithm_type() == PatchDataUser::QUALITY_IMPROVER ) {
    string name = (*pos)->get_name();
      //cout << "  o InstructionQueue: removing QualityImprover " << name <<  ".\n"; 
    instructions.erase(pos);
    nbPreConditionners--;
   }
   else
     err.set_msg("index does not point to a QualityImprover.");
}  


#undef __FUNC__
#define __FUNC__ "InstructionQueue::insert_preconditioner"
/*! \fn InstructionQueue::insert_preconditioner(QualityImprover* instr, size_t index, MsqError &err)
    \brief inserts a QualityImprover* into the instruction queue.

    Pre-conditionners can only be inserted before the master QualityImprover.

    \param index is 0-based. An error is set if the index does not correspond
           to a valid position in the queue.
*/
void InstructionQueue::insert_preconditioner(QualityImprover* instr,
                                           size_t index, MsqError &err)
{
  // checks index is valid
  if (isMasterSet==true && index > masterInstrIndex) {
    err.set_msg("cannot add a preconditionner after the master QualityImprover.");
    return;
  }
  if (index >= instructions.size() ) {
    err.set_msg("index points beyond end of list.");
    return;
  }

  // position the instruction iterator
  list<PatchDataUser*>::iterator pos;
  pos = instructions.begin();
  advance(pos, index);
  // adds the preconditioner
  instructions.insert(pos,instr);
  nbPreConditionners++;
}


#undef __FUNC__
#define __FUNC__ "InstructionQueue::add_quality_assessor"
/*! \fn InstructionQueue::add_quality_assessor(QualityAssessor* instr, MsqError &err)
    \brief adds a QualityAssessor to the instruction queue.

    QualityAssessor pointers can be added at any time to the instruction queue.
*/
void InstructionQueue::add_quality_assessor(QualityAssessor* instr,
                                            MsqError &/*err*/)
{
  instructions.push_back(instr);
}


#undef __FUNC__
#define __FUNC__ "InstructionQueue::remove_quality_assessor"
/*! \fn InstructionQueue::remove_quality_assessor(size_t index, MsqError &err)
    \brief removes a QualityAssessor* from the instruction queue

    \param index is 0-based. An error is set if the index does not correspond
           to a valid element in the queue.
*/
void InstructionQueue::remove_quality_assessor(size_t index, MsqError &err)
{
  // checks index is valid
  if (index >= instructions.size() ) {
    err.set_msg("index points beyond end of list.");
    return;
  }
  
  // position the instruction iterator over the QualityAssessor to delete
  list<PatchDataUser*>::iterator pos;
  pos = instructions.begin();
  advance(pos, index);

  if ( (*pos)->get_algorithm_type() == PatchDataUser::QUALITY_ASSESSOR ) {
    string name = (*pos)->get_name();
      //cout << "  o InstructionQueue: removing QualityAssessor " << name << ".\n"; 
    instructions.erase(pos);
   }
   else
     err.set_msg("index does not point to a QualityAssessor.");
}  


#undef __FUNC__
#define __FUNC__ "InstructionQueue::insert_quality_assessor"
/*! \fn InstructionQueue::insert_quality_assessor(QualityAssessor* instr, size_t index, MsqError &err)
    \brief inserts a QualityAssessor* into the instruction queue.

    QualityAssessors can be inserted at any position in the instruction queue.

    \param index is 0-based. An error is set if the index is past the end of the queue.
*/
void InstructionQueue::insert_quality_assessor(QualityAssessor* instr,
                                           size_t index, MsqError &err)
{
  // checks index is valid
  if (index > instructions.size()) {
    err.set_msg("index points two positions beyond end of list.");
    return;
  }

  // position the instruction iterator
  list<PatchDataUser*>::iterator pos;
  pos = instructions.begin();
  advance(pos, index);
  // adds the QualityAssessor
  instructions.insert(pos,instr);
}


#undef __FUNC__
#define __FUNC__ "InstructionQueue::set_master_quality_improver"
void InstructionQueue::set_master_quality_improver(QualityImprover* instr,
                                                 MsqError &err)
{
  if (isMasterSet) {
    cout << "WARNING: InstructionQueue::set_master_quality_improver():\n"
         << "\tOverwriting previously specified master quality improver.\n";
    // if master is already set, clears it and insert the new one at the same position.
    list<PatchDataUser*>::iterator master_pos;
    master_pos = this->clear_master(err); MSQ_CHKERR(err);
    instructions.insert(master_pos, instr);
    isMasterSet = true;
  } else {
    // if master is not set, add it at the end of the queue.
    instructions.push_back(instr);
    isMasterSet = true;
    masterInstrIndex = instructions.size()-1;
  }
}

#undef __FUNC__
#define __FUNC__ "InstructionQueue::run_instructions"
void InstructionQueue::run_instructions(MeshSet &ms, MsqError &err)
{ 
  Message::print_info(version_string(false));

  if (nbPreConditionners != 0 && isMasterSet == false ) {
    err.set_msg("no pre-conditionners allowed if master QualityImprover is not set.");
    return;
  }
  
#ifdef ENABLE_INTERRUPT
  MesquiteInterrupt::catch_interrupt(true);
#endif
  
  list<PatchDataUser*>::const_iterator instr_iter;
  
  // For each instruction in the list
  for (instr_iter = instructions.begin();
       !(instr_iter == instructions.end()); ++instr_iter) {
    
    // If instruction uses a global patch, creates one or reuse the existing one.
    if ((*instr_iter)->get_patch_type() == PatchData::GLOBAL_PATCH) {
      if (globalPatch == 0) {
        globalPatch = new PatchData;
        ms.get_next_patch(*globalPatch, (*instr_iter)->get_all_parameters(), err);
        MSQ_CHKERR(err);
      }
      (*instr_iter)->set_global_patch(globalPatch, err); MSQ_CHKERR(err);
      
    }
    // If instruction does not use a Global Patch, make sure we discard any existing patch.
    else {
      delete globalPatch;
      globalPatch = 0;
      (*instr_iter)->no_global_patch();
    }
    
    // applies the QualityImprover/QualityAssessor to the MeshSet
      (*instr_iter)->loop_over_mesh(ms, err); MSQ_CHKERR(err);

    // If this same instruction is reused in this queue, we can't assume the
    // global patch will still be valid. 
    (*instr_iter)->no_global_patch();
  }
 
  if (globalPatch != 0)
    delete globalPatch; // clean up once all instructions are executed.
#ifdef ENABLE_INTERRUPT
    //If an innterrupt was called, we respect that interrupt until
    //the end of the instruction queue run.  
  MesquiteInterrupt::clear_interrupt();
  MesquiteInterrupt::catch_interrupt(false);
#endif

}

  
#undef __FUNC__
#define __FUNC__ "InstructionQueue::clear"
void InstructionQueue::clear()
{
  instructions.clear();
  autoQualAssess = true;
  isMasterSet = false;
  masterInstrIndex = 0;
}


#undef __FUNC__
#define __FUNC__ "InstructionQueue::clear_master"
list<PatchDataUser*>::iterator InstructionQueue::clear_master(MsqError &err)
{
  list<PatchDataUser*>::iterator instr_iter;
  list<PatchDataUser*>::iterator master_pos;
  
  if (!isMasterSet) {
    err.set_msg("No master quality improver to clear.");
    return instr_iter;
  }
  
    // position the instruction iterator over the master quality improver
  master_pos = instructions.begin();
  advance(master_pos, masterInstrIndex);
  
    // erases the master quality improver
  instr_iter = instructions.erase(master_pos);
  isMasterSet = false;
  
    // returns the position where the Master was
  return instr_iter;
}
