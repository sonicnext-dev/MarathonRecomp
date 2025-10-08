#pragma once

#include <Marathon.inl>
#include <hk330/hkReferencedObject.h>

namespace hk330
{
    enum hkpShapeType
    {
        HK_SHAPE_INVALID = 0x0,
        HK_SHAPE_CONVEX = 0x1,
        HK_SHAPE_COLLECTION = 0x2, // 0x2 from Collection (2 of them)
        HK_SHAPE_BV_TREE = 0x3, // 0x3 BvTreeShape
        HK_SHAPE_SPHERE = 0x4, // 0x4 (from SphereShape)
        HK_SHAPE_CYLINDER = 0x5, // 0x5 - VERIFIED
        HK_SHAPE_TRIANGLE = 0x6, // 0x6 (from TriangleShp)
        HK_SHAPE_BOX = 0x7, //0x7 (from BoxShape)
        HK_SHAPE_CAPSULE = 0x8, // 0x8 (from CapsuleShape)
        HK_SHAPE_CONVEX_VERTICES = 0x9, // 0x9 (from CvxVerts)
        HK_SHAPE_MULTI_SPHERE = 0xB, // 0xB        
        HK_SHAPE_LIST = 0xC, // 0xC (from ListShape)
        HK_SHAPE_CONVEX_PIECE = 0xD, // 0xD (from CvxPieceMesh)
        HK_SHAPE_MULTI_RAY = 0x11, // 0x11  (from MultiRay)
        HK_SHAPE_BV = 0x16, //0x16 (form BVShape)
        HK_SHAPE_PLANE = 0x17, //0x17 (PlaneShape)
        HK_SHAPE_MOPP = 0x18, //0x18 (from MoppShape)
        HK_SHAPE_TRANSFORM = 0x19, //0x19 (from TransformShp)
        HK_SHAPE_PHANTOM_CALLBACK = 0x1A, //0x1A (PhantomCB)

        // Unknown
        HK_SHAPE_CONVEX_LIST = 0xE,
        HK_SHAPE_CONVEX_TRANSLATE = 0xF,
        HK_SHAPE_TRIANGLE_COLLECTION = 0x10,
        HK_SHAPE_CONVEX_TRANSFORM = 0x12,
        HK_SHAPE_SAMPLED_HEIGHT_FIELD = 0x13,
        HK_SHAPE_SPHERE_REP = 0x14,
        HK_SHAPE_TRI_PATCH = 0x15,
        HK_SHAPE_USER0 = 0x1C,
        HK_SHAPE_USER1 = 0x1D,
        HK_SHAPE_USER2 = 0x1E,
        HK_SHAPE_UNKNOWN = 0x1F,

        HK_SHAPE_ALL = -1,
    };


    class hkpShape : public hkReferencedObject
    {
    public:
        struct Vftable : hkReferencedObject::Vftable
        {
            be<uint32_t> fpFunc4;
            be<uint32_t> fpGetShapeType;
        };

        be<uint32_t> m_userData;

        int32_t GetShapeType()
        {
            auto vft = (Vftable*)m_pVftable.get();
            return GuestToHostFunction<int32_t>(vft->fpGetShapeType, this);
        }

        const char* GetShapeTypeName()
        {
            int32_t type = GetShapeType();
            switch (type)
            {
            case HK_SHAPE_INVALID: return "HK_SHAPE_INVALID";
            case HK_SHAPE_ALL: return "HK_SHAPE_ALL";
            case HK_SHAPE_COLLECTION: return "HK_SHAPE_COLLECTION";
            case HK_SHAPE_BV_TREE: return "HK_SHAPE_BV_TREE";
            case HK_SHAPE_SPHERE: return "HK_SHAPE_SPHERE";
            case HK_SHAPE_CYLINDER: return "HK_SHAPE_CYLINDER";
            case HK_SHAPE_TRIANGLE: return "HK_SHAPE_TRIANGLE";
            case HK_SHAPE_BOX: return "HK_SHAPE_BOX";
            case HK_SHAPE_CAPSULE: return "HK_SHAPE_CAPSULE";
            case HK_SHAPE_CONVEX_VERTICES: return "HK_SHAPE_CONVEX_VERTICES";
            case HK_SHAPE_MULTI_SPHERE: return "HK_SHAPE_MULTI_SPHERE";
            case HK_SHAPE_LIST: return "HK_SHAPE_LIST";
            case HK_SHAPE_CONVEX_PIECE: return "HK_SHAPE_CONVEX_PIECE";
            case HK_SHAPE_MULTI_RAY: return "HK_SHAPE_MULTI_RAY";
            case HK_SHAPE_BV: return "HK_SHAPE_BV";
            case HK_SHAPE_PLANE: return "HK_SHAPE_PLANE";
            case HK_SHAPE_MOPP: return "HK_SHAPE_MOPP";
            case HK_SHAPE_TRANSFORM: return "HK_SHAPE_TRANSFORM";
            case HK_SHAPE_PHANTOM_CALLBACK: return "HK_SHAPE_PHANTOM_CALLBACK";

                // Unknown types
            case HK_SHAPE_CONVEX: return "HK_SHAPE_CONVEX";
            case HK_SHAPE_CONVEX_LIST: return "HK_SHAPE_CONVEX_LIST";
            case HK_SHAPE_CONVEX_TRANSLATE: return "HK_SHAPE_CONVEX_TRANSLATE";
            case HK_SHAPE_TRIANGLE_COLLECTION: return "HK_SHAPE_TRIANGLE_COLLECTION";
            case HK_SHAPE_CONVEX_TRANSFORM: return "HK_SHAPE_CONVEX_TRANSFORM";
            case HK_SHAPE_SAMPLED_HEIGHT_FIELD: return "HK_SHAPE_SAMPLED_HEIGHT_FIELD";
            case HK_SHAPE_SPHERE_REP: return "HK_SHAPE_SPHERE_REP";
            case HK_SHAPE_TRI_PATCH: return "HK_SHAPE_TRI_PATCH";
            case HK_SHAPE_USER0: return "HK_SHAPE_USER0";
            case HK_SHAPE_USER1: return "HK_SHAPE_USER1";
            case HK_SHAPE_USER2: return "HK_SHAPE_USER2";
            case HK_SHAPE_UNKNOWN: return "HK_SHAPE_UNKNOWN";
            default: return "UNKNOWN_SHAPE_TYPE";
            }
        }
    };

    MARATHON_ASSERT_OFFSETOF(hkpShape, m_userData, 0x08);
}
