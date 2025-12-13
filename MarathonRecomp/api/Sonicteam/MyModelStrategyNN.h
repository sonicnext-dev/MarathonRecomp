#pragma once

#include <Marathon.inl>
#include <Sonicteam/MyModelStrategy.h>
#include <Sonicteam/SoX/Array.h>
#include <stdx/string.h>
#include <stdx/vector.h>
#include <Sonicteam/SoX/Math/Vector.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/Graphics/Xenon/VertexBufferXenon.h>
#include <Sonicteam/SoX/Graphics/Xenon/IndexBufferXenon.h>
#include <Sonicteam/SoX/Graphics/SamplerCommon.h>
#include <boost/smart_ptr/shared_ptr.h>

namespace Sonicteam
{
    class MyGraphicDevice;
    class MyModel;
    class MyShaderCommon;
    class MyBoneCommon;
    class MyMaterialCommon;

    struct StrategyNNBone
    {
    public:
        boost::shared_ptr<MyBoneCommon> spField0;
        MARATHON_INSERT_PADDING(4); // pointer
        boost::shared_ptr<MyBoneCommon> spMyBone;
        MARATHON_INSERT_PADDING(0x18); // bones params ?
    };

    MARATHON_ASSERT_OFFSETOF(StrategyNNBone, spField0, 0);
    MARATHON_ASSERT_OFFSETOF(StrategyNNBone, spMyBone, 0xC);
    MARATHON_ASSERT_SIZEOF(StrategyNNBone, 0x2C);

    struct StrategyNNModel38_2
    {
    public:
        be<uint32_t> TextureIndex;
        stdx::vector<StrategyNNBone> avBones[3]; // This should be 3 separate vectors, not array of vectors
        be<uint32_t> Field34;
    };

    MARATHON_ASSERT_OFFSETOF(StrategyNNModel38_2, TextureIndex, 0);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel38_2, avBones, 4);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel38_2, Field34, 0x34);
    MARATHON_ASSERT_SIZEOF(StrategyNNModel38_2, 0x38);

    struct StrategyNNModel38
    {
    public:
        SoX::RefSharedPointer<MyShaderCommon> spMyShaderCommon;
        stdx::vector<StrategyNNModel38_2> vField4;
        stdx::vector<StrategyNNModel38_2> vField14;
        stdx::vector<StrategyNNModel38_2> vField24;
        be<uint32_t> Index; // probably or flag?, index to tecnhique?
    };

    MARATHON_ASSERT_OFFSETOF(StrategyNNModel38, spMyShaderCommon, 0);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel38, vField4, 4);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel38, vField14, 0x14);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel38, vField24, 0x24);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel38, Index, 0x34);
    MARATHON_ASSERT_SIZEOF(StrategyNNModel38, 0x38);

    struct StrategyNNModel
    {
    public:
        xpointer<MyGraphicDevice> pMyGraphicDevice;
        stdx::string Name; //player/sonic_new/sonic_Root.xno
        SoX::RefSharedPointer<SoX::Graphics::Xenon::VertexBufferXenon> pVertexBufferXenon;
        SoX::RefSharedPointer<SoX::Graphics::Xenon::IndexBufferXenon> pIndexBufferXenon;
        SoX::Math::Vector Field30;
        SoX::Math::Vector Field40;
        char Field50;
        SoX::Math::Vector Field60;
        be<float> Field70;
        MARATHON_INSERT_PADDING(0x10);
        stdx::vector<StrategyNNModel38> Field84; //Shader-Tex-Bone?
        stdx::vector<StrategyNNModel38> Field94; //Shader-Tex-Bone? (empty, almost, no reaction)
        stdx::vector<boost::shared_ptr<MyMaterialCommon>> vspMaterials;
        stdx::vector<boost::shared_ptr<SoX::Graphics::SamplerCommon>> vspSamplers;
        be<uint32_t> FlagC4; // == 5 (default Model Render, 6 = (face only with eyes), 8 = (face only no eyes) .....)
        SoX::RefSharedPointer<SoX::RefCountObject> maRefObjects[3];
        boost::shared_ptr<void> spFieldD4;
        char FieldDC;
    };

    MARATHON_ASSERT_OFFSETOF(StrategyNNModel, pMyGraphicDevice, 0);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel, Name, 4);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel, pVertexBufferXenon, 0x20);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel, pIndexBufferXenon, 0x24);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel, Field30, 0x30);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel, Field40, 0x40);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel, Field50, 0x50);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel, Field60, 0x60);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel, Field70, 0x70);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel, Field84, 0x84);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel, Field94, 0x94);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel, vspMaterials, 0xA4);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel, vspSamplers, 0xB4);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel, FlagC4, 0xC4);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel, maRefObjects, 0xC8);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel, spFieldD4, 0xD4);
    MARATHON_ASSERT_OFFSETOF(StrategyNNModel, FieldDC, 0xDC);
    MARATHON_ASSERT_SIZEOF(StrategyNNModel, 0xE0);


    struct alignas(16) MyModelStrategyNNBoneTransform
    {
    public:
        MARATHON_INSERT_PADDING(0x80);
        stdx::string Name;
        MARATHON_INSERT_PADDING(0x54);
    };

    class MyModelStrategyNN : public MyModelStrategy
    {
    public:
        xpointer<MyGraphicDevice> m_pMyGraphicDevice;
        xpointer<MyModel> m_pMyModel;
        MARATHON_INSERT_PADDING(4);
        StrategyNNModel m_NNModel;
        Sonicteam::SoX::ArrayOld<MyModelStrategyNNBoneTransform> m_aBoneTransform;
        be<uint32_t> m_BoneCount;
        be<uint32_t> m_FieldF8; // count ?
        xpointer<void> m_pFieldFC;  // transform, for bones?
        stdx::string m_AttachPoint;
        stdx::vector<stdx::string> m_MaterialNames;
    };

    MARATHON_ASSERT_OFFSETOF(MyModelStrategyNN, m_pMyGraphicDevice, 4);
    MARATHON_ASSERT_OFFSETOF(MyModelStrategyNN, m_pMyModel, 8);
    MARATHON_ASSERT_OFFSETOF(MyModelStrategyNN, m_NNModel, 0x10);
    MARATHON_ASSERT_OFFSETOF(MyModelStrategyNN, m_aBoneTransform, 0xF0);
    MARATHON_ASSERT_OFFSETOF(MyModelStrategyNN, m_BoneCount, 0xF4);
    MARATHON_ASSERT_OFFSETOF(MyModelStrategyNN, m_FieldF8, 0xF8);
    MARATHON_ASSERT_OFFSETOF(MyModelStrategyNN, m_pFieldFC, 0xFC);
    MARATHON_ASSERT_OFFSETOF(MyModelStrategyNN, m_AttachPoint, 0x100);
    MARATHON_ASSERT_OFFSETOF(MyModelStrategyNN, m_MaterialNames, 0x11C);
    MARATHON_ASSERT_SIZEOF(MyModelStrategyNN, 0x130);
}
