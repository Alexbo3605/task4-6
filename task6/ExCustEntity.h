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

// ExCustEntity.h: interface for the ExCustEntity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(EXCUSTENTITY_INCLUDED)
#define EXCUSTENTITY_INCLUDED

#include "ExCustObjExport.h"
#include "DbCircle.h"
#include "Ge/GeCircArc3d.h"

class EXCUSTOBJEXPORT ExCustEntity : public OdDbCircle
{
  static const OdInt16 lastKnownVersion;
  OdInt16 m_nCount;
public:
  // Macro to declare
  ODDB_DECLARE_MEMBERS(ExCustEntity);

  ExCustEntity();
  virtual ~ExCustEntity();

  static OdInt16 getVersion();

  void setCount(OdInt16 n);
  OdInt16 getCount() const;

  // Methods to override
  OdResult dwgInFields(OdDbDwgFiler* pFiler);
  void dwgOutFields(OdDbDwgFiler* pFiler) const;
  OdResult dxfInFields(OdDbDxfFiler* pFiler);
  void dxfOutFields(OdDbDxfFiler* pFiler) const;
  bool subWorldDraw(OdGiWorldDraw * pWd) const;
  virtual OdResult subGetClassID(
    void* pClsid) const;
};

typedef OdSmartPtr<ExCustEntity> ExCustEntityPtr;

class ExStar : public OdDbEntity
{
private:


    OdGeCircArc3d   m_insideCircle;        // внутрішнє коло
    OdGeCircArc3d   m_outsideCircle;       // зовнішнє коло

    double m_insideDelta;          //зміщення точок внутрішнього кола
    double m_globalDelta;          // зміщення всіх точок кіл

    OdCmEntityColor m_backcolor;


public:


    ODDB_DECLARE_MEMBERS(ExStar);	// Declare default functions. It is necessary for 
    // all Teigha database objects.
    ExStar();
    virtual ~ExStar();
    // Step 1
    bool subWorldDraw(OdGiWorldDraw* mode) const;			// for drawing 
    OdResult dwgInFields(OdDbDwgFiler* pFiler);			// for loading from DWG file
    void dwgOutFields(OdDbDwgFiler* pFiler) const;		// for saving in DWG file
    // Step 2
    OdResult dxfInFields(OdDbDxfFiler* filer);			// for loading from DXF file
    void dxfOutFields(OdDbDxfFiler* filer) const;		// for saving in DXF file
    // Step 3
    OdResult subTransformBy(const OdGeMatrix3d& xform);	// for moving and rotating 
    void scaleRadius(const double r);					// for scalling

    // Step 4
    //virtual OdResult subGetGripPoints(OdGePoint3dArray& gripPoints) const;
    //OdResult subMoveGripPointsAt(const OdGePoint3dArray& gripPoints, const OdIntArray& indices);
    //virtual OdResult subMoveGripPointsAt(const OdIntArray& indices, const OdGeVector3d& offset);
    //// Step 5
    //virtual OdResult subGetOsnapPoints(OdDb::OsnapMode osnapMode, OdGsMarker gsSelectionMark,
    //    const OdGePoint3d& pickPoint, const OdGePoint3d& lastPoint,
    //    const OdGeMatrix3d& xfm, const OdGeMatrix3d& ucs,
    //    OdGePoint3dArray& snapPoints) const;
    //// Step 6
    //virtual OdResult subExplode(OdGePoint3dArray& pointSet) const;



    //
    // Star property functions
    //

    virtual OdCmEntityColor backColor()	const;		// back color
    virtual void setBackColor(const OdCmEntityColor& color);


    //virtual OdGeVector3d normal() const;			// normal
    //virtual void setNormal(const OdGeVector3d v);

    virtual OdGePoint3d center() const;
    virtual void setCenter(const OdGePoint3d c);

    virtual void setInsideRadius(double radius);
    virtual void setOutsideRadius(double radius);

    virtual void setInsideDelta(double delta);
    virtual void setGlobalDelta(double delta);

    virtual double getInsideRadius() const;
    virtual double getOutsideRadius() const;

    virtual double getInsideDelta() const;
    virtual double getGlobalDelta() const;



    // Subentity functions
    virtual OdResult subGetGsMarkersAtSubentPath(const OdDbFullSubentPath& subPath, OdGsMarkerArray& gsMarkers) const;

    virtual OdResult subGetSubentPathsAtGsMarker(OdDb::SubentType type, OdGsMarker gsMark,
        const OdGePoint3d& pickPoint, const OdGeMatrix3d& xfm,
        OdDbFullSubentPathArray& subentPaths,
        const OdDbObjectIdArray* pEntAndInsertStack = 0) const;

    virtual OdResult subGetSubentClassId(const OdDbFullSubentPath& path, void* clsId) const;

    virtual OdResult subGetSubentPathGeomExtents(const OdDbFullSubentPath& path, OdGeExtents3d& extents) const;

    virtual OdDbEntityPtr subSubentPtr(const OdDbFullSubentPath& id) const;

    virtual OdResult subGetGripPointsAtSubentPath(const OdDbFullSubentPath& path, OdDbGripDataPtrArray& grips,
        const double curViewUnitSize, const int gripSize,
        const OdGeVector3d& curViewDir, const OdUInt32 bitflags) const;

    virtual OdResult subMoveGripPointsAtSubentPaths(const OdDbFullSubentPathArray& paths, const OdDbVoidPtrArray& gripAppData,
        const OdGeVector3d& offset, const OdUInt32 bitflags);

    virtual OdResult subTransformSubentPathsBy(const OdDbFullSubentPathArray& paths, const OdGeMatrix3d& xform);

    virtual void composeForLoad(OdDb::SaveType format, OdDb::DwgVersion version, OdDbAuditInfo* pAuditInfo);

    
};

typedef OdSmartPtr<ExStar> ExStarPtr;

#endif
