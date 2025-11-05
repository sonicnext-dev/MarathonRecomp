#pragma once

#include <Marathon.inl>
#include <Chao/CSD/Core/csdBase.h>
#include <Chao/CSD/Manager/csdmResourceBase.h>
#include <Chao/CSD/Manager/csdmSceneObserver.h>
#include <Chao/CSD/Manager/csdmSubjectBase.h>

namespace Chao::CSD
{
    class CScene;
    class CNode;

    struct Cast
    {
        MARATHON_INSERT_PADDING(0x144);
    };

    struct CastLink
    {
        be<uint32_t> ChildCastIndex;
        be<uint32_t> SiblingCastIndex;
    };

    struct CastNode
    {
        be<uint32_t> CastCount;
        MARATHON_INSERT_PADDING(4);
        xpointer<xpointer<Cast>> pCasts;
        MARATHON_INSERT_PADDING(4);
        be<uint32_t> RootCastIndex;
        MARATHON_INSERT_PADDING(4);
        xpointer<CastLink> pCastLinks;
        MARATHON_INSERT_PADDING(4);
    };

    struct CastIndex
    {
        xpointer<const char> pCastName;
        MARATHON_INSERT_PADDING(4);
        be<uint32_t> CastNodeIndex;
        be<uint32_t> CastIndex;
    };

    struct Scene
    {
        MARATHON_INSERT_PADDING(8);
        be<float> FPS;
        MARATHON_INSERT_PADDING(0x24);
        be<uint32_t> CastNodeCount;
        MARATHON_INSERT_PADDING(4);
        xpointer<CastNode> pCastNodes;
        MARATHON_INSERT_PADDING(4);
        be<uint32_t> CastCount;
        MARATHON_INSERT_PADDING(4);
        xpointer<CastIndex> pCastIndices;
        MARATHON_INSERT_PADDING(4);
        be<uint32_t> AnimationCount;
        MARATHON_INSERT_PADDING(4);
        xpointer<void> pAnimationKeyFrameDataList;
        MARATHON_INSERT_PADDING(4);
        xpointer<void> pAnimationDictionary;
        MARATHON_INSERT_PADDING(4);
        be<float> AspectRatio;
        MARATHON_INSERT_PADDING(4);
        xpointer<void> pAnimationFrameDataList;
        MARATHON_INSERT_PADDING(4);
    };

    struct SceneIndex
    {
        xpointer<const char> pSceneName;
        MARATHON_INSERT_PADDING(4);
        be<uint32_t> SceneIndex;
        MARATHON_INSERT_PADDING(4);
    };

    struct SceneNodeIndex
    {
        xpointer<const char> pSceneNodeName;
        be<uint32_t> SceneNodeIndex;
    };

    struct SceneNode
    {
        be<uint32_t> SceneCount;
        MARATHON_INSERT_PADDING(4);
        xpointer<xpointer<Scene>> pScenes;
        MARATHON_INSERT_PADDING(4);
        xpointer<SceneIndex> pSceneIndices;
        MARATHON_INSERT_PADDING(4);
        be<uint32_t> SceneNodeCount;
        MARATHON_INSERT_PADDING(4);
        xpointer<SceneNode> pSceneNodes;
        MARATHON_INSERT_PADDING(4);
        xpointer<SceneNodeIndex> pSceneNodeIndices;
    };

    enum MotionRepeatType : uint32_t
    {
        MotionRepeatType_PlayOnce,
        MotionRepeatType_Loop,
        MotionRepeatType_PingPong,
        MotionRepeatType_PlayThenDestroy
    };

    class CScene : public CResourceBase<Scene>, SubjectBase<CSceneObserver, CScene>, CBase
    {
    public:
        MARATHON_INSERT_PADDING(0x60);
        be<float> m_PrevMotionFrame;
        be<float> m_MotionFrame;
        be<float> m_MotionSpeed;
        be<float> m_MotionStartFrame;
        be<float> m_MotionEndFrame;
        MARATHON_INSERT_PADDING(0x0C);
        be<uint32_t> m_MotionDisableFlag;
        MARATHON_INSERT_PADDING(0x10);
        be<MotionRepeatType> m_MotionRepeatType;
        MARATHON_INSERT_PADDING(0x2C);
    };

    MARATHON_ASSERT_OFFSETOF(CastLink, ChildCastIndex, 0x00);
    MARATHON_ASSERT_OFFSETOF(CastLink, SiblingCastIndex, 0x04);

    MARATHON_ASSERT_OFFSETOF(CastNode, CastCount, 0x00);
    MARATHON_ASSERT_OFFSETOF(CastNode, pCasts, 0x08);
    MARATHON_ASSERT_OFFSETOF(CastNode, RootCastIndex, 0x10);
    MARATHON_ASSERT_OFFSETOF(CastNode, pCastLinks, 0x18);

    MARATHON_ASSERT_OFFSETOF(CastIndex, pCastName, 0x00);
    MARATHON_ASSERT_OFFSETOF(CastIndex, CastNodeIndex, 0x08);
    MARATHON_ASSERT_OFFSETOF(CastIndex, CastIndex, 0x0C);

    MARATHON_ASSERT_OFFSETOF(Scene, FPS, 0x08);
    MARATHON_ASSERT_OFFSETOF(Scene, CastNodeCount, 0x30);
    MARATHON_ASSERT_OFFSETOF(Scene, pCastNodes, 0x38);
    MARATHON_ASSERT_OFFSETOF(Scene, CastCount, 0x40);
    MARATHON_ASSERT_OFFSETOF(Scene, pCastIndices, 0x48);
    MARATHON_ASSERT_OFFSETOF(Scene, AnimationCount, 0x50);
    MARATHON_ASSERT_OFFSETOF(Scene, pAnimationKeyFrameDataList, 0x58);
    MARATHON_ASSERT_OFFSETOF(Scene, pAnimationDictionary, 0x60);
    MARATHON_ASSERT_OFFSETOF(Scene, AspectRatio, 0x68);
    MARATHON_ASSERT_OFFSETOF(Scene, pAnimationFrameDataList, 0x70);

    MARATHON_ASSERT_OFFSETOF(SceneIndex, pSceneName, 0x00);
    MARATHON_ASSERT_OFFSETOF(SceneIndex, SceneIndex, 0x08);

    MARATHON_ASSERT_OFFSETOF(SceneNodeIndex, pSceneNodeName, 0x00);
    MARATHON_ASSERT_OFFSETOF(SceneNodeIndex, SceneNodeIndex, 0x04);

    MARATHON_ASSERT_OFFSETOF(SceneNode, SceneCount, 0x00);
    MARATHON_ASSERT_OFFSETOF(SceneNode, pScenes, 0x08);
    MARATHON_ASSERT_OFFSETOF(SceneNode, pSceneIndices, 0x10);
    MARATHON_ASSERT_OFFSETOF(SceneNode, SceneNodeCount, 0x18);
    MARATHON_ASSERT_OFFSETOF(SceneNode, pSceneNodes, 0x20);
    MARATHON_ASSERT_OFFSETOF(SceneNode, pSceneNodeIndices, 0x28);

    MARATHON_ASSERT_OFFSETOF(CScene, m_PrevMotionFrame, 0x7C);
    MARATHON_ASSERT_OFFSETOF(CScene, m_MotionFrame, 0x80);
    MARATHON_ASSERT_OFFSETOF(CScene, m_MotionSpeed, 0x84);
    MARATHON_ASSERT_OFFSETOF(CScene, m_MotionStartFrame, 0x88);
    MARATHON_ASSERT_OFFSETOF(CScene, m_MotionEndFrame, 0x8C);
    MARATHON_ASSERT_OFFSETOF(CScene, m_MotionDisableFlag, 0x9C);
    MARATHON_ASSERT_OFFSETOF(CScene, m_MotionRepeatType, 0xB0);
}
