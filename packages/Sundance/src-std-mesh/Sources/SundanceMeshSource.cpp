#include "SundanceMeshSource.hpp"
#include "SundanceOut.hpp"

using namespace Sundance;
using namespace Sundance::StdMesh::Internal;
using namespace TSFExtended;
using namespace Teuchos;



MeshSource::MeshSource()
  : Handle<MeshSourceBase>()
{}

MeshSource::MeshSource(Handleable<MeshSourceBase>* rawPtr)
  : Handle<MeshSourceBase>(rawPtr)
{}


MeshSource::MeshSource(const RefCountPtr<MeshSourceBase>& smartPtr)
  : Handle<MeshSourceBase>(smartPtr)
{}

Mesh MeshSource::getMesh() const
{
  Mesh rtn;
  try
    {
      int nProc = ptr()->comm().getNProc();
      SUNDANCE_OUT(ptr()->verbosity() > VerbSilent, 
                   "MeshSource::getMesh()");
      if (serializeLocal() && nProc > 1)
        {
          int myRank = ptr()->comm().getRank();
          for (int p=0; p<nProc; p++)
            {
              ptr()->comm().synchronize();
              if (p != myRank) continue;
              SUNDANCE_OUT(ptr()->verbosity() > VerbSilent, 
                           "========= Building local mesh on processor " 
                           << p << " ============ ");
              rtn = ptr()->getMesh();
            }
        }
      else 
        {
          rtn = ptr()->getMesh();
        }
      if (rtn.spatialDim() > 1) rtn.assignIntermediateCellOwners(1);
      if (rtn.spatialDim() > 2) rtn.assignIntermediateCellOwners(2);
    }
  catch(std::exception& e)
    {
      SUNDANCE_TRACE(e);
    }
  return rtn;
}

void MeshSource::getAttributes(RefCountPtr<Array<Array<double> > >& nodeAttributes,
                               RefCountPtr<Array<Array<double> > >& elemAttributes) const
{
  getMesh();
  ptr()->getAttributes(nodeAttributes, elemAttributes);
}
