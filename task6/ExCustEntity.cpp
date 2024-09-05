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

/************************************************************************/
/* ExCustEntity.cpp: implementation of the ExCustEntity class           */
/*                                                                      */
/* ExCustEntity is derived from OdDbCircle, but includes an m_nCount    */
/* member which determines the display characteristics of the           */ 
/* ExCustEntity.                                                        */
/*                                                                      */
/* m_nCount  Display                                                    */
/*  <= 1     A circle, whose center is the center of the ExCustEntity,  */
/*           is solid hatched and displayed.                            */
/*  >1       A polar array of m_nCount circles, whose center is the     */
/*           of the ExCustEntity, is solid hatched and displayed        */
/*                                                                      */
/* In addition, any block inserts with attributes with tags of          */
/* "RADIUS" and "COUNT" will override the stored radius and             */
/* and m_nCount of the ExCustEntity.                                    */
/*                                                                      */                                
/************************************************************************/
#include "OdaCommon.h"
#include "ExCustEntity.h"
#include "DbFiler.h"
#include "Gi/GiWorldDraw.h"
#include "DbProxyEntity.h"
#include "DbHatch.h"
#include "Ge/GeCircArc2d.h"
#include "DbCommandContext.h"
#include "DbDatabase.h"
#include "DbBlockTableRecord.h"
#include "DbUserIO.h"
#include "Gi/GiPathNode.h"
#include "DbBlockReference.h"
#include "DbAttribute.h"
#include "OdDToStr.h"
#include "StaticRxObject.h"
#include "Gs/Gs.h"





#include "Ge/GeCircArc3d.h"
#include "DbFiler.h"			// need for OdDbDwgFiler class and OdDbDxfFiler class
#include "DbAudit.h"

const double kCurrentVersionNumber = 1.0;


ODRX_DXF_DEFINE_MEMBERS(ExCustEntity,                                                  // ClassName
                        OdDbCircle,                                                    // ParentClass
                        DBOBJECT_CONSTR,                                               // DOCREATE
                        OdDb::vAC15,                                                   // DwgVer
                        OdDb::kMRelease0,                                              // MaintVer
                        OdDbProxyEntity::kAllButCloningAllowed,                              // NProxyFlags
                        EXCUSTENTITY,                                                  // DxfName
                        ExCustObjs|Description: Teigha Run-time Extension Example)     // AppName



const OdInt16 ExCustEntity::lastKnownVersion = 1;

ExCustEntity::ExCustEntity()
  : m_nCount(4)
{
  setRadius(1.);
}

ExCustEntity::~ExCustEntity()
{
}

OdInt16 ExCustEntity::getVersion()
{
  return lastKnownVersion;
}
/************************************************************************/
/* Properties unique to ExCustEntity                                    */
/************************************************************************/
void ExCustEntity::setCount(OdInt16 n)
{
  assertWriteEnabled();
  m_nCount = n;
}

OdInt16 ExCustEntity::getCount() const
{
  assertReadEnabled();
  return m_nCount;
}


/************************************************************************/
/* Read the DWG format data of this object from the specified file.     */
/************************************************************************/
OdResult ExCustEntity::dwgInFields(OdDbDwgFiler* pFiler)
{
  /**********************************************************************/
  /* Read the fields common to OdDbCircle                               */
  /**********************************************************************/
  OdResult res = OdDbCircle::dwgInFields(pFiler);
  if (res != eOk)
  {
    return res;
  }
  /**********************************************************************/
  /* Read the version of the ExCustEntity, and indicate create if a     */
  /* proxy is required.                                                 */
  /**********************************************************************/
  int nVersion = pFiler->rdInt16();
  if (nVersion > lastKnownVersion)
  {
    return eMakeMeProxy;
  }

  /**********************************************************************/
  /* Read and set the remaining properties unique to ExCustEntity.      */
  /**********************************************************************/
  m_nCount = pFiler->rdInt16();
  return eOk;
}

/************************************************************************/
/* Write the DWG format data of this object to the specified file.      */
/************************************************************************/
void ExCustEntity::dwgOutFields(OdDbDwgFiler* pFiler) const
{
  /**********************************************************************/
  /* Write the fields common to OdDbCircle                              */
  /**********************************************************************/
  OdDbCircle::dwgOutFields(pFiler);
  /**********************************************************************/
  /* Write the properties common to ExCustEntity.                       */
  /**********************************************************************/
  pFiler->wrInt16(getVersion());
  pFiler->wrInt16(m_nCount);
}

OdResult ExCustEntity::dxfInFields(OdDbDxfFiler* pFiler)
{
  /**********************************************************************/
  /* Read the fields common to OdDbCircle                               */
  /**********************************************************************/
  OdResult res = OdDbCircle::dxfInFields(pFiler);
  if (res != eOk)
    return res;
  if (!pFiler->atSubclassData(desc()->name()))
    return eBadDxfSequence;

  /**********************************************************************/
  /* Read and Set the remaining properties unique to ExCustEntity.      */
  /**********************************************************************/
  while(!pFiler->atEOF())
  {
    int nCode = pFiler->nextItem();
    switch (nCode)
    {
    case 70:
      {
      int nVersion = pFiler->rdInt16();
      if (nVersion > lastKnownVersion)
      {
        return eMakeMeProxy;
      }
      }
      break;
    case 71:
      m_nCount = pFiler->rdInt16();
      break;
    }
  }
  return eOk;
}

/************************************************************************/
/* Write the DXF format data of this object to the specified file.      */
/************************************************************************/
void ExCustEntity::dxfOutFields(OdDbDxfFiler* pFiler) const
{
  /**********************************************************************/
  /* Write the fields common to OdDbCircle  .                           */
  /**********************************************************************/
  OdDbCircle::dxfOutFields(pFiler);
  /**********************************************************************/
  /* Write the properties unique to ExCustEntity.                       */
  /**********************************************************************/
  pFiler->wrSubclassMarker(desc()->name());
  pFiler->wrInt16(70, getVersion());
  pFiler->wrInt16(71, m_nCount);
}

/************************************************************************/
/* Creates a viewport-independent geometric representation of the       */
/* ExCustEntity.                                                        */
/************************************************************************/
bool ExCustEntity::subWorldDraw(OdGiWorldDraw * pWd) const
{
  assertReadEnabled();
  OdGePoint3d ptCenter(center());
  OdGeVector3d vNormal(normal());
  double dRadius = radius();
  int nCount = m_nCount;

  /**********************************************************************/
  /* Override dRadius and nCount from block reference(s) with           */
  /* attributes with tags of "RADIUS" and "COUNT" respectively.         */
  /**********************************************************************/  
  const OdGiPathNode* pPath = pWd->geometry().currentGiPath();
  if(pPath)
  {
    while(pPath)
    {
      /******************************************************************/
      /* Is this a block reference?                                     */
      /******************************************************************/
      OdDbBlockReferencePtr pInsert = OdDbBlockReference::cast(OdDbObjectId(pPath->persistentDrawableId()).openObject());
      if(pInsert.get())
      {
        OdDbObjectIteratorPtr pIter = pInsert->attributeIterator();
        /****************************************************************/
        /* Iterate through the associated attributes                    */
        /****************************************************************/
        while(!pIter->done())
        {
          OdDbAttributePtr pAttrib = pIter->objectId().safeOpenObject();
          if(pAttrib->tag()==OD_T("RADIUS"))
          {
            dRadius = odStrToD(pAttrib->textString());
          }
          else if(pAttrib->tag()==OD_T("COUNT"))
          {
            nCount = odStrToInt(pAttrib->textString());
          }
          pIter->step();
        }
        break;
      }
      pPath = pPath->parent();
    }
  }

  /**********************************************************************/
  /* Create a hatch entity                                              */
  /**********************************************************************/
  OdDbHatchPtr pHatch = OdDbHatch::createObject();
  pHatch->setPattern(OdDbHatch::kPreDefined, OD_T("SOLID"));
  pHatch->setHatchStyle(OdDbHatch::kNormal);

  /**********************************************************************/
  /* Inherit the properties of this ExCustEntity                        */
  /**********************************************************************/
  pHatch->setPropertiesFrom(this);
  pHatch->setAssociative(false);
  pHatch->setNormal(vNormal);
  pHatch->setElevation(ptCenter.z);

  EdgeArray edgePtrs;

  if (nCount <= 1)
  {
    /********************************************************************/
    /* Vectorize the circle                                             */
    /********************************************************************/  
    pWd->geometry().circle(ptCenter, dRadius, vNormal);
    OdGeCircArc2d *cirArc = new OdGeCircArc2d(OdGePoint2d(ptCenter.x, ptCenter.y), dRadius);
    /********************************************************************/
    /* Use the circle as a hatch boundary                               */
    /********************************************************************/  
    edgePtrs.append(cirArc);
    pHatch->appendLoop(OdDbHatch::kDefault, edgePtrs);
  }
  else
  {
    OdGeVector3d vDisp(radius(), 0., 0.);
    double step = Oda2PI / nCount;
    while (nCount--)
    {
      OdGePoint3d ptC(ptCenter + vDisp);
      /******************************************************************/
      /* Vectorize a circle                                             */
      /******************************************************************/  
      pWd->geometry().circle(ptC, dRadius, vNormal);
      vDisp.rotateBy(step, vNormal);
      /******************************************************************/
      /* Use the circle as a hatch boundary                             */
      /******************************************************************/  
      OdGeCircArc2d *cirArc = new OdGeCircArc2d(OdGePoint2d(ptC.x, ptC.y), dRadius);
      edgePtrs.resize(0);
      edgePtrs.append(cirArc);
      pHatch->appendLoop(OdDbHatch::kDefault, edgePtrs);
    }
  }
  pHatch->worldDraw(pWd);
  return true;
}
/************************************************************************/
                          
/************************************************************************/

void _ExCreateCustomEntity_func(OdEdCommandContext* pCmdCtx)
{
    /**********************************************************************/
    /* Establish command context, database, and UserIO                    */
    /**********************************************************************/
    OdDbCommandContextPtr pDbCmdCtx(pCmdCtx);
    OdDbDatabasePtr pDb = pDbCmdCtx->database();
    OdDbUserIO* pIO = (OdDbUserIO*)pDbCmdCtx->userIO();

    /**********************************************************************/
    /* Open Model Space for writing                                       */
    /**********************************************************************/
    OdDbBlockTableRecordPtr pMS = pDb->getModelSpaceId().openObject(OdDb::kForWrite);

    /**********************************************************************/
    /* Create the ExCustEntity                                            */
    /**********************************************************************/
    ExCustEntityPtr pMyEnt = ExCustEntity::createObject();

    /**********************************************************************/
    /* Set the default properties common all entities                     */
    /**********************************************************************/
    pMyEnt->setDatabaseDefaults(pDb);

    /**********************************************************************/
    /* Set the center of the ExCustEntity                                 */
    /**********************************************************************/
    pMyEnt->setCenter(pIO->getPoint(OD_T("Enter circle center:"), true));

    /**********************************************************************/
    /* Define a RadiusTracker to drag the radius                          */
    /**********************************************************************/
    struct RadiusTracker : OdStaticRxObject<OdEdRealTracker>
    {
        OdDbCirclePtr _ent;
        RadiusTracker(const OdGePoint3d& center, OdDbDatabase* pDb)
        {
            /******************************************************************/
            /* Use a circle to drag the radius.                               */
            /******************************************************************/
            _ent = OdDbCircle::createObject();
            _ent->setDatabaseDefaults(pDb);
            _ent->setCenter(center);
        }

        /********************************************************************/
        /* Set the radius of the circle to the tracked real value           */
        /********************************************************************/
        virtual void setValue(double r) { _ent->setRadius(r); }

        /********************************************************************/
        /* Add the dragged circle to the view                               */
        /********************************************************************/
        virtual int addDrawables(OdGsView* pView)
        {
            pView->add(_ent, 0);
            return 1;
        }

        /********************************************************************/
        /* Remove the dragged circle from the view                          */
        /********************************************************************/
        virtual void removeDrawables(OdGsView* pView)
        {
            pView->erase(_ent);
        }
    }
    /**********************************************************************/
    /* Initialize the tracker                                             */
    /**********************************************************************/
    tracker(pMyEnt->center(), pDb);

    /**********************************************************************/
    /* Get the radius                                                     */
    /**********************************************************************/
    pMyEnt->setRadius(pIO->getDist(OD_T("\nSpecify radius of circle: "), OdEd::kGdsFromLastPoint, 0.0, OdString::kEmpty, &tracker));

    /**********************************************************************/
    /* Get the count                                                     */
    /**********************************************************************/
    pMyEnt->setCount((OdInt16)pIO->getInt(OD_T("\nSpecify number of circle circles: ")));

    /**********************************************************************/
    /* Add the ExCustEntity to Model Space                                */
    /**********************************************************************/
    pMS->appendOdDbEntity(pMyEnt);
}

#if defined(_MSC_VER) && defined(_TOOLKIT_IN_DLL_)
class __declspec(uuid("8D0E4AE8-D8B1-4C26-92E9-264D1C9D5C79")) CExCustEntityWrapper;
#endif

OdResult ExCustEntity::subGetClassID(void* pClsid) const
{
#if defined(_MSC_VER) && defined(_TOOLKIT_IN_DLL_)
    * (GUID*)pClsid = __uuidof(CExCustEntityWrapper);
    return eOk;
#else
    return eNotImplemented;
#endif
}

//void _Ex1_func(OdEdCommandContext* pCmdCtx)
//{


    //OdDbCommandContextPtr pDbCmdCtx(pCmdCtx);
    //OdDbDatabasePtr pDb = pDbCmdCtx->database();
    //OdDbUserIO* pIO = (OdDbUserIO*)pDbCmdCtx->userIO();

    //
    //OdDbBlockTableRecordPtr pMS = pDb->getModelSpaceId().openObject(OdDb::kForWrite);

   
    //ExStarPtr pMyEnt = ExStar::createObject();

    //
    //pMyEnt->setDatabaseDefaults(pDb);

    //pMyEnt->setCenter(pIO->getPoint(OD_T("Enter star center:"), true));


    //struct RadiusTracker : OdStaticRxObject<OdEdRealTracker>
    //{
    //    OdDbCirclePtr _ent;
    //    RadiusTracker(const OdGePoint3d& center, OdDbDatabase* pDb)
    //    {
    //        /* Use a circle to drag the radius.  */
    //        _ent = OdDbCircle::createObject();
    //        _ent->setDatabaseDefaults(pDb);
    //        _ent->setCenter(center);

    //    }
    //    /* Set the radius of the circle to the tracked real value */
    //    virtual void setValue(double r) { _ent->setRadius(r); }

    //    /* Add the dragged circle to the view */
    //    virtual int addDrawables(OdGsView* pView)
    //    {
    //        pView->add(_ent, 0);
    //        return 1;
    //    }
    //    /* Remove the dragged circle from the view  */
    //    virtual void removeDrawables(OdGsView* pView)
    //    {
    //        pView->erase(_ent);
    //    }
    //}

    ///* Initialize the tracker */

    //tracker(pMyEnt->center(), pDb);
  
    //double rawRadius = pIO->getDist(OD_T("\nSpecify radius of circle: "), OdEd::kGdsFromLastPoint, 0.0, OdString::kEmpty, &tracker);
    //pMyEnt->setInsideRadius(rawRadius* 0.4);
    //pMyEnt->setOutsideRadius(rawRadius);

    //

    //pMS->appendOdDbEntity(pMyEnt);
//}

void _Ex1_func(OdEdCommandContext* pCmdCtx)
{
    OdDbCommandContextPtr pDbCmdCtx(pCmdCtx);
    OdDbDatabasePtr pDb = pDbCmdCtx->database();
    OdDbUserIO* pIO = (OdDbUserIO*)pDbCmdCtx->userIO();


    OdDbBlockTableRecordPtr pMS = pDb->getModelSpaceId().openObject(OdDb::kForWrite);

    ExStarPtr pMyEnt = ExStar::createObject();


    pMyEnt->setDatabaseDefaults(pDb);

    pMyEnt->setCenter(pIO->getPoint(OD_T("Enter star center:"), true));


    struct RadiusTracker : OdStaticRxObject<OdEdRealTracker>
    {
        OdDbCirclePtr _ent;
        RadiusTracker(const OdGePoint3d& center, OdDbDatabase* pDb)
        {
            _ent = OdDbCircle::createObject();
            _ent->setDatabaseDefaults(pDb);
            _ent->setCenter(center);

        }

        virtual void setValue(double r) { _ent->setRadius(r); }

        virtual int addDrawables(OdGsView* pView)
        {
            pView->add(_ent, 0);
            return 1;
        }

        virtual void removeDrawables(OdGsView* pView)
        {
            pView->erase(_ent);
        }
    }



    tracker(pMyEnt->center(), pDb);

    double rawRadius = pIO->getDist(OD_T("\nSpecify radius of circle: "), OdEd::kGdsFromLastPoint, 0.0, OdString::kEmpty, &tracker);
    pMyEnt->setInsideRadius(rawRadius * 0.4);
    pMyEnt->setOutsideRadius(rawRadius);

    //сюди дельта трекер

    pMS->appendOdDbEntity(pMyEnt);
}


ODRX_DXF_DEFINE_MEMBERS(ExStar,									// class name
    OdDbEntity,									// parent class name
    DBOBJECT_CONSTR,							// creation macro
    OdDb::kDHL_CURRENT,                         // dwg version
    OdDb::kMReleaseCurrent,                     // maintenance release version
    OdDbProxyEntity::kTransformAllowed |
    OdDbProxyEntity::kColorChangeAllowed |
    OdDbProxyEntity::kLayerChangeAllowed,       // proxy flags
    EXSTAR,									// DXF name
    ExStarDb | Description: Teigha Run - time Extension Example)	// Application name




    ExStar::ExStar() : m_globalDelta(0.), m_insideDelta(OdaPI / 6.)
{
    m_insideCircle.setRadius(1.);
    m_outsideCircle.setRadius(2.5);

    m_backcolor.setRGB(0, 0, 0);
    recordGraphicsModified();
}

ExStar::~ExStar() {}		// Destructor of smiley object


bool ExStar::subWorldDraw(OdGiWorldDraw* wd) const			// Drawing function
{
    assertReadEnabled();


    OdGePoint3d ptCenter(center());
    /*OdGeVector3d vNormal(normal());*/

    double iRadius = getInsideRadius();
    double oRadius = getOutsideRadius();

    double iDelta = getInsideDelta();
    double gDelta = getGlobalDelta();


    // If dragging, don't fill
    if (wd->isDragging()) {
        wd->subEntityTraits().setTrueColor(entityColor());  // Returns a 'True' true color.
        wd->subEntityTraits().setFillType(kOdGiFillNever);
    }
    else
        wd->subEntityTraits().setFillType(kOdGiFillAlways);


    //мальовка зірки


    // Calculate points of the star
    const int numPoints = 5;
    OdGePoint3dArray starPoints;
    double angleStep = OdaPI / numPoints; // Angle between points

    for (int i = 0; i < numPoints * 2; i++) {

        double angle = i * angleStep + gDelta;
        double radius = (i % 2 == 0) ? oRadius : iRadius;
        double delta = (i % 2 == 0) ? 0 : iDelta;
        double x = ptCenter.x + radius * cos(angle + delta);
        double y = ptCenter.y + radius * sin(angle + delta);

        starPoints.append(OdGePoint3d(x, y, ptCenter.z));
    }

    // Draw the star as a polyline


    //заповнення кольором!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


    wd->geometry().polyline(starPoints.length(), starPoints.asArrayPtr());

    if (!wd->isDragging())
        wd->subEntityTraits().setTrueColor(m_backcolor);

    return true;
}

void ExStar::dwgOutFields(OdDbDwgFiler* filer) const
{
    assertReadEnabled();
    OdDbEntity::dwgOutFields(filer);

    filer->wrDouble(kCurrentVersionNumber);
    filer->wrPoint3d(center());

    filer->wrDouble(getOutsideRadius());
    filer->wrDouble(getInsideRadius());

    filer->wrDouble(getGlobalDelta());
    filer->wrDouble(getInsideDelta());

    filer->wrInt32(backColor().color());
}

OdResult ExStar::dwgInFields(OdDbDwgFiler* filer)
{
    assertWriteEnabled();
    OdDbEntity::dwgInFields(filer);

    // Read version number
    if (filer->rdDouble() > kCurrentVersionNumber) return eMakeMeProxy;

    // Read data
    setCenter(filer->rdPoint3d());

    setOutsideRadius(filer->rdDouble());
    setInsideRadius(filer->rdDouble());

    setGlobalDelta(filer->rdDouble());
    setInsideDelta(filer->rdDouble());

    m_backcolor.setColor(filer->rdInt32());

    recordGraphicsModified();
    return eOk;

}

void ExStar::dxfOutFields(OdDbDxfFiler* filer) const
{
    assertReadEnabled();
    OdDbEntity::dxfOutFields(filer);

    filer->wrSubclassMarker(desc()->name());
    filer->wrDouble(OdResBuf::kDxfReal, kCurrentVersionNumber);
    filer->wrPoint3d(OdResBuf::kDxfXCoord, center());

    filer->wrDouble(OdResBuf::kDxfReal + 1, getOutsideRadius());
    filer->wrDouble(OdResBuf::kDxfReal + 2, getInsideRadius());

    filer->wrDouble(OdResBuf::kDxfReal + 3, getGlobalDelta());
    filer->wrDouble(OdResBuf::kDxfReal + 4, getInsideDelta());


    filer->wrInt32(OdResBuf::kRtColor, backColor().color());
}

OdResult ExStar::dxfInFields(OdDbDxfFiler* filer)
{
    assertWriteEnabled();
    OdResult es;
    OdGePoint3d center;

    if (eOk != (es = OdDbEntity::dxfInFields(filer))) return es;

    // Check that we are at the correct subclass data
    if (!filer->atSubclassData(desc()->name())) return eBadDxfSequence;

    // Read version number (must be first)
    if (filer->nextItem() != OdResBuf::kDxfReal) return eMakeMeProxy;
    if (filer->rdDouble() > kCurrentVersionNumber) return eMakeMeProxy;

    // Read of data
    while (!filer->atEOF()) {
        switch (filer->nextItem()) {
        case OdResBuf::kDxfXCoord:
            filer->rdPoint3d(center);
            setCenter(center);
            break;
        case OdResBuf::kDxfReal + 1:
            setOutsideRadius(filer->rdDouble());
            break;
        case OdResBuf::kDxfReal + 2:
            setOutsideRadius(filer->rdDouble());
            break;
        case OdResBuf::kDxfReal + 3:
            setGlobalDelta(filer->rdDouble());
            break;
        case OdResBuf::kDxfReal + 4:
            setInsideDelta(filer->rdDouble());
            break;
        case OdResBuf::kRtColor:
            m_backcolor.setColor(filer->rdInt32());
            break;
        }
    }
    recordGraphicsModified();
    return eOk;
}

//void ExStar::composeForLoad(OdDb::SaveType format, OdDb::DwgVersion version, OdDbAuditInfo* pAuditInfo)
//{
//    OdDbEntity::composeForLoad(format, version, pAuditInfo);
//    if (pAuditInfo)
//    {
//        pAuditInfo->printError(this, L"Am I alive?", L"I think I'm OK", L"This should be the case");  // Just for testing
//    }
//}


//OdResult ExStar::subExplode(OdGePoint3dArray& pointSet) const
//{
//    assertReadEnabled();
//
//    const int numPoints = 5;
//    OdGePoint3dArray starPoints;
//    double angleStep = OdaPI / numPoints; // Angle between points
//
//    OdGePoint3d center(center());
//
//    for (int i = 0; i < numPoints * 2; i++) {
//
//        double angle = i * angleStep + getGlobalDelta();
//        double radius = (i % 2 == 0) ? getOutsideRadius() : getInsideRadius();
//        double delta = (i % 2 == 0) ? 0 : getInsideDelta();
//        double x = center.x + radius * cos(angle + delta);
//        double y = center.y + radius * sin(angle + delta);
//
//        pointSet.append(OdGePoint3d(x, y, center.z));
//    }
//
//    return eOk;
//}

OdCmEntityColor ExStar::backColor()	const		// get back color of smiley
{
    assertReadEnabled();
    return m_backcolor;
}
// set back color of smiley
void ExStar::setBackColor(const OdCmEntityColor& color)
{
    assertWriteEnabled();
    m_backcolor = color;
}

//OdGeVector3d ExStar::normal() const
//{
//    assertReadEnabled();
//    return m_normal;
//}
//
//void ExStar::setNormal(const OdGeVector3d v)
//{
//    assertWriteEnabled();
//    m_normal = v;
//    recordGraphicsModified();
//}


OdGePoint3d ExStar::center() const
{
    assertReadEnabled();
    return m_insideCircle.center();
}

void ExStar::setCenter(const OdGePoint3d c)
{
    assertWriteEnabled();
    m_insideCircle.setCenter(c);
    m_outsideCircle.setCenter(c);
    recordGraphicsModified();
}

void ExStar::setInsideRadius(double r)
{
    assertWriteEnabled();
    m_insideCircle.setRadius(r);
}

void ExStar::setOutsideRadius(double r)
{
    assertWriteEnabled();
    m_outsideCircle.setRadius(r);
}

void ExStar::setInsideDelta(double d)
{
    assertWriteEnabled();
    m_insideDelta = d;
}

void ExStar::setGlobalDelta(double d)
{
    assertWriteEnabled();
    m_globalDelta = d;
}

double ExStar::getInsideRadius() const
{
    assertReadEnabled();
    return m_insideCircle.radius();
}

double ExStar::getOutsideRadius() const
{
    assertReadEnabled();
    return m_outsideCircle.radius();
}

double ExStar::getInsideDelta() const
{
    assertReadEnabled();
    return m_insideDelta;
}

double ExStar::getGlobalDelta() const
{
    assertReadEnabled();
    return m_globalDelta;
}


//
// Step 3
// We are implementing 'transformBy' function to move, to scale and  
// to rotate the smiley object. To stretch and constrict of smiley, 
// we have implemented the 'scaleRadius' function. This function will
// being used in following steps.
//
OdResult ExStar::subTransformBy(const OdGeMatrix3d& xform)
{
    assertWriteEnabled();

    xDataTransformBy(xform);

    return eOk;
}

void ExStar::scaleRadius(const double r)     // stretch and constrict of smiley
{
    assertWriteEnabled();

    recordGraphicsModified();
}
//
// Step 4
// We are implementing 'getGripPoints' function to create the array of 
// grip points for smiley object and are implementing 'moveGripPointsAt' 
// function to stretch and to move the face, the mouth and eyes of 
// smiley object using a grip points. To control the size of smiley at 
// changes of its elements, we have implemented the 'ensureRadius' functions.
//

OdResult ExStar::subGetGripPoints(OdGePoint3dArray& gripPoints) const
{
    assertReadEnabled();

    return eOk;
}

OdResult ExStar::subMoveGripPointsAt(const OdGePoint3dArray& gripPoints,
    const OdIntArray& indices)
{
    assertWriteEnabled();

    return eOk;
}


//
// Step 5
// We are implementing 'getOsnapPoints' function to create the array of 
// snap points for smiley object. Snap points are being used for joining of
// smiley object with other objects at drawing. At testing, appropriate snap
// mode must be ON.
//
OdResult ExStar::subGetOsnapPoints(OdDb::OsnapMode osnapMode,
    OdGsMarker   /*gsSelectionMark*/,
    const OdGePoint3d& /*pickPoint*/,
    const OdGePoint3d& /*lastPoint*/,
    const OdGeMatrix3d& /*xfm*/,
    const OdGeMatrix3d& /*ucs*/,
    OdGePoint3dArray& snapPoints) const
{
    assertReadEnabled();

    return eOk;

}

//==================================================================================================//

OdResult ExStar::subGetGsMarkersAtSubentPath(const OdDbFullSubentPath& subPath,
    OdGsMarkerArray& gsMarkers) const
{
    assertReadEnabled();

    return eOk;
}

//==================================================================================================//

OdResult ExStar::subGetSubentPathsAtGsMarker(OdDb::SubentType  type,
    OdGsMarker  gsMark,
    const OdGePoint3d& /* pickPoint */,
    const OdGeMatrix3d& /* xfm */,
    OdDbFullSubentPathArray& subentPaths,
    const OdDbObjectIdArray* /* pEntAndInsertStack */) const
{
    assertReadEnabled();

    return eOk;
}

//==================================================================================================//

OdResult ExStar::subGetSubentClassId(const OdDbFullSubentPath& path, void* clsId) const
{
    assertReadEnabled();

    OdDbEntityPtr pEnt = subSubentPtr(path);

    if (!pEnt.isNull())
    {
        pEnt->getClassID(clsId);
        return eOk;
    }

    return eInvalidInput;
}

//==================================================================================================//

OdResult ExStar::subGetSubentPathGeomExtents(const OdDbFullSubentPath& path, OdGeExtents3d& extents) const
{
    assertReadEnabled();

    OdDbEntityPtr pEnt = subSubentPtr(path);

    if (!pEnt.isNull())
    {
        pEnt->getGeomExtents(extents);
        return eOk;
    }

    return eInvalidInput;
}

//==================================================================================================//

OdDbEntityPtr ExStar::subSubentPtr(const OdDbFullSubentPath& id) const
{
    assertReadEnabled();

    return OdDbEntityPtr();
}

//==================================================================================================//

OdResult ExStar::subGetGripPointsAtSubentPath(
    const OdDbFullSubentPath& path,
    OdDbGripDataPtrArray& grips,
    const double curViewUnitSize,
    const int gripSize,
    const OdGeVector3d& curViewDir,
    const OdUInt32 bitflags) const
{
    assertReadEnabled();


    return eOk;
}

//==================================================================================================//

OdResult ExStar::subMoveGripPointsAtSubentPaths(
    const OdDbFullSubentPathArray& paths,
    const OdDbVoidPtrArray& gripAppData,
    const OdGeVector3d& offset,
    const OdUInt32 bitflags)
{
    assertWriteEnabled();


    return eOk;
}

//==================================================================================================//

OdResult ExStar::subMoveGripPointsAt(const OdIntArray& indices, const OdGeVector3d& offset)
{
    assertWriteEnabled();


    return eOk;
}

//==================================================================================================//

OdResult ExStar::subTransformSubentPathsBy(const OdDbFullSubentPathArray& paths, const OdGeMatrix3d& xform)
{
    assertWriteEnabled();


    return eOk;
}