#include "Xpetra_ConfigDefs.hpp"

#ifdef HAVE_XPETRA_EPETRA

#include "Xpetra_StridedEpetraMap.hpp"
#include "Xpetra_EpetraUtils.hpp"

#include "Xpetra_EpetraExceptions.hpp"

// MPI helper
#define sumAll(rcpComm, in, out)                                        \
  Teuchos::reduceAll(*rcpComm, Teuchos::REDUCE_SUM, in, Teuchos::outArg(out));
#define minAll(rcpComm, in, out)                                        \
  Teuchos::reduceAll(*rcpComm, Teuchos::REDUCE_MIN, in, Teuchos::outArg(out));
#define maxAll(rcpComm, in, out)                                        \
  Teuchos::reduceAll(*rcpComm, Teuchos::REDUCE_MAX, in, Teuchos::outArg(out));

namespace Xpetra {

  // Implementation note for constructors: the Epetra_Comm is cloned in the constructor of Epetra_BlockMap. We don't need to keep a reference on it.
  // TODO: use toEpetra() function here.
  StridedEpetraMap::StridedEpetraMap(global_size_t numGlobalElements, int indexBase, std::vector<size_t>& stridingInfo, const Teuchos::RCP<const Teuchos::Comm<int> > &comm,
                        LocalOrdinal stridedBlockId, LocalGlobal lg, const Teuchos::RCP<Node> &node)
  : EpetraMap(numGlobalElements, indexBase, comm, lg, node), StridedMap<int, int>(numGlobalElements, indexBase, stridingInfo, comm, stridedBlockId)
  {    
    // check input data and reorganize map

    global_size_t numGlobalNodes = numGlobalElements / getFixedBlockSize();	// number of nodes (over all processors)
    
    // build an equally distributed node map
    RCP<Epetra_Map> nodeMap = Teuchos::null;
    IF_EPETRA_EXCEPTION_THEN_THROW_GLOBAL_INVALID_ARG((nodeMap = (rcp(new Epetra_Map(numGlobalNodes, indexBase, *toEpetra(comm))))));
    
     // translate local node ids to local dofs
    int nStridedOffset = 0;
    int nDofsPerNode = Teuchos::as<int>(getFixedBlockSize()); // dofs per node for local striding block
    if(stridedBlockId > -1) {
      // determine nStridedOffset
      for(int j=0; j<stridedBlockId; j++) {
	nStridedOffset += stridingInfo[j];
      }
      nDofsPerNode = stridingInfo[stridedBlockId];
      
      numGlobalElements = nodeMap->NumGlobalElements()*nDofsPerNode;
    }
    std::vector<int> dofgids;
    for(int i = 0; i<nodeMap->NumMyElements(); i++) {
      int gid = nodeMap->GID(i);
      for(int dof = 0; dof < nDofsPerNode; ++dof) {
	dofgids.push_back(gid*getFixedBlockSize() + nStridedOffset + dof);
      }
    }
    
    if (numGlobalElements == Teuchos::OrdinalTraits<global_size_t>::invalid()) {
      IF_EPETRA_EXCEPTION_THEN_THROW_GLOBAL_INVALID_ARG((map_ = (rcp(new Epetra_BlockMap(-1, dofgids.size(), &dofgids[0], 1, indexBase, *toEpetra(comm))))));
    } else {
      IF_EPETRA_EXCEPTION_THEN_THROW_GLOBAL_INVALID_ARG((map_ = (rcp(new Epetra_BlockMap(numGlobalElements, dofgids.size(), &dofgids[0], 1, indexBase, *toEpetra(comm))))));
    }
    
    TEUCHOS_TEST_FOR_EXCEPTION(map_->NumMyPoints() % nDofsPerNode != 0, Exceptions::RuntimeError, "StridedEpetraMap::StridedEpetraMap: wrong distribution of dofs among processors.");
    if(stridedBlockId == -1) {
      TEUCHOS_TEST_FOR_EXCEPTION(getNodeNumElements() != Teuchos::as<size_t>(nodeMap->NumMyElements()*nDofsPerNode), Exceptions::RuntimeError, "StridedEpetraMap::StridedEpetraMap: wrong distribution of dofs among processors.");
      TEUCHOS_TEST_FOR_EXCEPTION(getGlobalNumElements() != Teuchos::as<size_t>(nodeMap->NumGlobalElements()*nDofsPerNode), Exceptions::RuntimeError, "StridedEpetraMap::StridedEpetraMap: wrong distribution of dofs among processors.");
    } else {
      TEUCHOS_TEST_FOR_EXCEPTION(stridingInfo.size() < Teuchos::as<size_t>(stridedBlockId), Exceptions::RuntimeError, "StridedEpetraMap::StridedEpetraMap: stridedBlockId > stridingInfo.size()");      
      int nDofsInStridedBlock = stridingInfo[stridedBlockId];
      TEUCHOS_TEST_FOR_EXCEPTION(getNodeNumElements() != Teuchos::as<size_t>(nodeMap->NumMyElements()*nDofsInStridedBlock), Exceptions::RuntimeError, "StridedEpetraMap::StridedEpetraMap: wrong distribution of dofs among processors.");
      TEUCHOS_TEST_FOR_EXCEPTION(getGlobalNumElements() != Teuchos::as<size_t>(nodeMap->NumGlobalElements()*nDofsInStridedBlock), Exceptions::RuntimeError, "StridedEpetraMap::StridedEpetraMap: wrong distribution of dofs among processors.");
    }  
    
    TEUCHOS_TEST_FOR_EXCEPTION(CheckConsistency() == false, Exceptions::RuntimeError, "StridedEpetraMap::StridedEpetraMap: CheckConsistency() == false");      
  }

  StridedEpetraMap::StridedEpetraMap(global_size_t numGlobalElements, size_t numLocalElements, int indexBase,
                       std::vector<size_t>& stridingInfo, const Teuchos::RCP<const Teuchos::Comm<int> > &comm, LocalOrdinal stridedBlockId, const Teuchos::RCP<Node> &node)
  : EpetraMap(numGlobalElements, numLocalElements, indexBase, comm, node), StridedMap<int, int>(numGlobalElements, numLocalElements, indexBase, stridingInfo, comm,stridedBlockId)
  {
    // check input data and reorganize map

    global_size_t numGlobalNodes = numGlobalElements / getFixedBlockSize();	// number of nodes (over all processors)
    size_t        numLocalNodes  = numLocalElements / getFixedBlockSize();      // number of nodes (on each processor)
    
    // build an equally distributed node map
    RCP<Epetra_Map> nodeMap = Teuchos::null;
    IF_EPETRA_EXCEPTION_THEN_THROW_GLOBAL_INVALID_ARG((nodeMap = (rcp(new Epetra_Map(numGlobalNodes, numLocalNodes, indexBase, *toEpetra(comm))))));
    
    // translate local node ids to local dofs
    int nStridedOffset = 0;
    int nDofsPerNode = Teuchos::as<int>(getFixedBlockSize()); // dofs per node for local striding block
    if(stridedBlockId > -1) {
      // determine nStridedOffset
      for(int j=0; j<stridedBlockId; j++) {
	nStridedOffset += stridingInfo[j];
      }
      nDofsPerNode = stridingInfo[stridedBlockId];
      
      numGlobalElements = nodeMap->NumGlobalElements()*nDofsPerNode;
    }
    std::vector<int> dofgids;
    for(int i = 0; i<nodeMap->NumMyElements(); i++) {
      int gid = nodeMap->GID(i);
      for(int dof = 0; dof < nDofsPerNode; ++dof) {
	dofgids.push_back(gid*getFixedBlockSize() + nStridedOffset + dof);
      }
    }
    
    if (numGlobalElements == Teuchos::OrdinalTraits<global_size_t>::invalid()) {
      IF_EPETRA_EXCEPTION_THEN_THROW_GLOBAL_INVALID_ARG((map_ = (rcp(new Epetra_BlockMap(-1, dofgids.size(), &dofgids[0], 1, indexBase, *toEpetra(comm))))));
    } else {
      IF_EPETRA_EXCEPTION_THEN_THROW_GLOBAL_INVALID_ARG((map_ = (rcp(new Epetra_BlockMap(numGlobalElements, dofgids.size(), &dofgids[0], 1, indexBase, *toEpetra(comm))))));
    }
    
    TEUCHOS_TEST_FOR_EXCEPTION(map_->NumMyPoints() % nDofsPerNode != 0, Exceptions::RuntimeError, "StridedEpetraMap::StridedEpetraMap: wrong distribution of dofs among processors.");
    if(stridedBlockId == -1) {
      TEUCHOS_TEST_FOR_EXCEPTION(getNodeNumElements() != Teuchos::as<size_t>(nodeMap->NumMyElements()*nDofsPerNode), Exceptions::RuntimeError, "StridedEpetraMap::StridedEpetraMap: wrong distribution of dofs among processors.");
      TEUCHOS_TEST_FOR_EXCEPTION(getGlobalNumElements() != Teuchos::as<size_t>(nodeMap->NumGlobalElements()*nDofsPerNode), Exceptions::RuntimeError, "StridedEpetraMap::StridedEpetraMap: wrong distribution of dofs among processors.");
    } else {
      TEUCHOS_TEST_FOR_EXCEPTION(stridingInfo.size() < Teuchos::as<size_t>(stridedBlockId), Exceptions::RuntimeError, "StridedEpetraMap::StridedEpetraMap: stridedBlockId > stridingInfo.size()");      
      int nDofsInStridedBlock = stridingInfo[stridedBlockId];
      TEUCHOS_TEST_FOR_EXCEPTION(getNodeNumElements() != Teuchos::as<size_t>(nodeMap->NumMyElements()*nDofsInStridedBlock), Exceptions::RuntimeError, "StridedEpetraMap::StridedEpetraMap: wrong distribution of dofs among processors.");
      TEUCHOS_TEST_FOR_EXCEPTION(getGlobalNumElements() != Teuchos::as<size_t>(nodeMap->NumGlobalElements()*nDofsInStridedBlock), Exceptions::RuntimeError, "StridedEpetraMap::StridedEpetraMap: wrong distribution of dofs among processors.");
    }    
    
    TEUCHOS_TEST_FOR_EXCEPTION(CheckConsistency() == false, Exceptions::RuntimeError, "StridedEpetraMap::StridedEpetraMap: CheckConsistency() == false");      
  }
    
#if 0    
  StridedEpetraMap::StridedEpetraMap(global_size_t numGlobalElements, const Teuchos::ArrayView<const int> &elementList, int indexBase,
                       std::vector<size_t>& stridingInfo, const Teuchos::RCP<const Teuchos::Comm<int> > &comm, LocalOrdinal stridedBlockId, const Teuchos::RCP<Node> &node)
  : EpetraMap(numGlobalElements, elementList, indexBase, comm, node), StridedMap<int, int>(numGlobalElements, elementList, indexBase, stridingInfo, comm, stridedBlockId)
  {   
    int nDofsPerNode = Teuchos::as<int>(getFixedBlockSize());
    TEUCHOS_TEST_FOR_EXCEPTION(map_->NumMyPoints() % nDofsPerNode != 0, Exceptions::RuntimeError, "StridedEpetraMap::StridedEpetraMap: wrong distribution of dofs among processors.");
    if(stridedBlockId == -1) {
    } else {
      TEUCHOS_TEST_FOR_EXCEPTION(stridingInfo.size() < Teuchos::as<size_t>(stridedBlockId), Exceptions::RuntimeError, "StridedEpetraMap::StridedEpetraMap: stridedBlockId > stridingInfo.size()");      
    }   
  }
#endif
   

  std::string StridedEpetraMap::description() const {
    std::ostringstream oss;
    oss << EpetraMap::description();
    oss << "{getGlobalNumElements() = " << EpetraMap::getGlobalNumElements()
        << ", getNodeNumElements() = " << EpetraMap::getNodeNumElements()
        << ", isContiguous() = " << EpetraMap::isContiguous()
        << ", isDistributed() = " << EpetraMap::isDistributed()
        << "}";
    return oss.str();
  }

  void StridedEpetraMap::describe( Teuchos::FancyOStream &out, const Teuchos::EVerbosityLevel verbLevel) const {
      
    const Teuchos::RCP<const Teuchos::Comm<int> > comm_ = EpetraMap::getComm();

    // This implementation come from Tpetra_Map_def.hpp (without modification)
    using std::endl;
    using std::setw;
    using Teuchos::VERB_DEFAULT;
    using Teuchos::VERB_NONE;
    using Teuchos::VERB_LOW;
    using Teuchos::VERB_MEDIUM;
    using Teuchos::VERB_HIGH;
    using Teuchos::VERB_EXTREME;
     
    const size_t nME = EpetraMap::getNodeNumElements();
    Teuchos::ArrayView<const int> myEntries = EpetraMap::getNodeElementList();
    int myImageID = comm_->getRank();
    int numImages = comm_->getSize();
     
    Teuchos::EVerbosityLevel vl = verbLevel;
    if (vl == VERB_DEFAULT) vl = VERB_LOW;
     
    size_t width = 1;
    for (size_t dec=10; dec<EpetraMap::getGlobalNumElements(); dec *= 10) {
      ++width;
    }
    width = std::max<size_t>(width,12) + 2;
     
    Teuchos::OSTab tab(out);
     
    if (vl == VERB_NONE) {
      // do nothing
    }
    else if (vl == VERB_LOW) {
      out << this->description() << endl;
    }
    else {  // MEDIUM, HIGH or EXTREME
      for (int imageCtr = 0; imageCtr < numImages; ++imageCtr) {
        if (myImageID == imageCtr) {
          if (myImageID == 0) { // this is the root node (only output this info once)
            out << endl
                << "Number of Global Entries = " << EpetraMap::getGlobalNumElements()  << endl
                << "Maximum of all GIDs      = " << EpetraMap::getMaxAllGlobalIndex() << endl
                << "Minimum of all GIDs      = " << EpetraMap::getMinAllGlobalIndex() << endl
                << "Index Base               = " << EpetraMap::getIndexBase()         << endl;
          }
          out << endl;
          if (vl == VERB_HIGH || vl == VERB_EXTREME) {
            out << "Number of Local Elements   = " << nME           << endl
                << "Maximum of my GIDs         = " << EpetraMap::getMaxGlobalIndex() << endl
                << "Minimum of my GIDs         = " << EpetraMap::getMinGlobalIndex() << endl;
            out << endl;
          }
          if (vl == VERB_EXTREME) {
            out << std::setw(width) << "Node ID"
                << std::setw(width) << "Local Index"
                << std::setw(width) << "Global Index"
                << endl;
            for (size_t i=0; i < nME; i++) {
              out << std::setw(width) << myImageID
                  << std::setw(width) << i
                  << std::setw(width) << myEntries[i]
                  << endl;
            }
            out << std::flush;
          }
        }
        // Do a few global ops to give I/O a chance to complete
        comm_->barrier();
        comm_->barrier();
        comm_->barrier();
      }
    }
  }

}

#endif
