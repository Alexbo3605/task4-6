/////////////////////////////////////////////////////////////////////////////// 
// Copyright (C) 2002-2016, Open Design Alliance (the "Alliance"). 
// All rights reserved. 
// 
// This software and its documentation and related materials are owned by 
// the Alliance. The software may only be incorporated into application 
// programs owned by members of the Alliance, subject to a signed 
// Membership Agreement and Supplemental Software License Agreement with the
// Alliance. The structure and organization of this software are the valuable  
// trade secrets of the Alliance and its suppliers. The software is also 
// protected by copyright law and international treaty provisions. Application  
// programs incorporating this software must include the following statement 
// with their copyright notices:
//   
//   This application incorporates Teigha(R) software pursuant to a license 
//   agreement with Open Design Alliance.
//   Teigha(R) Copyright (C) 2002-2016 by Open Design Alliance. 
//   All rights reserved.
//
// By use of this software, its documentation or related materials, you 
// acknowledge and accept the above terms.
///////////////////////////////////////////////////////////////////////////////

// ExLine.cpp: implementation of the ExLine function.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "DbLine.h"


#include "DbHostAppServices.h"
#include "DbIdMapping.h"


void _CloneSelect_func(OdEdCommandContext* pCmdCtx)
{
    OdDbCommandContextPtr pDbCmdCtx(pCmdCtx);
    OdDbDatabasePtr pDb = pDbCmdCtx->database();
    OdDbUserIO* pIO = pDbCmdCtx->dbUserIO();


    pDb->startUndoRecord();
    pDb->startTransaction();
   
    // get selected objects
    OdDbObjectIdArray arrToCopy = OdDbSelectionSet::cast(pIO->select(
        L"",
        OdEd::kSelAllowObjects |
        OdEd::kSelAllowInactSpaces
    ).get())->objectIdArray();


    if (arrToCopy.isEmpty())
        return;
    try 
    {
        //get the name and path of the file
        OdString fileName = pIO->getString(L"Enter the name of the file:");
        fileName += L".dwg";
        const OdString filePath = pDb->appServices()->findFile(fileName);
            
        //create a new base and clone objects there
        OdDbDatabasePtr pNewDb = pDb->appServices()->createDatabase();


        //file does not exist
        if (filePath.isEmpty()) 
        {
            OdDbIdMappingPtr idMap = OdDbIdMapping::createObject();

            pDb->wblockCloneObjects(arrToCopy, pNewDb->getModelSpaceId(), *idMap.get(), OdDb::kDrcReplace); 

            //create a new file
            OdStreamBufPtr pFile = ::odSystemServices()->createFile(
                fileName,
                Oda::kFileWrite,
                Oda::kShareDenyReadWrite,
                Oda::kCreateAlways);

            pNewDb->writeFile(pFile, OdDb::kDwg, OdDb::kDHL_CURRENT);
        }
        else
        {
            //читаємо дані з файлу у БД
            pNewDb = pDb->appServices()->readFile(fileName);

            OdDbIdMappingPtr idMap = OdDbIdMapping::createObject();
           
            pNewDb->wblockCloneObjects(arrToCopy, pNewDb->getModelSpaceId(), *idMap.get(), OdDb::kDrcReplace);

            pNewDb->writeFile(fileName, OdDb::kDwg, OdDb::kDHL_CURRENT);
        }
    }
    catch (const OdError& err)
    {
        OdString strMessage = err.description();
        pIO->putString(strMessage);

        pDb->abortTransaction();
    }
    pDb->undo();
}
