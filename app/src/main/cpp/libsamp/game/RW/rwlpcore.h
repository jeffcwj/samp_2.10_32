#pragma once

/******************************************/
/*                                        */
/*    RenderWare(TM) Graphics Library     */
/*                                        */
/******************************************/

/*
 * This file is a product of Criterion Software Ltd.
 *
 * This file is provided as is with no warranties of any kind and is
 * provided without any obligation on Criterion Software Ltd.
 * or Canon Inc. to assist in its use or modification.
 *
 * Criterion Software Ltd. and Canon Inc. will not, under any
 * circumstances, be liable for any lost revenue or other damages
 * arising from the use of this file.
 *
 * Copyright (c) 1999. Criterion Software Ltd.
 * All Rights Reserved.
 */

/*************************************************************************
 *
 * Filename: <C:/daily/rwsdk/include/d3d9/rwplcore.h>
 * Automatically Generated on: Thu Feb 12 13:01:33 2004
 *
 ************************************************************************/

#define RWFORCEENUMSIZEINT ((RwInt32)((~((RwUInt32)0))>>1))


typedef long RwFixed;
typedef int  RwInt32;
typedef unsigned int RwUInt32;
typedef short RwInt16;
typedef unsigned short RwUInt16;
typedef unsigned char RwUInt8;
typedef signed char RwInt8;

typedef char RwChar;
typedef float RwReal;
typedef RwInt32 RwBool;

/* Limits of types */
#define RwInt32MAXVAL       0x7FFFFFFF
#define RwInt32MINVAL       0x80000000
#define RwUInt32MAXVAL      0xFFFFFFFF
#define RwUInt32MINVAL      0x00000000
#define RwRealMAXVAL        (RwReal)(3.40282347e+38)
#define RwRealMINVAL        (RwReal)(1.17549435e-38)
#define RwInt16MAXVAL       0x7FFF
#define RwInt16MINVAL       0x8000
#define RwUInt16MAXVAL      0xFFFF
#define RwUInt16MINVAL      0x0000

/*****************/

/* Complex types */

/*****************/


typedef struct RwV2d RwV2d;
/**
 * \ingroup rwv2d
 * \struct RwV2d
 * This type represents points in a 2D space, such as device
 * space, specified by the (x, y) coordinates of the point.
 */
struct RwV2d
{
    RwReal x;   /**< X value*/
    RwReal y;   /**< Y value */
};

typedef struct RwV3d RwV3d;
/**
 * \ingroup rwv3d
 * \struct RwV3d
 *  This type represents 3D points and vectors specified by
 * the (x, y, z) coordinates of a 3D point or the (x, y, z) components of a
 * 3D vector.
 */
struct RwV3d
{
    RwReal x;   /**< X value */
    RwReal y;   /**< Y value */
    RwReal z;   /**< Z value */
};

typedef struct RwRect RwRect;
/**
 * \ingroup geometricaltypes
 * \struct RwRect
 * This type represents a 2D device space rectangle specified
 * by the position of the top-left corner (the offset x, y) and its width (w)
 * and height (h).
 */
struct RwRect
{
    RwInt32 x;  /**< X value of the top-left corner */
    RwInt32 y;  /**< Y value of the top-left corner */
    RwInt32 w;  /**< Width of the rectangle */
    RwInt32 h;  /**< Height of the rectangle */
};

typedef struct RwSphere RwSphere;
/**
 * \ingroup geometricaltypes
 * \struct RwSphere
 * This type represents a sphere specified by the position
 * of its center and its radius.
 */
struct RwSphere
{
    RwV3d center;   /**< Sphere center */
    RwReal radius;  /**< Sphere radius */
};

struct RwMemory
{
	RwUInt8* start; /**< Starting address. */
	RwUInt32    length; /**< Length in bytes. */
};

typedef struct RwLine RwLine;
/**
 * \ingroup geometricaltypes
 * \struct RwLine
 * This type represents a 3D line specified by the position
 * of its start and end points.
 */
struct RwLine
{
    RwV3d start;    /**< Line start */
    RwV3d end;      /**< Line end */
};

enum RwTextureCoordinateIndex
{
    rwNARWTEXTURECOORDINATEINDEX = 0,
    rwTEXTURECOORDINATEINDEX0,
    rwTEXTURECOORDINATEINDEX1,
    rwTEXTURECOORDINATEINDEX2,
    rwTEXTURECOORDINATEINDEX3,
    rwTEXTURECOORDINATEINDEX4,
    rwTEXTURECOORDINATEINDEX5,
    rwTEXTURECOORDINATEINDEX6,
    rwTEXTURECOORDINATEINDEX7,
    rwTEXTURECOORDINATEINDEXFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwTextureCoordinateIndex RwTextureCoordinateIndex;

/**
 * \ingroup rwstream
 * \ref RwStreamType
 * This type represents the different types of stream that
 * can be used.
 * See API section \ref rwstream
 */
enum RwStreamType
{
    rwNASTREAM = 0,     /**<Invalid stream type */
    rwSTREAMFILE,       /**<File */
    rwSTREAMFILENAME,   /**<File name */
    rwSTREAMMEMORY,     /**<Memory*/
    rwSTREAMCUSTOM,     /**<Custom */
    rwSTREAMTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwStreamType RwStreamType;

/**
 * \ingroup rwstream
 * \ref RwStreamAccessType
 * This type represents the options available for
 * accessing a stream when it is opened.
 * See API section \ref rwstream */
enum RwStreamAccessType
{
    rwNASTREAMACCESS = 0,   /**<Invalid stream access */
    rwSTREAMREAD,           /**<Read */
    rwSTREAMWRITE,          /**<Write */
    rwSTREAMAPPEND,         /**<Append */
    rwSTREAMACCESSTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwStreamAccessType RwStreamAccessType;

typedef struct RwTexCoords RwTexCoords;
/**
 * \ingroup fundtypesdatatypes
 * \struct RwTexCoords
 * This type represents the u and v texture
 * coordinates of a particular vertex.
 */
 struct RwTexCoords
{
    RwReal u;   /**< U value */
    RwReal v;   /**< V value */
};

typedef struct RwPlane RwPlane;
struct RwPlane
{
    RwV3d normal;    /**< Normal to the plane */
    RwReal distance; /**< Distance to plane from origin in normal direction*/
};

enum RwPlaneType
{
    rwXPLANE = 0, /* These are deliberately multiples of sizeof(RwReal) */
    rwYPLANE = 4,
    rwZPLANE = 8,
    rwPLANETYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwPlaneType RwPlaneType;

/****************************************************************************
 Defines
 */

/* Set true depth information (for fogging, eg) */
#define RwIm2DVertexSetCameraX(vert, camx)          /* Nothing */
#define RwIm2DVertexSetCameraY(vert, camy)          /* Nothing */
#define RwIm2DVertexSetCameraZ(vert, camz)          /* Nothing */

#define RwIm2DVertexSetRecipCameraZ(vert, recipz)   ((vert)->rhw = recipz)

#define RwIm2DVertexGetCameraX(vert)                (cause an error)
#define RwIm2DVertexGetCameraY(vert)                (cause an error)
#define RwIm2DVertexGetCameraZ(vert)                (cause an error)
#define RwIm2DVertexGetRecipCameraZ(vert)           ((vert)->rhw)

/* Set screen space coordinates in a device vertex */
#define RwIm2DVertexSetScreenX(vert, scrnx)         ((vert)->x = (scrnx))
#define RwIm2DVertexSetScreenY(vert, scrny)         ((vert)->y = (scrny))
#define RwIm2DVertexSetScreenZ(vert, scrnz)         ((vert)->z = (scrnz))
#define RwIm2DVertexGetScreenX(vert)                ((vert)->x)
#define RwIm2DVertexGetScreenY(vert)                ((vert)->y)
#define RwIm2DVertexGetScreenZ(vert)                ((vert)->z)

/* Set texture coordinates in a device vertex */
#define RwIm2DVertexSetU(vert, texU, recipz)        ((vert)->u = (texU))
#define RwIm2DVertexSetV(vert, texV, recipz)        ((vert)->v = (texV))
#define RwIm2DVertexGetU(vert)                      ((vert)->u)
#define RwIm2DVertexGetV(vert)                      ((vert)->v)

/* Modify the luminance stuff */
#define RwIm2DVertexSetRealRGBA(vert, red, green, blue, alpha)  \
    ((vert)->emissiveColor =                                    \
     (((RwFastRealToUInt32(alpha)) << 24) |                        \
      ((RwFastRealToUInt32(red)) << 16) |                          \
      ((RwFastRealToUInt32(green)) << 8) |                         \
      ((RwFastRealToUInt32(blue)))))

#define RwIm2DVertexSetIntRGBA(vert, red, green, blue, alpha)   \
    ((vert)->emissiveColor =                                    \
     ((((RwUInt32)(alpha)) << 24) |                             \
      (((RwUInt32)(red)) << 16) |                               \
      (((RwUInt32)(green)) << 8) |                              \
      (((RwUInt32)(blue)))))

#define RwIm2DVertexGetRed(vert)    \
    (((vert)->emissiveColor >> 16) & 0xFF)

#define RwIm2DVertexGetGreen(vert)  \
    (((vert)->emissiveColor >> 8) & 0xFF)

#define RwIm2DVertexGetBlue(vert)   \
    ((vert)->emissiveColor & 0xFF)

#define RwIm2DVertexGetAlpha(vert)  \
    (((vert)->emissiveColor >> 24) & 0xFF)

#define RwIm2DVertexCopyRGBA(dst, src)  \
    ((dst)->emissiveColor = (src)->emissiveColor)

#define RwV3dAssignMacro(_target, _source)                     \
    ( *(_target) = *(_source) )

/* Get components */
#define RwMatrixGetRight(m)    (&(m)->right)
#define RwMatrixGetUp(m)       (&(m)->up)
#define RwMatrixGetAt(m)       (&(m)->at)
#define RwMatrixGetPos(m)      (&(m)->pos)

#define RwV2dAssign(o, a)               RwV2dAssignMacro(o, a)
#define RwV2dAdd(o, a, b)               RwV2dAddMacro(o, a, b)
#define RwV2dSub(o, a, b)               RwV2dSubMacro(o, a, b)
#define RwV2dLineNormal(_o, _a, _b)     RwV2dLineNormalMacro(_o, _a, _b)
#define RwV2dScale(o, i, s)             RwV2dScaleMacro(o, i, s)
#define RwV2dDotProduct(a,b)            RwV2dDotProductMacro(a,b)
#define RwV2dPerp(o, a)                 RwV2dPerpMacro(o, a)
#define RwV3dAssign(o, a)               RwV3dAssignMacro(o, a)
#define RwV3dAdd(o, a, b)               RwV3dAddMacro(o, a, b)
#define RwV3dSub(o, a, b)               RwV3dSubMacro(o, a, b)
#define RwV3dScale(o, a, s)             RwV3dScaleMacro(o, a, s)
#define RwV3dIncrementScaled(o, a, s)   RwV3dIncrementScaledMacro(o, a, s)
#define RwV3dNegate(o, a)               RwV3dNegateMacro(o, a)
#define RwV3dDotProduct(a, b)           RwV3dDotProductMacro(a, b)
#define RwV3dCrossProduct(o, a, b)      RwV3dCrossProductMacro(o, a, b)
/****************************************************************************
 Global Types
 */
typedef struct RwD3D9Vertex RwD3D9Vertex;
/**
 * \ingroup rwcoredriverd3d9
 * \struct RwD3D9Vertex
 * D3D9 vertex structure definition for 2D geometry
 */
struct RwD3D9Vertex
{
    RwReal      x;              /**< Screen X */
    RwReal      y;              /**< Screen Y */
    RwReal      z;              /**< Screen Z */
    RwReal      rhw;            /**< Reciprocal of homogeneous W */

    RwUInt32    emissiveColor;  /**< Vertex color */

    RwReal      u;              /**< Texture coordinate U */
    RwReal      v;              /**< Texture coordinate V */
};

typedef RwD3D9Vertex    RwIm2DVertex;
typedef RwUInt32        RxVertexIndex;
typedef RxVertexIndex   RwImVertexIndex;

/****************************************************************************
 Defines
 */
 enum RwRenderState
{
    rwRENDERSTATENARENDERSTATE = 0,

    rwRENDERSTATETEXTURERASTER = 1,
        /**<Raster used for texturing (normally used in immediate mode).
         *  The value is a pointer to an \ref RwRaster.
         * Default: NULL.
         */
    rwRENDERSTATETEXTUREADDRESS = 2,
        /**<\ref RwTextureAddressMode: wrap, clamp, mirror or border.
         * Default: rwTEXTUREADDRESSWRAP.
         */
    rwRENDERSTATETEXTUREADDRESSU = 3,
        /**<\ref RwTextureAddressMode in u only.
         * Default: rwTEXTUREADDRESSWRAP.
         */
    rwRENDERSTATETEXTUREADDRESSV = 4,
        /**<\ref RwTextureAddressMode in v only.
         * Default: rwTEXTUREADDRESSWRAP.
         */
    rwRENDERSTATETEXTUREPERSPECTIVE = 5,
        /**<Perspective correction on/off (always enabled on many platforms).
         */
    rwRENDERSTATEZTESTENABLE = 6,
        /**<Z-buffer test on/off.
         * Default: TRUE.
         */
    rwRENDERSTATESHADEMODE = 7,
        /**<\ref RwShadeMode: flat or gouraud shading.
         * Default: rwSHADEMODEGOURAUD.
         */
    rwRENDERSTATEZWRITEENABLE = 8,
        /**<Z-buffer write on/off.
         * Default: TRUE.
         */
    rwRENDERSTATETEXTUREFILTER = 9,
        /**<\ref RwTextureFilterMode: point sample, bilinear, trilinear, etc.
         * Default: rwFILTERLINEAR.
         */
    rwRENDERSTATESRCBLEND = 10,
        /**<\ref RwBlendFunction used to modulate the source pixel color
         *  when blending to the frame buffer.
         * Default: rwBLENDSRCALPHA.
         */
    rwRENDERSTATEDESTBLEND = 11,
        /**<\ref RwBlendFunction used to modulate the destination pixel
         *  color in the frame buffer when blending. The resulting pixel
         *  color is given by the formula
         *  (SRCBLEND * srcColor + DESTBLEND * destColor) for each RGB
         *  component. For a particular platform, not all combinations
         *  of blend function are allowed (see platform specific
         *  restrictions).
         * Default: rwBLENDINVSRCALPHA.
         */
    rwRENDERSTATEVERTEXALPHAENABLE = 12,
        /**<Alpha blending on/off (always enabled on some platforms).
         *  This is normally used in immediate mode to enable alpha blending
         *  when vertex colors or texture rasters have transparency. Retained
         *  mode pipelines will usually set this state based on material colors
         *  and textures.
         * Default: FALSE.
         */
    rwRENDERSTATEBORDERCOLOR = 13,
        /**<Border color for \ref RwTextureAddressMode
         *  \ref rwTEXTUREADDRESSBORDER. The value should be a packed
         *  RwUInt32 in a platform specific format. The macro
         *  RWRGBALONG(r, g, b, a) may be used to construct this using
         *  8-bit color components.
         * Default: RWRGBALONG(0, 0, 0, 0).
         */
    rwRENDERSTATEFOGENABLE = 14,
        /**<Fogging on/off (all polygons will be fogged).
         * Default: FALSE.
         */
    rwRENDERSTATEFOGCOLOR = 15,
        /**<Color used for fogging. The value should be a packed RwUInt32
         *  in a platform specific format. The macro RWRGBALONG(r, g, b, a)
         *  may be used to construct this using 8-bit color components.
         * Default: RWRGBALONG(0, 0, 0, 0).
         */
    rwRENDERSTATEFOGTYPE = 16,
        /**<\ref RwFogType, the type of fogging to use.
         * Default: rwFOGTYPELINEAR.
         */
    rwRENDERSTATEFOGDENSITY = 17,
        /**<Fog density for \ref RwFogType of
         *  \ref rwFOGTYPEEXPONENTIAL or \ref rwFOGTYPEEXPONENTIAL2.
         *  The value should be a pointer to an RwReal in the
         *  range 0 to 1.
         * Default: 1.
         */
    rwRENDERSTATECULLMODE = 20,
        /**<\ref RwCullMode, for selecting front/back face culling, or
         *  no culling.
         * Default: rwCULLMODECULLBACK.
         */
    rwRENDERSTATESTENCILENABLE,
        /**<Stenciling on/off.
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: FALSE.
         */
    rwRENDERSTATESTENCILFAIL,
        /**<\ref RwStencilOperation used when the stencil test passes.
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: rwSTENCILOPERATIONKEEP.
         */
    rwRENDERSTATESTENCILZFAIL,
        /**<\ref RwStencilOperation used when the stencil test passes and
         *  the depth test (z-test) fails.
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: rwSTENCILOPERATIONKEEP.
         */
    rwRENDERSTATESTENCILPASS,
        /**<\ref RwStencilOperation used when both the stencil and the depth
         *  (z) tests pass.
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: rwSTENCILOPERATIONKEEP.
         */
    rwRENDERSTATESTENCILFUNCTION,
        /**<\ref RwStencilFunction for the stencil test.
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: rwSTENCILFUNCTIONALWAYS.
         */
    rwRENDERSTATESTENCILFUNCTIONREF,
        /**<Integer reference value for the stencil test.
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: 0.
         */
    rwRENDERSTATESTENCILFUNCTIONMASK,
        /**<Mask applied to the reference value and each stencil buffer
         *  entry to determine the significant bits for the stencil test.
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: 0xffffffff.
         */
    rwRENDERSTATESTENCILFUNCTIONWRITEMASK,
        /**<Write mask applied to values written into the stencil buffer.
         *  <i> Supported on Xbox, D3D8, D3D9, and OpenGL only. </i>
         * Default: 0xffffffff.
         */
    rwRENDERSTATEALPHATESTFUNCTION,
        /**<\ref RwAlphaTestFunction for the alpha test. When a pixel fails,
         * neither the frame buffer nor the Z-buffer are updated.
         * Default: rwALPHATESTFUNCTIONGREATER (GameCube, Xbox, D3D8, D3D9
         * and OpenGL). The default PS2 behaviour is to always update the
         * frame buffer and update the Z-buffer only if a greater than or
         * equal test passes.
         */
    rwRENDERSTATEALPHATESTFUNCTIONREF,
        /**<Integer reference value for the alpha test.
         *  <i> Range is 0 to 255, mapped to the platform's actual range </i>
         * Default: 128 (PS2) 0 (GameCube, Xbox, D3D8, D3D9 and OpenGL).
         */

    rwRENDERSTATEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwRenderState RwRenderState;

enum RwShadeMode
{
    rwSHADEMODENASHADEMODE = 0,
    rwSHADEMODEFLAT,                /**<Flat shading */
    rwSHADEMODEGOURAUD,             /**<Gouraud shading */
    rwSHADEMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwShadeMode RwShadeMode;

enum RwTextureFilterMode
{
    rwFILTERNAFILTERMODE = 0,
    rwFILTERNEAREST,                /**<Point sampled */
    rwFILTERLINEAR,                 /**<Bilinear */
    rwFILTERMIPNEAREST,             /**<Point sampled per pixel mip map */
    rwFILTERMIPLINEAR,              /**<Bilinear per pixel mipmap */
    rwFILTERLINEARMIPNEAREST,       /**<MipMap interp point sampled */
    rwFILTERLINEARMIPLINEAR,        /**<Trilinear */
    rwTEXTUREFILTERMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwTextureFilterMode RwTextureFilterMode;

enum RwFogType
{
    rwFOGTYPENAFOGTYPE = 0,
    rwFOGTYPELINEAR,            /**<Linear fog */
    rwFOGTYPEEXPONENTIAL,       /**<Exponential fog */
    rwFOGTYPEEXPONENTIAL2,      /**<Exponential^2 fog */
    rwFOGTYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwFogType RwFogType;

enum RwBlendFunction
{
    rwBLENDNABLEND = 0,
    rwBLENDZERO,            /**<(0,    0,    0,    0   ) */
    rwBLENDONE,             /**<(1,    1,    1,    1   ) */
    rwBLENDSRCCOLOR,        /**<(Rs,   Gs,   Bs,   As  ) */
    rwBLENDINVSRCCOLOR,     /**<(1-Rs, 1-Gs, 1-Bs, 1-As) */
    rwBLENDSRCALPHA,        /**<(As,   As,   As,   As  ) */
    rwBLENDINVSRCALPHA,     /**<(1-As, 1-As, 1-As, 1-As) */
    rwBLENDDESTALPHA,       /**<(Ad,   Ad,   Ad,   Ad  ) */
    rwBLENDINVDESTALPHA,    /**<(1-Ad, 1-Ad, 1-Ad, 1-Ad) */
    rwBLENDDESTCOLOR,       /**<(Rd,   Gd,   Bd,   Ad  ) */
    rwBLENDINVDESTCOLOR,    /**<(1-Rd, 1-Gd, 1-Bd, 1-Ad) */
    rwBLENDSRCALPHASAT,     /**<(f,    f,    f,    1   )  f = min (As, 1-Ad) */
    rwBLENDFUNCTIONFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwBlendFunction RwBlendFunction;

enum RwTextureAddressMode
{
    rwTEXTUREADDRESSNATEXTUREADDRESS = 0,
    rwTEXTUREADDRESSWRAP,      /**<UV wraps (tiles) */
    rwTEXTUREADDRESSMIRROR,    /**<Alternate UV is flipped */
    rwTEXTUREADDRESSCLAMP,     /**<UV is clamped to 0-1 */
    rwTEXTUREADDRESSBORDER,    /**<Border color takes effect outside of 0-1 */
    rwTEXTUREADDRESSMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwTextureAddressMode RwTextureAddressMode;

enum RwStencilOperation
{
    rwSTENCILOPERATIONNASTENCILOPERATION = 0,

    rwSTENCILOPERATIONKEEP,
        /**<Do not update the entry in the stencil buffer */
    rwSTENCILOPERATIONZERO,
        /**<Set the stencil-buffer entry to 0 */
    rwSTENCILOPERATIONREPLACE,
        /**<Replace the stencil-buffer entry with reference value */
    rwSTENCILOPERATIONINCRSAT,
        /**<Increment the stencil-buffer entry, clamping to the
         *  maximum value */
    rwSTENCILOPERATIONDECRSAT,
        /**<Decrement the stencil-buffer entry, clamping to zero */
    rwSTENCILOPERATIONINVERT,
        /**<Invert the bits in the stencil-buffer entry */
    rwSTENCILOPERATIONINCR,
        /**<Increment the stencil-buffer entry, wrapping to zero if
         *  the new value exceeds the maximum value */
    rwSTENCILOPERATIONDECR,
        /**<Decrement the stencil-buffer entry, wrapping to the maximum
         *  value if the new value is less than zero */

    rwSTENCILOPERATIONFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwStencilOperation RwStencilOperation;

enum RwStencilFunction
{
    rwSTENCILFUNCTIONNASTENCILFUNCTION = 0,

    rwSTENCILFUNCTIONNEVER,
        /**<Always fail the test */
    rwSTENCILFUNCTIONLESS,
        /**<Accept the new pixel if its value is less than the value of
         *  the current pixel */
    rwSTENCILFUNCTIONEQUAL,
        /**<Accept the new pixel if its value equals the value of the
         *  current pixel */
    rwSTENCILFUNCTIONLESSEQUAL,
        /**<Accept the new pixel if its value is less than or equal to
         *  the value of the current pixel */
    rwSTENCILFUNCTIONGREATER,
        /**<Accept the new pixel if its value is greater than the value
         *  of the current pixel */
    rwSTENCILFUNCTIONNOTEQUAL,
        /**<Accept the new pixel if its value does not equal the value of
         *  the current pixel */
    rwSTENCILFUNCTIONGREATEREQUAL,
        /**<Accept the new pixel if its value is greater than or equal
         *  to the value of the current pixel */
    rwSTENCILFUNCTIONALWAYS,
        /**<Always pass the test */

    rwSTENCILFUNCTIONFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwStencilFunction RwStencilFunction;

enum RwAlphaTestFunction
{
    rwALPHATESTFUNCTIONNAALPHATESTFUNCTION = 0,

    rwALPHATESTFUNCTIONNEVER,
        /**<Always fail the test */
    rwALPHATESTFUNCTIONLESS,
        /**<Accept the new pixel if its alpha value is less than the value of
         *  the reference value */
    rwALPHATESTFUNCTIONEQUAL,
        /**<Accept the new pixel if its alpha value equals the value of the
         *  reference value */
    rwALPHATESTFUNCTIONLESSEQUAL,
        /**<Accept the new pixel if its alpha value is less than or equal to
         *  the value of the reference value */
    rwALPHATESTFUNCTIONGREATER,
        /**<Accept the new pixel if its alpha value is greater than the value
         *  of the reference value */
    rwALPHATESTFUNCTIONNOTEQUAL,
        /**<Accept the new pixel if its alpha value does not equal the value of
         *  the reference value */
    rwALPHATESTFUNCTIONGREATEREQUAL,
        /**<Accept the new pixel if its alpha value is greater than or equal
         *  to the value of the reference value */
    rwALPHATESTFUNCTIONALWAYS,
        /**<Always pass the test */

    rwALPHATESTFUNCTIONFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwAlphaTestFunction RwAlphaTestFunction;

enum RwCullMode
{
    rwCULLMODENACULLMODE = 0,

    rwCULLMODECULLNONE,
        /**<Both front and back-facing triangles are drawn. */
    rwCULLMODECULLBACK,
        /**<Only front-facing triangles are drawn */
    rwCULLMODECULLFRONT,
        /**<Only back-facing triangles are drawn */

    rwCULLMODEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwCullMode RwCullMode;

enum RwPrimitiveType
{
    rwPRIMTYPENAPRIMTYPE = 0,   /**<Invalid primative type */
    rwPRIMTYPELINELIST = 1,     /**<Unconnected line segments, each line is specified by
                                 * both its start and end index, independently of other lines
                                 * (for example, 3 segments specified as 0-1, 2-3, 4-5) */
    rwPRIMTYPEPOLYLINE = 2,     /**<Connected line segments, each line's start index
                                 * (except the first) is specified by the index of the end of
                                 * the previous segment (for example, 3 segments specified as
                                 * 0-1, 1-2, 2-3) */
    rwPRIMTYPETRILIST = 3,      /**<Unconnected triangles: each triangle is specified by
                                 * three indices, independently of other triangles (for example,
                                 * 3 triangles specified as 0-1-2, 3-4-5, 6-7-8) */
    rwPRIMTYPETRISTRIP = 4,     /**<Connected triangles sharing an edge with, at most, one
                                 * other forming a series (for example, 3 triangles specified
                                 * as 0-2-1, 1-2-3-, 2-4-3) */
    rwPRIMTYPETRIFAN = 5 ,      /**<Connected triangles sharing an edge with, at most,
                                 * two others forming a fan (for example, 3 triangles specified
                                 * as 0-2-1, 0-3-2, 0-4-3) */
    rwPRIMTYPEPOINTLIST = 6,    /**<Points 1, 2, 3, etc. This is not
                                 * supported by the default RenderWare
                                 * immediate or retained-mode pipelines
                                 * (except on PlayStation 2), it is intended
                                 * for use by user-created pipelines */
    rwPRIMITIVETYPEFORCEENUMSIZEINT = RWFORCEENUMSIZEINT
};
typedef enum RwPrimitiveType RwPrimitiveType;

/* Expose Z buffer range */
extern RwReal (*RwIm2DGetNearScreenZ)(void);
extern RwReal (*RwIm2DGetFarScreenZ)(void);

extern RwBool (*RwRenderStateGet)(RwRenderState state, void *value);
extern RwBool (*RwRenderStateSet)(RwRenderState state, void *value);

extern RwBool (*RwIm2DRenderLine)(RwIm2DVertex *vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2);
extern RwBool (*RwIm2DRenderTriangle)(RwIm2DVertex *vertices, RwInt32 numVertices,
                                   RwInt32 vert1, RwInt32 vert2, RwInt32 vert3 );
extern RwBool (*RwIm2DRenderPrimitive)(RwPrimitiveType primType, RwIm2DVertex *vertices, RwInt32 numVertices);
extern RwBool (*RwIm2DRenderIndexedPrimitive)(RwPrimitiveType primType, RwIm2DVertex *vertices, RwInt32 numVertices,
                                                             RwImVertexIndex *indices, RwInt32 numIndices);

/****************************************************************************
 Global Types
 */
typedef struct RwRGBAReal RwRGBAReal;
struct RwRGBAReal
{
    RwReal red;     /**< red component */
    RwReal green;   /**< green component */
    RwReal blue;    /**< blue component */
    RwReal alpha;   /**< alpha component */
};

typedef struct RwRGBA RwRGBA;
struct RwRGBA
{
    RwUInt8 red;    /**< red component */
    RwUInt8 green;  /**< green component */
    RwUInt8 blue;   /**< blue component */
    RwUInt8 alpha;  /**< alpha component */
};


struct RwSurfaceProperties
{
	RwReal ambient;   /**< ambient reflection coefficient */
	RwReal specular;  /**< specular reflection coefficient */
	RwReal diffuse;   /**< reflection coefficient */
};

struct RpMaterial
{
	struct RwTexture* texture; /**< texture */
	RwRGBA              color; /**< color */
	struct RxPipeline* pipeline; /**< pipeline */
	RwSurfaceProperties surfaceProps; /**< surfaceProps */
	RwInt16             refCount;          /* C.f. rwsdk/world/bageomet.h:RpGeometry */
	RwInt16             pad;
};

struct RpMaterialList
{
	RpMaterial** materials;
	RwInt32        numMaterials;
	RwInt32        space;
};

struct RpGeometry
{
	RwUInt32            object;     /* Generic type */
	RwUInt32			pad;

	RwUInt32            flags;      /* Geometry flags */

	RwUInt16            lockedSinceLastInst; /* What has been locked since we last instanced - for re-instancing */
	RwInt16             refCount;   /* Reference count (for keeping track of atomics referencing geometry) */

	RwInt32             numTriangles; /* Quantity of various things (polys, verts and morph targets) */
	RwInt32             numVertices;
	RwInt32             numMorphTargets;
	RwInt32             numTexCoordSets;

	RpMaterialList      matList;

	struct RpTriangle* triangles;  /* The triangles */

	RwRGBA* preLitLum;  /* The pre-lighting values */

	RwTexCoords* texCoords[8]; /* Texture coordinates */

	struct RpMeshHeader* mesh;   /* The mesh - groups polys of the same material */

	struct RwResEntry* repEntry;       /* Information for an instance */

	struct RpMorphTarget* morphTarget;    /* The Morph Target */
};

typedef struct RwStream RwStream;

struct RwMatrixTag
{
    /* These are padded to be 16 byte quantities per line */
    RwV3d               right;
    RwUInt32            flags;
    RwV3d               up;
    RwUInt32            pad1;
    RwV3d               at;
    RwUInt32            pad2;
    RwV3d               pos;
    RwUInt32            pad3;
};

typedef RwMatrixTag RwMatrix;

struct RwObject
{
    RwUInt8 type;                /**< Internal Use */
    RwUInt8 subType;             /**< Internal Use */
    RwUInt8 flags;               /**< Internal Use */
    RwUInt8 privateFlags;        /**< Internal Use */
    void   *parent;              /**< Internal Use */
    /* Often a Frame  */
};

typedef struct RwLLLink  RwLLLink;                     /*** RwLLLink ***/

struct RwLLLink
{
    RwLLLink *next;
    RwLLLink *prev;
};

typedef struct RwLinkList RwLinkList;

struct RwLinkList
{
    RwLLLink link;
};

typedef RwObject *(*RwObjectCallBack)(RwObject *object, void *data);

/****************************************************************************
 Global Types
 */
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwplugin
 * \ref RwPluginDataChunkWriteCallBack represents the function
 * registered by \ref RwCameraRegisterPluginStream, etc. as the function that
 * writes extension data to a binary stream.
 *
 * \param  stream   Pointer to the binary stream
 *
 * \param  binaryLength   A RwInt32 value equal to the binary
 * size (in bytes) of the extension data that will be written to the binary
 * stream.
 *
 * \param  object   Pointer to the object containing the extension
 * data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the byte
 * offset of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \return Pointer to the stream
 */
typedef RwStream *(*RwPluginDataChunkWriteCallBack)(RwStream *stream, RwInt32 binaryLength, const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

/**
 * \ingroup rwplugin
 * \ref RwPluginDataChunkReadCallBack represents the function
 * registered by \ref RwCameraRegisterPluginStream, etc. as the function that
 * reads extension data from a binary stream.
 *
 * \param  stream   Pointer to the binary stream
 *
 * \param  binaryLength   A RwInt32 value equal to the binary
 * size (in bytes) of the extension data that will be read from a binary
 * stream.
 *
 * \param  object   Pointer to the object containing the extension
 * data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the byte
 * offset of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \return Pointer to the stream
 */
typedef RwStream *(*RwPluginDataChunkReadCallBack)(RwStream *stream, RwInt32 binaryLength, void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

/**
 * \ingroup rwplugin
 * \ref RwPluginDataChunkGetSizeCallBack represents the callback
 * registered by \ref RwCameraRegisterPluginStream, etc. as the function that
 * determines the binary size of the extension data.
 *
 * \param  object   Pointer to the object containing the extension data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the byte
 * offset of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \return A RwInt32 value equal to the size in bytes of the plugin extension data.
 */
typedef RwInt32(*RwPluginDataChunkGetSizeCallBack)(const void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

/**
 * \ingroup rwplugin
 * \ref RwPluginDataChunkAlwaysCallBack represents the callback
 * registered by \ref RwCameraSetStreamAlwaysCallBack, etc. as the
 * function that is called after the reading of plugin stream data is
 * finished (useful to set up plugin data for plugins that found no
 * data in the stream, but that cannot set up the data during the
 * \ref RwPluginObjectConstructor callback).
 *
 * \param  object   Pointer to the object containing the extension data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the byte
 * offset of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \return Returns TRUE if successful, FALSE otherwise.
 */
typedef RwBool(*RwPluginDataChunkAlwaysCallBack)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

/**
 * \ingroup rwplugin
 * \ref RwPluginDataChunkRightsCallBack represents the callback
 * registered by RwCameraSetStreamRightsCallBack, etc. as the
 * function that is called after the reading of plugin stream data is
 * finished, and the object finalised, if and only if the object's rights
 * id was equal to that of the plugin registering the call.
 * For convience the extension data is passed to the callback.
 *
 * \param  object   Pointer to the object containing the extension data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the byte
 * offset of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \param  extraData     An RwUInt32 writen with the plugin id.
 *
 * \return Returns TRUE if successful, FALSE otherwise.
 */
typedef RwBool(*RwPluginDataChunkRightsCallBack)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject, RwUInt32 extraData);

/**
 * \ingroup rwplugin
 * \ref RwPluginObjectConstructor represents the callback
 * registered by \ref RwEngineRegisterPlugin, \ref RwCameraRegisterPlugin, etc.
 * as the function that initializes either the global extension data (in the
 * case of \ref RwEngineRegisterPlugin) or the object extension data (in all
 * other cases). Registered by \ref RwCameraSetStreamAlwaysCallBack, etc.
 *
 * \param  object   Pointer to the object (global or otherwise)
 * that contains the extension data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the
 * byte offset of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \return Pointer to the object
 */
typedef void *(*RwPluginObjectConstructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

/**
 * \ingroup rwplugin
 * \ref RwPluginObjectCopy represents the callback registered by
 * \ref RwCameraRegisterPlugin, etc. as the function that copies the object
 * extension data when an object is duplicated.
 *
 * \param  dstObject   Pointer to the destination object that will
 * receive the extension data.
 *
 * \param  srcObject   Pointer to the source object containing
 * extension data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the byte offset
 * of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \return Pointer to the object
 */
typedef void *(*RwPluginObjectCopy)(void *dstObject, const void *srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);

/**
 * \ingroup rwplugin
 * \ref RwPluginObjectDestructor represents the callback registered
 * by \ref RwEngineRegisterPlugin, \ref RwCameraRegisterPlugin, etc. as the
 * function that destroys either the global extension data (in the case of
 * \ref RwEngineRegisterPlugin) or the object extension data (in all other
 * cases).
 *
 * \param  object   Pointer to the object (global or otherwise)
 * containing the extension data.
 *
 * \param  offsetInObject   A RwInt32 value equal to the byte
 * offset of the extension data in the object.
 *
 * \param  sizeInObject   A RwInt32 value equal to the size
 * (in bytes) of the extension data.
 *
 * \return Pointer to the object.
 */
#endif /* RWADOXYGENEXTERNAL */
typedef void *(*RwPluginObjectDestructor)(void *object, RwInt32 offsetInObject, RwInt32 sizeInObject);

typedef void *(*RwPluginErrorStrCallBack)(void *);

typedef struct RwPluginRegistry RwPluginRegistry;
typedef struct RwPluginRegEntry RwPluginRegEntry;

#if (!defined(DOXYGEN))
struct RwPluginRegistry
{
    RwInt32          sizeOfStruct;
    RwInt32          origSizeOfStruct;
    RwInt32          maxSizeOfStruct;
    RwInt32          staticAlloc;
    RwPluginRegEntry *firstRegEntry;
    RwPluginRegEntry *lastRegEntry;
};

struct RwPluginRegEntry
{
    RwInt32         offset;
    RwInt32         size;
    RwUInt32        pluginID;
    RwPluginDataChunkReadCallBack readCB;
    RwPluginDataChunkWriteCallBack writeCB;
    RwPluginDataChunkGetSizeCallBack getSizeCB;
    RwPluginDataChunkAlwaysCallBack alwaysCB;
    RwPluginDataChunkRightsCallBack rightsCB;
    RwPluginObjectConstructor constructCB;
    RwPluginObjectDestructor destructCB;
    RwPluginObjectCopy copyCB;
    RwPluginErrorStrCallBack errStrCB;
    RwPluginRegEntry *nextRegEntry;
    RwPluginRegEntry *prevRegEntry;
    RwPluginRegistry *parentRegistry;
};
#endif /* (!defined(DOXYGEN)) */

/*--- Automatically derived from: C:/daily/rwsdk/src/plcore/bamatrix.h ---*/

/****************************************************************************
 Defines
 */

RwMatrix* RwMatrixUpdate(RwMatrix* matrix);
RwBool RwMatrixDestroy(RwMatrix* mpMat);
RwBool RwStreamFindChunk(RwStream* stream, RwUInt32 type, RwUInt32* lengthOut, RwUInt32* versionOut);
RwStream* RwStreamOpen(RwStreamType type, RwStreamAccessType accessType, const void* data);
RwBool RwStreamClose(RwStream* stream, void* data);

