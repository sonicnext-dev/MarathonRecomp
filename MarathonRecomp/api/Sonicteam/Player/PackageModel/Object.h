#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Graphics/Motion.h>
#include <Sonicteam/SoX/Graphics/Texture.h>
#include <Sonicteam/SoX/Graphics/FrameGP.h>
#include <Sonicteam/SoX/Graphics/InstanceModelCommon.h>
#include <Sonicteam/SoX/Graphics/MotionHierarchy.h>
#include <Sonicteam/SoX/Graphics/LocalAnimationUserDataListener.h>
#include <Sonicteam/SoX/Scenery/Clump.h>
#include <Sonicteam/SoX/Graphics/Model.h>
#include <Sonicteam/SoX/RefSharedPointer.h>

#include <stdx/vector.h>
#include <boost/smart_ptr/shared_ptr.h>

namespace Sonicteam::Player::PackageModel
{
    class IAnimation;

    struct ObjectTexture
    {
        SoX::RefSharedPointer<SoX::Graphics::Model> spDiffuseTexture; // ch_sonic_tex04_df.dds
        SoX::RefSharedPointer<SoX::Graphics::Model> spNormalTexture; // ch_sonic_tex04_nw.dds
        SoX::RefSharedPointer<SoX::Graphics::Model> spSpecularTexture; // ch_sonic_tex04_sp.dds
    };

    struct ObjectE0
    {
        MARATHON_INSERT_PADDING(0xC);
    };

    class Object
	{
    public:
        xpointer<void> m_pVftable;
        xpointer<SoX::Graphics::FrameGP> m_pFrameGPObject;
        stdx::vector<void*> m_vField8; // some Transform Data? size 0x70+-
        stdx::vector<void*> m_vField18; // some Transform Data? size 4 ?
        stdx::vector<void*> m_vField28; // ???

        SoX::RefSharedPointer<SoX::Graphics::Model> m_spModelRoot;
        SoX::RefSharedPointer<SoX::Graphics::InstanceModelCommon> m_spInstanceRootModel;
        SoX::RefSharedPointer<SoX::Scenery::Clump> m_spRootClump;

        SoX::RefSharedPointer<SoX::Graphics::Model> m_spModelHead;
        SoX::RefSharedPointer<SoX::Graphics::InstanceModelCommon> m_spInstanceHeadModel;
        SoX::RefSharedPointer<SoX::Scenery::Clump> m_spHeadClump;

        stdx::vector<SoX::RefSharedPointer<SoX::Graphics::Motion>> m_vspMotionsRoot;
        stdx::vector<SoX::RefSharedPointer<SoX::Graphics::Motion>> m_vspMotionsFace;
        stdx::vector<ObjectTexture> m_vTextures;
        SoX::RefSharedPointer<SoX::Graphics::MotionHierarchy> m_spAnimationHierarchyRoot;
        SoX::RefSharedPointer<SoX::Graphics::MotionHierarchy> m_spAnimationHierarchyFace;
        MARATHON_INSERT_PADDING(0x10);
        boost::shared_ptr<IAnimation> m_PackageAnimationRoot;
        MARATHON_INSERT_PADDING(4);
        boost::shared_ptr<IAnimation> m_PackageAnimationFace;
        MARATHON_INSERT_PADDING(0xC);
        be<float> m_FieldB8;
        be<float> m_FieldBC;
        MARATHON_INSERT_PADDING(0x10);
        be<float> m_FieldD0;
        be<uint32_t> m_FieldD4;
        be<uint32_t> m_FieldD8;
        SoX::RefSharedPointer<SoX::Graphics::LocalAnimationUserDataListener> m_spLocalAnimationUserDataListener;
        ObjectE0 m_FieldE0[3];
        MARATHON_INSERT_PADDING(0x4);
        boost::shared_ptr<IAnimation> m_spHairAnimation;
	};

    MARATHON_ASSERT_OFFSETOF(Object, m_pVftable, 0x00);
    MARATHON_ASSERT_OFFSETOF(Object, m_vField8, 0x8);
    MARATHON_ASSERT_OFFSETOF(Object, m_vField18, 0x18);
    MARATHON_ASSERT_OFFSETOF(Object, m_vField28, 0x28);

    MARATHON_ASSERT_OFFSETOF(Object, m_spModelRoot, 0x38);
    MARATHON_ASSERT_OFFSETOF(Object, m_spInstanceRootModel, 0x3C);
    MARATHON_ASSERT_OFFSETOF(Object, m_spRootClump, 0x40);

    MARATHON_ASSERT_OFFSETOF(Object, m_spModelHead, 0x44);
    MARATHON_ASSERT_OFFSETOF(Object, m_spInstanceHeadModel, 0x48);
    MARATHON_ASSERT_OFFSETOF(Object, m_spHeadClump, 0x4C);

    MARATHON_ASSERT_OFFSETOF(Object, m_vspMotionsRoot, 0x50);
    MARATHON_ASSERT_OFFSETOF(Object, m_vspMotionsFace, 0x60);
    MARATHON_ASSERT_OFFSETOF(Object, m_vTextures, 0x70);
    MARATHON_ASSERT_OFFSETOF(Object, m_spAnimationHierarchyRoot, 0x80);
    MARATHON_ASSERT_OFFSETOF(Object, m_spAnimationHierarchyFace, 0x84);
    MARATHON_ASSERT_OFFSETOF(Object, m_PackageAnimationRoot, 0x98);
    MARATHON_ASSERT_OFFSETOF(Object, m_PackageAnimationFace, 0xA4);
    MARATHON_ASSERT_OFFSETOF(Object, m_FieldB8, 0xB8);
    MARATHON_ASSERT_OFFSETOF(Object, m_FieldBC, 0xBC);
    MARATHON_ASSERT_OFFSETOF(Object, m_FieldD0, 0xD0);
    MARATHON_ASSERT_OFFSETOF(Object, m_FieldD4, 0xD4);
    MARATHON_ASSERT_OFFSETOF(Object, m_FieldD8, 0xD8);
    MARATHON_ASSERT_OFFSETOF(Object, m_spLocalAnimationUserDataListener, 0xDC);
    MARATHON_ASSERT_OFFSETOF(Object, m_FieldE0, 0xE0);
    MARATHON_ASSERT_OFFSETOF(Object, m_spHairAnimation, 0x108);
    MARATHON_ASSERT_SIZEOF(Object, 0x110);
}
