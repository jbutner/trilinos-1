#include "CEpetra_Comm_Cpp.hpp"
#include "CEpetra_BlockMap_Cpp.hpp"
#include "CEpetra_BlockMap.h"
#include "Epetra_BlockMap.h"
#include "Teuchos_RCP.hpp"
#include "CTrilinos_enums.h"
#include "CTrilinos_utils.hpp"
#include "CTrilinos_Table.hpp"


namespace {


using Teuchos::RCP;
using CTrilinos::Table;


Table<Epetra_BlockMap>& tableOfBlockMaps()
{
    static Table<Epetra_BlockMap> loc_tableOfBlockMaps(CT_Epetra_BlockMap_ID, "CT_Epetra_BlockMap_ID");
    return loc_tableOfBlockMaps;
}


} // namespace


//
// Definitions from CEpetra_BlockMap.h
//


extern "C" {


CT_Epetra_BlockMap_ID_t Epetra_BlockMap_Cast (
  CTrilinos_Object_ID_t id )
{
    return CTrilinos::cast(tableOfBlockMaps(), id);
}

CT_Epetra_BlockMap_ID_t Epetra_BlockMap_Create ( 
  int NumGlobalElements, int ElementSize, int IndexBase, 
  CT_Epetra_Comm_ID_t CommID )
{
    const Teuchos::RCP<Epetra_Comm> 
        pComm = CEpetra::getComm(CommID);

    return tableOfBlockMaps().store(new Epetra_BlockMap(
        NumGlobalElements, ElementSize, IndexBase, *pComm));

}

CT_Epetra_BlockMap_ID_t Epetra_BlockMap_Create_Linear ( 
  int NumGlobalElements, int NumMyElements, int ElementSize, 
  int IndexBase, CT_Epetra_Comm_ID_t CommID )
{
    const Teuchos::RCP<Epetra_Comm> 
        pComm = CEpetra::getComm(CommID);

    return tableOfBlockMaps().store(new Epetra_BlockMap(
        NumGlobalElements, NumMyElements, ElementSize, IndexBase, *pComm));

}

CT_Epetra_BlockMap_ID_t Epetra_BlockMap_Create_Arbitrary ( 
  int NumGlobalElements, int NumMyElements, 
  const int * MyGlobalElements, int ElementSize, int IndexBase, 
  CT_Epetra_Comm_ID_t CommID )
{
    const Teuchos::RCP<Epetra_Comm> 
        pComm = CEpetra::getComm(CommID);

    return tableOfBlockMaps().store(new Epetra_BlockMap(
        NumGlobalElements, NumMyElements, MyGlobalElements, ElementSize, IndexBase, *pComm));

}

CT_Epetra_BlockMap_ID_t Epetra_BlockMap_Create_Variable ( 
  int NumGlobalElements, int NumMyElements, 
  const int * MyGlobalElements, const int * ElementSizeList, 
  int IndexBase, CT_Epetra_Comm_ID_t CommID )
{
    const Teuchos::RCP<Epetra_Comm> 
        pComm = CEpetra::getComm(CommID);

    return tableOfBlockMaps().store(new Epetra_BlockMap(
        NumGlobalElements, NumMyElements, MyGlobalElements, ElementSizeList, IndexBase, *pComm));

}

CT_Epetra_BlockMap_ID_t Epetra_BlockMap_Duplicate ( 
  CT_Epetra_BlockMap_ID_t mapID )
{
    const Teuchos::RCP<Epetra_BlockMap> 
        pmap = CEpetra::getBlockMap(mapID);

    return tableOfBlockMaps().store(new Epetra_BlockMap(*pmap));

}

void Epetra_BlockMap_Destroy ( CT_Epetra_BlockMap_ID_t * selfID )
{
    tableOfBlockMaps().remove(selfID);
}

int Epetra_BlockMap_RemoteIDList ( 
  CT_Epetra_BlockMap_ID_t selfID, int NumIDs, const int * GIDList, 
  int * PIDList, int * LIDList )
{
    return CEpetra::getBlockMap(selfID)->RemoteIDList(
        NumIDs, GIDList, PIDList, LIDList);
}

int Epetra_BlockMap_RemoteIDList_WithSize ( 
  CT_Epetra_BlockMap_ID_t selfID, int NumIDs, const int * GIDList, 
  int * PIDList, int * LIDList, int * SizeList )
{
    return CEpetra::getBlockMap(selfID)->RemoteIDList(
        NumIDs, GIDList, PIDList, LIDList, SizeList);
}

int Epetra_BlockMap_LID ( CT_Epetra_BlockMap_ID_t selfID, int GID )
{
    return CEpetra::getBlockMap(selfID)->LID(GID);
}

int Epetra_BlockMap_GID ( CT_Epetra_BlockMap_ID_t selfID, int LID )
{
    return CEpetra::getBlockMap(selfID)->GID(LID);
}

int Epetra_BlockMap_FindLocalElementID ( 
  CT_Epetra_BlockMap_ID_t selfID, int PointID, int * ElementID, 
  int * ElementOffset )
{
    return CEpetra::getBlockMap(selfID)->FindLocalElementID(
        PointID, *ElementID, *ElementOffset);
}

char Epetra_BlockMap_MyGID ( 
  CT_Epetra_BlockMap_ID_t selfID, int GID_in )
{
    return CEpetra::getBlockMap(selfID)->MyGID(GID_in);
}

char Epetra_BlockMap_MyLID ( 
  CT_Epetra_BlockMap_ID_t selfID, int LID_in )
{
    return CEpetra::getBlockMap(selfID)->MyLID(LID_in);
}

int Epetra_BlockMap_MinAllGID ( CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->MinAllGID();
}

int Epetra_BlockMap_MaxAllGID ( CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->MaxAllGID();
}

int Epetra_BlockMap_MinMyGID ( CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->MinMyGID();
}

int Epetra_BlockMap_MaxMyGID ( CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->MaxMyGID();
}

int Epetra_BlockMap_MinLID ( CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->MinLID();
}

int Epetra_BlockMap_MaxLID ( CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->MaxLID();
}

int Epetra_BlockMap_NumGlobalElements ( 
  CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->NumGlobalElements();
}

int Epetra_BlockMap_NumMyElements ( CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->NumMyElements();
}

int Epetra_BlockMap_MyGlobalElements_Fill ( 
  CT_Epetra_BlockMap_ID_t selfID, int * MyGlobalElementList )
{
    return CEpetra::getBlockMap(selfID)->MyGlobalElements(
        MyGlobalElementList);
}

int Epetra_BlockMap_ElementSize_Const ( 
  CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->ElementSize();
}

int Epetra_BlockMap_ElementSize ( 
  CT_Epetra_BlockMap_ID_t selfID, int LID )
{
    return CEpetra::getBlockMap(selfID)->ElementSize(LID);
}

int Epetra_BlockMap_FirstPointInElement ( 
  CT_Epetra_BlockMap_ID_t selfID, int LID )
{
    return CEpetra::getBlockMap(selfID)->FirstPointInElement(LID);
}

int Epetra_BlockMap_IndexBase ( CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->IndexBase();
}

int Epetra_BlockMap_NumGlobalPoints ( 
  CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->NumGlobalPoints();
}

int Epetra_BlockMap_NumMyPoints ( CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->NumMyPoints();
}

int Epetra_BlockMap_MinMyElementSize ( 
  CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->MinMyElementSize();
}

int Epetra_BlockMap_MaxMyElementSize ( 
  CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->MaxMyElementSize();
}

int Epetra_BlockMap_MinElementSize ( 
  CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->MinElementSize();
}

int Epetra_BlockMap_MaxElementSize ( 
  CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->MaxElementSize();
}

char Epetra_BlockMap_UniqueGIDs ( CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->UniqueGIDs();
}

char Epetra_BlockMap_ConstantElementSize ( 
  CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->ConstantElementSize();
}

char Epetra_BlockMap_SameAs ( 
  CT_Epetra_BlockMap_ID_t selfID, CT_Epetra_BlockMap_ID_t MapID )
{
    const Teuchos::RCP<Epetra_BlockMap> 
        pMap = CEpetra::getBlockMap(MapID);

    return CEpetra::getBlockMap(selfID)->SameAs(*pMap);
}

char Epetra_BlockMap_PointSameAs ( 
  CT_Epetra_BlockMap_ID_t selfID, CT_Epetra_BlockMap_ID_t MapID )
{
    const Teuchos::RCP<Epetra_BlockMap> 
        pMap = CEpetra::getBlockMap(MapID);

    return CEpetra::getBlockMap(selfID)->PointSameAs(*pMap);
}

char Epetra_BlockMap_LinearMap ( CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->LinearMap();
}

char Epetra_BlockMap_DistributedGlobal ( 
  CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->DistributedGlobal();
}

int * Epetra_BlockMap_MyGlobalElements ( 
  CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->MyGlobalElements();
}

int * Epetra_BlockMap_FirstPointInElementList ( 
  CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->FirstPointInElementList();
}

int * Epetra_BlockMap_ElementSizeList ( 
  CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->ElementSizeList();
}

int * Epetra_BlockMap_PointToElementList ( 
  CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->PointToElementList();
}

int Epetra_BlockMap_ElementSizeList_Fill ( 
  CT_Epetra_BlockMap_ID_t selfID, int * ElementSizeList )
{
    return CEpetra::getBlockMap(selfID)->ElementSizeList(
        ElementSizeList);
}

int Epetra_BlockMap_FirstPointInElementList_Fill ( 
  CT_Epetra_BlockMap_ID_t selfID, int * FirstPointInElementList )
{
    return CEpetra::getBlockMap(selfID)->FirstPointInElementList(
        FirstPointInElementList);
}

int Epetra_BlockMap_PointToElementList_Fill ( 
  CT_Epetra_BlockMap_ID_t selfID, int * PointToElementList )
{
    return CEpetra::getBlockMap(selfID)->PointToElementList(
        PointToElementList);
}

char Epetra_BlockMap_IsOneToOne ( CT_Epetra_BlockMap_ID_t selfID )
{
    return CEpetra::getBlockMap(selfID)->IsOneToOne();
}


} // extern "C"


//
// Definitions from CEpetra_BlockMap_Cpp.hpp
//


const Teuchos::RCP<Epetra_BlockMap>
CEpetra::getBlockMap( CT_Epetra_BlockMap_ID_t id )
{
    return tableOfBlockMaps().get(id);
}

void
CEpetra::purgeBlockMapTable(  )
{
    tableOfBlockMaps().purge();
}



