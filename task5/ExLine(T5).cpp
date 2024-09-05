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

#include <ctime>
#include <sstream>
#include <random>
#include <unordered_map>

//пошук елементів та їх видалення
void _F2_func(OdEdCommandContext* pCmdCtx)
{
    OdDbCommandContextPtr pDbCmdCtx(pCmdCtx);
    OdDbDatabasePtr pDb = pDbCmdCtx->database();
    OdDbUserIO* pIO = pDbCmdCtx->dbUserIO();

    //start - this code do not touch
    const int size = 1000000;
    const int sizeExclusions = 1000;
    OdArray<int> idArr(size);
    OdArray<int> exclusions(sizeExclusions);


    for (int i = 0; i < size; i++)
    {
        idArr.push_back(i);
    }
    const int rand_max = size - 1;

    for (int i = 0; i < sizeExclusions; i++) 
        exclusions.push_back(std::rand() % size);

    std::random_shuffle(idArr.begin(), idArr.end());
    //end - this code do not touch

    std::clock_t start_time = std::clock();
    ///////////////////////////////////////////////////////////////////////////////

    //0.202
    std::unordered_map<int, bool> exclusionMap;
    for (const auto& exclusion : exclusions) {
        exclusionMap[exclusion] = true;
    }

    auto isExcluded = [&exclusionMap](int id) {
        return exclusionMap.find(id) != exclusionMap.end();
        };

    idArr.erase(
        std::remove_if(idArr.begin(), idArr.end(), isExcluded),
        idArr.end()
    );
    

    ///////////////////////////////////////////////////////////////////////////////
    std::clock_t end_time = std::clock();

    double elapsed_time = double(end_time - start_time) / CLOCKS_PER_SEC;
    std::ostringstream ss;
    ss << elapsed_time;
    std::string elapsed_time_str = ss.str();


    OdString str(elapsed_time_str.c_str());
    str.insert(str.getLength(), L" sec11");

    pIO->putString(str);
}


#include "DbHostAppServices.h"
#include "DbIdMapping.h"
#include "DbViewport.h"


#include "Ed/EdCommandStack.h"
#include "DbBlockReference.h" 

#include "DbLayerTableRecord.h"
#include "DbLayerTable.h"


#include <ctime>
#include <sstream>

OdDbObjectIdArray selectEntities(OdDbUserIO* pIO)
{
    OdDbObjectIdArray entIds;
    OdDbSelectionSetIteratorPtr it = pIO->select(L"Select entities", OdEd::kSelAllowInactSpaces)->newIterator();
    while (!it->done())
    {
        OdDbEntityPtr pEnt = OdDbEntity::cast(it->objectId().openObject());
        if (pEnt.get())
        {
            entIds.append(it->objectId());
        }
        else
        {
            pIO->putString(L"1 selected object is not an entity - skipped");
        }
        it->next();
    }
    if (entIds.size() == 0)
    {
        pIO->putString(L"Nothing selected");
    }
    return entIds;
}
////////////////////////////////////////////////////////////////////////////




//зміна кольору обєктів всіх окрім вибраних
void transformReferenceToArray(OdDbBlockReference* pRef, OdDbObjectIdArray& ObjectsArray)
{
    OdDbBlockTableRecordPtr pSpace = pRef->blockTableRecord().openObject(OdDb::kForRead);
    OdDbObjectIteratorPtr pIterator = pSpace->newIterator();
    for (; !pIterator->done(); pIterator->step())
    {
        ObjectsArray.append(pIterator->objectId());
    }
}


void convertBlockIdToEntityID2(OdDbObjectIdArray& arrayIN,  OdDbObjectIdArray& arrayRefOUT, OdDbObjectIdArray* arrayObjOUT = nullptr)
{
    for (OdDbObjectIdArray::iterator i = arrayIN.begin(); i != arrayIN.end(); ++i) {

        OdDbEntityPtr pEnt = i->openObject(OdDb::kForRead);

        if (pEnt->isA() == OdDbBlockReference::desc())
        {
            arrayRefOUT.append(*i);

            OdDbBlockReferencePtr pRef = OdDbBlockReference::cast(pEnt);
            OdDbObjectIdArray nestedArray;
            transformReferenceToArray(pRef, nestedArray);
            convertBlockIdToEntityID2(nestedArray, arrayRefOUT);

        }
        else if (arrayObjOUT != nullptr)
        {
            arrayObjOUT->append(*i);
        }
    }
}



void _TestS_func(OdEdCommandContext* pCmdCtx)
{
    std::clock_t start_time = std::clock();

    OdDbCommandContextPtr pDbCmdCtx(pCmdCtx);
    OdDbDatabasePtr pDb = pDbCmdCtx->database();
    OdDbUserIO* pIO = pDbCmdCtx->dbUserIO();


    OdDbObjectIdArray arrSelectRecords;
    OdDbObjectIdArray arrSelectObjects;

    OdCmColor colWhite;
    OdCmColor colFcf;
    colFcf.setColorMethod(OdCmEntityColor::kByColor);
    colFcf.setRGB(48, 111, 113);
    

    OdDbObjectId spaceId = ExCommandsUtils::activeBlockTableRecord(pDb);
    OdDbBlockTableRecordPtr pSpace = spaceId.openObject(OdDb::kForRead);

    OdDbObjectIteratorPtr pIterator = pSpace->newIterator();
    for (; !pIterator->done(); pIterator->step())
    {
        OdDbEntityPtr pEnt = pIterator->entity(OdDb::kForWrite);
        pEnt->setColor(colFcf);
    }

    OdDbObjectIdArray bufArrSelect = OdDbSelectionSet::cast(pIO->select(
        L"",
        OdEd::kSelAllowInactSpaces
    ).get())->objectIdArray();

    int size1 = bufArrSelect.size();

    /*convertBlockIdToEntityID2(bufArrSelect, arrSelectRecords, &arrSelectObjects);*/

    int sizeAllObjects1 = arrSelectRecords.size();
    int sizeSelect1 = arrSelectObjects.size();


 

    colWhite.setColorMethod(OdCmEntityColor::kByColor);
    colWhite.setRGB(255, 255, 255);

    for (OdDbObjectIdArray::iterator i = bufArrSelect.begin(); i != bufArrSelect.end(); ++i)
    {
        OdDbObjectPtr pObj = i->openObject(OdDb::kForWrite);
        OdDbEntityPtr pEntity = OdDbEntity::cast(pObj);
        pEntity->setColor(colWhite);
    }

    /*for (OdDbObjectIdArray::iterator i = arrSelectRecords.begin(); i != arrSelectRecords.end(); ++i)
    {
        OdDbObjectPtr pObj = i->openObject(OdDb::kForWrite);
        OdDbEntityPtr pEntity = OdDbEntity::cast(pObj);
        pEntity->setColor(colWhite);
    }
    for (OdDbObjectIdArray::iterator i = arrSelectObjects.begin(); i != arrSelectObjects.end(); ++i)
    {
        OdDbObjectPtr pObj = i->openObject(OdDb::kForWrite);
        OdDbEntityPtr pEntity = OdDbEntity::cast(pObj);
        pEntity->setColor(colWhite);
    }*/


    // Update the rendering tile
    ::odedRegCmds()->executeCommand(OD_T("REGEN"), pCmdCtx);



    // Кінцевий час
    std::clock_t end_time = std::clock();

    // Вираховуємо час виконання
    double elapsed_time = double(end_time - start_time) / CLOCKS_PER_SEC;
    std::ostringstream ss;
    ss << elapsed_time;
    std::string elapsed_time_str = ss.str();

    // Створення OdString з рядка
    OdString str(elapsed_time_str.c_str());


    str.insert(str.getLength(), L" sec11");


    pIO->putString(str);

}


void _F1_func(OdEdCommandContext* pCmdCtx)
{
    std::clock_t start_time = std::clock();


    OdDbCommandContextPtr pDbCmdCtx(pCmdCtx);
    OdDbDatabasePtr pDb = pDbCmdCtx->database();
    OdDbUserIO* pIO = pDbCmdCtx->dbUserIO();

    OdDbObjectIdArray bufArrSelect = OdDbSelectionSet::cast(pIO->select(
        L"",
        OdEd::kSelAllowInactSpaces
    ).get())->objectIdArray();

    OdDbObjectIdArray arrSelectRecords;
    OdDbObjectIdArray arrSelectObjects;

    // get the array of all objects in ModelSpace/PaperSpace
    OdDbObjectIdArray bufArrAllObjects;
    OdDbObjectIdArray arrAllObjects;
    OdDbObjectIdArray arrAllRecords;
    OdDbObjectIdArray arrAllRecords1;
    

    OdCmColor colFcf;
    colFcf.setColorMethod(OdCmEntityColor::kByColor);
    colFcf.setRGB(48, 111, 113);

    OdCmColor colRed;
    colRed.setColorMethod(OdCmEntityColor::kByColor);
    colRed.setRGB(255, 0, 0);

    OdDbObjectId spaceId = ExCommandsUtils::activeBlockTableRecord(pDb);
    OdDbBlockTableRecordPtr pSpace = spaceId.openObject(OdDb::kForRead);

    OdDbObjectIteratorPtr pIterator = pSpace->newIterator();
    for (; !pIterator->done(); pIterator->step())
    {
        OdDbEntityPtr pEnt = pIterator->entity(OdDb::kForRead);

        if (pEnt->isA() == OdDbBlockReference::desc())
        {
            arrAllRecords.append(pIterator->objectId()); //синім
        }
        else
        {
            arrAllObjects.append(pIterator->objectId()); //червоним
        }
    }


    for (OdDbObjectIdArray::iterator i = arrAllObjects.begin(); i != arrAllObjects.end(); ++i)
    {
        OdDbObjectPtr pObj = i->openObject(OdDb::kForWrite);
        OdDbEntityPtr pEntity = OdDbEntity::cast(pObj);

        pEntity->setColor(colRed);
    }

    int sizeAllObjects = arrAllRecords.size(); //28
    int sizeSelect = arrAllObjects.size(); //20000


    //зробити так аби ід бралося лише з верху
    /*convertBlockIdToEntityID(bufArrSelect, arrSelectRecords, arrSelectObjects);*/

    int sizeAllObjects1 = arrSelectRecords.size(); // 1
    int sizeSelect1 = arrSelectObjects.size(); // 2500


    //for (OdDbObjectIdArray::iterator i = arrSelectRecords.begin(); i != arrSelectRecords.end(); ++i)
    //{
    //    arrAllRecords.remove(*i);
    //}

    /*convertBlockIdToEntityID(arrAllRecords, arrAllRecords1, arrAllObjects);*/

    sizeSelect1 = arrAllRecords1.size(); // 25030
    sizeAllObjects1 = arrAllObjects.size(); // 30010  || без find набирає копії на 85000



 

    for (OdDbObjectIdArray::iterator i = arrAllRecords1.begin(); i != arrAllRecords1.end(); ++i)
    {
        OdDbObjectPtr pObj = i->openObject(OdDb::kForWrite);
        OdDbEntityPtr pEntity = OdDbEntity::cast(pObj);

        pEntity->setColor(colFcf);
    }


    //for (OdDbObjectIdArray::iterator i = arrSelectRecords.begin(); i != arrSelectRecords.end(); ++i)
    //{
    //    arrAllRecords1.remove(*i);
    //}
    //for (OdDbObjectIdArray::iterator i = arrSelectObjects.begin(); i != arrSelectObjects.end(); ++i)
    //{
    //    arrAllObjects.remove(*i); 
    //}




    //for (OdDbObjectIdArray::iterator i = arrAllObjects.begin(); i != arrAllObjects.end(); ++i)
    //{
    //    OdDbObjectPtr pObj = i->openObject(OdDb::kForWrite);
    //    OdDbEntityPtr pEntity = OdDbEntity::cast(pObj);

    //    pEntity->setColor(colFcf);
    //}


    //for (OdDbObjectIdArray::iterator i = arrAllObjects.begin(); i != arrAllObjects.end(); ++i)
    //{
    //    OdDbObjectPtr pObj = i->openObject(OdDb::kForWrite);
    //    OdDbEntityPtr pEntity = OdDbEntity::cast(pObj);

    //    pEntity->setColor(colFcf);
    //}

    ::odedRegCmds()->executeCommand(OD_T("REGEN"), pCmdCtx);


    // Кінцевий час
    std::clock_t end_time = std::clock();

    // Вираховуємо час виконання
    double elapsed_time = double(end_time - start_time) / CLOCKS_PER_SEC;
    std::ostringstream ss;
    ss << elapsed_time;
    std::string elapsed_time_str = ss.str();

    // Створення OdString з рядка
    OdString str(elapsed_time_str.c_str());


    str.insert(str.getLength(), L" sec11");


    pIO->putString(str);
}

////////////////////////////////////////////////////////////////////////////////////////




void _Cls_func(OdEdCommandContext* pCmdCtx)
{
    OdDbCommandContextPtr pDbCmdCtx(pCmdCtx);
    OdDbDatabasePtr pDb = pDbCmdCtx->database();
    OdDbObjectId spaceId = ExCommandsUtils::activeBlockTableRecord(pDb);
    OdDbBlockTableRecordPtr pSpace = spaceId.openObject(OdDb::kForRead);

    OdDbObjectIdArray arrAllObjects; 
    OdDbObjectIdArray arrAllObjectsBuf;



    OdDbObjectIteratorPtr pIterator = pSpace->newIterator();
    for (; !pIterator->done(); pIterator->step())
    {
        arrAllObjectsBuf.append(pIterator->objectId());
    }

    /*convertBlockIdToEntityID(arrAllObjectsBuf, arrAllObjects);*/

    OdCmColor colFcf;
    colFcf.setColorMethod(OdCmEntityColor::kByColor);
    colFcf.setRGB(255, 255, 255);

    for (OdDbObjectIdArray::iterator i = arrAllObjects.begin(); i != arrAllObjects.end(); ++i)
    {
        OdDbObjectPtr pObj = i->openObject(OdDb::kForWrite);
        OdDbEntityPtr pEntity = OdDbEntity::cast(pObj);
        pEntity->setColor(colFcf); 
    }
  
    ::odedRegCmds()->executeCommand(OD_T("REGEN"), pCmdCtx);
}
