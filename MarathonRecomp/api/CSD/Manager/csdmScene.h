#pragma once

#include <Marathon.inl>
#include <CSD/Manager/csdmBase.h>
#include <CSD/Manager/csdmResourceBase.h>
#include <CSD/Manager/csdmSceneObserver.h>
#include <CSD/Manager/csdmSubjectBase.h>

namespace Chao::CSD
{
    class CScene;
    class CNode;

    struct Cast
    {
        MARATHON_INSERT_PADDING(0x144); // TODO: might be 0x130?
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

    enum EMotionRepeatType : uint32_t
    {
        eMotionRepeatType_PlayOnce = 0,
        eMotionRepeatType_Loop = 1,
        eMotionRepeatType_PingPong = 2,
        eMotionRepeatType_PlayThenDestroy = 3
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
        be<EMotionRepeatType> m_MotionRepeatType;
        MARATHON_INSERT_PADDING(0x2C);

        ~CScene();
        void Update(float in_DeltaTime = 0.0f);
        void Render(void* in_pUnk);

        void GetNode(RCPtr<CNode>& out_rResult, const char* in_pName);

        bool SetMotion(const char* in_pName);
        void SetMotionFrame(float in_MotionFrame);
        void SetPosition(float in_X, float in_Y);
        void SetHideFlag(uint32_t in_HideFlag);
        void SetRotation(float in_Angle);
        void SetScale(float in_X, float in_Y);
    };
}

#include "CSD/Manager/csdmScene.inl"
