#pragma once

#include <Marathon.inl>
#include <Sonicteam/Spanverse/_ackBase.h>
#include <Sonicteam/Spanverse/_ackMomBase.h>

#include <Sonicteam/Spanverse/_acmSceneBloom.h>
#include <Sonicteam/Spanverse/_acmTranslateNormal.h>
#include <Sonicteam/Spanverse/_acmTranslateAdd.h>
#include <Sonicteam/Spanverse/_acmTranslateAccel.h>
#include <Sonicteam/Spanverse/_acmTranslateRandomNormal.h>
#include <Sonicteam/Spanverse/_acmTranslateRandomAdd.h>
#include <Sonicteam/Spanverse/_acmTranslateRandomAccel.h>
#include <Sonicteam/Spanverse/_acmTranslateSin.h>
#include <Sonicteam/Spanverse/_acmTranslateRandomSin.h>
#include <Sonicteam/Spanverse/_acmTranslateGoal.h>
#include <Sonicteam/Spanverse/_acmTranslateSpline.h>
#include <Sonicteam/Spanverse/_acmTranslateAddGoal.h>
#include <Sonicteam/Spanverse/_acmTranslateRandomGoal.h>
#include <Sonicteam/Spanverse/_acmTranslateWrap.h>
#include <Sonicteam/Spanverse/_acmRotateNormal.h>
#include <Sonicteam/Spanverse/_acmRotateAdd.h>
#include <Sonicteam/Spanverse/_acmRotateAccel.h>
#include <Sonicteam/Spanverse/_acmRotateRandomNormal.h>
#include <Sonicteam/Spanverse/_acmRotateRandomAdd.h>
#include <Sonicteam/Spanverse/_acmRotateRandomAccel.h>
#include <Sonicteam/Spanverse/_acmRotateSin.h>
#include <Sonicteam/Spanverse/_acmRotateRandomSin.h>
#include <Sonicteam/Spanverse/_acmRotateGoal.h>
#include <Sonicteam/Spanverse/_acmRotateRandomGoal.h>
#include <Sonicteam/Spanverse/_acmRotateAddGoal.h>
#include <Sonicteam/Spanverse/_acmRotateErase.h>
#include <Sonicteam/Spanverse/_acmScaleNormal.h>
#include <Sonicteam/Spanverse/_acmScaleAdd.h>
#include <Sonicteam/Spanverse/_acmScaleAccel.h>
#include <Sonicteam/Spanverse/_acmScaleRandomNormal.h>
#include <Sonicteam/Spanverse/_acmScaleRandomAdd.h>
#include <Sonicteam/Spanverse/_acmScaleRandomAccel.h>
#include <Sonicteam/Spanverse/_acmScaleSin.h>
#include <Sonicteam/Spanverse/_acmScaleRandomSin.h>
#include <Sonicteam/Spanverse/_acmScaleGoal.h>
#include <Sonicteam/Spanverse/_acmScaleRandomGoal.h>
#include <Sonicteam/Spanverse/_acmScaleAddGoal.h>
#include <Sonicteam/Spanverse/_acmPlaceFanShaped.h>
#include <Sonicteam/Spanverse/_acmPlaceLineShaped.h>
#include <Sonicteam/Spanverse/_acmPlaceSelectLocation.h>
#include <Sonicteam/Spanverse/_acmPlacePolygonShaped.h>
#include <Sonicteam/Spanverse/_acmCameraPosition.h>
#include <Sonicteam/Spanverse/_acmCameraTarget.h>
#include <Sonicteam/Spanverse/_acmCameraRoll.h>
#include <Sonicteam/Spanverse/_acmTurnCamera.h>
#include <Sonicteam/Spanverse/_acmCameraContactLens.h>
#include <Sonicteam/Spanverse/_acmUvScrollNormal.h>
#include <Sonicteam/Spanverse/_acmUvScrollByCameraDirection.h>
#include <Sonicteam/Spanverse/_acmPointLight.h>
#include <Sonicteam/Spanverse/_acmUvScrollSin.h>
#include <Sonicteam/Spanverse/_acmUvSet.h>
#include <Sonicteam/Spanverse/_acmDirectionalLight.h>
#include <Sonicteam/Spanverse/_acmAmbientLight.h>
#include <Sonicteam/Spanverse/_acmDetachCoordinate.h>
#include <Sonicteam/Spanverse/_acmMaterialColorNormal.h>
#include <Sonicteam/Spanverse/_acmMaterialColorSin.h>
#include <Sonicteam/Spanverse/_acmMaterialColorRandomNormal.h>
#include <Sonicteam/Spanverse/_acmMaterialColorRandomSin.h>
#include <Sonicteam/Spanverse/_acmMaterialColorGoal.h>
#include <Sonicteam/Spanverse/_acmMaterialColorRandomGoal.h>
#include <Sonicteam/Spanverse/_acmMaterialColorSetByCamDist.h>
#include <Sonicteam/Spanverse/_acmMaterialColorSetByCamDire.h>
#include <Sonicteam/Spanverse/_acmMotionSet.h>
#include <Sonicteam/Spanverse/_acmModelMotionSet.h>
#include <Sonicteam/Spanverse/_acmMotionFrameSet.h>
#include <Sonicteam/Spanverse/_acmParticleBillboardEx.h>
#include <Sonicteam/Spanverse/_acmParticleBillboardPV.h>
#include <Sonicteam/Spanverse/_acmParticleBillboardND.h>
#include <Sonicteam/Spanverse/_acmLineEx.h>
#include <Sonicteam/Spanverse/_acmSparklingTail.h>
#include <Sonicteam/Spanverse/_acmBlurBelt.h>
#include <Sonicteam/Spanverse/_acmBillboardTail.h>
#include <Sonicteam/Spanverse/_acmTexScreen.h>
#include <Sonicteam/Spanverse/_acmTextureSurfaceAnimation.h>
#include <Sonicteam/Spanverse/_acmTextureSurfaceAnimationEasy.h>
#include <Sonicteam/Spanverse/_acmTexturePatternAnimation.h>
#include <Sonicteam/Spanverse/_acmTexturePatternAnimationEasy.h>
#include <Sonicteam/Spanverse/_acmTextureSet.h>
#include <Sonicteam/Spanverse/_acmTextureListAnimation.h>
#include <Sonicteam/Spanverse/_acmScreenPerspective.h>
#include <Sonicteam/Spanverse/_acmTimeRate.h>
#include <Sonicteam/Spanverse/_acmThunder.h>
#include <Sonicteam/Spanverse/_acmLensFlare.h>
#include <Sonicteam/Spanverse/_acmSendParamNormal.h>
#include <Sonicteam/Spanverse/_acmSendParamAdd.h>
#include <Sonicteam/Spanverse/_acmSendParamAccel.h>
#include <Sonicteam/Spanverse/_acmSendParamSin.h>
#include <Sonicteam/Spanverse/_acmSendParamRandomNormal.h>
#include <Sonicteam/Spanverse/_acmSendParamRandomAdd.h>
#include <Sonicteam/Spanverse/_acmSendParamRandomAccel.h>
#include <Sonicteam/Spanverse/_acmSendParamRandomSin.h>
#include <Sonicteam/Spanverse/_acmSendParamGoal.h>
#include <Sonicteam/Spanverse/_acmSendParamAddGoal.h>
#include <Sonicteam/Spanverse/_acmSendParamRandomGoal.h>
#include <Sonicteam/Spanverse/_acmSendParamSpline.h>
#include <Sonicteam/Spanverse/_acmSendParamEqualSpace.h>
#include <Sonicteam/Spanverse/_acmModelDrawMultitude.h>
#include <Sonicteam/Spanverse/_acmSoundPlay.h>
#include <Sonicteam/Spanverse/_acmSound3DPlay.h>
#include <Sonicteam/Spanverse/_acmParticlePlay.h>
#include <Sonicteam/Spanverse/_acmParticleSet.h>
#include <Sonicteam/Spanverse/_acmCellSpriteSceneSet.h>
#include <Sonicteam/Spanverse/_acmSubtitle.h>
#include <Sonicteam/Spanverse/_acmClipPlane.h>
#include <Sonicteam/Spanverse/_acmModelJoin.h>
#include <Sonicteam/Spanverse/_acmShadowOn.h>
#include <Sonicteam/Spanverse/_acmFilterClassicBlur.h>
#include <Sonicteam/Spanverse/_acmFilterColorCorrection.h>
#include <Sonicteam/Spanverse/_acmFilterDepthOfField.h>

namespace Sonicteam::Spanverse
{
    // constructor 828F5BB8 
    class AckMomServer : public _ackBase
    {
    public:
        enum MomBase : uint32_t
        {
            MomBase__acmTranslateNormal = 0x0,
            MomBase__acmTranslateAdd = 0x1,
            MomBase__acmTranslateAccel = 0x2,
            MomBase__acmTranslateRandomNormal = 0x3,
            MomBase__acmTranslateRandomAdd = 0x4,
            MomBase__acmTranslateRandomAccel = 0x5,
            MomBase__acmTranslateSin = 0x6,
            MomBase__acmTranslateRandomSin = 0x7,
            MomBase__acmTranslateGoal = 0x8,
            MomBase__acmTranslateSpline = 0x9,
            MomBase__acmTranslateAddGoal = 0xA,
            MomBase__acmTranslateRandomGoal = 0xB,
            MomBase__acmTranslateWrap = 0x5C11,
            MomBase__acmRotateNormal = 0x14,
            MomBase__acmRotateAdd = 0x15,
            MomBase__acmRotateAccel = 0x16,
            MomBase__acmRotateRandomNormal = 0x17,
            MomBase__acmRotateRandomAdd = 0x18,
            MomBase__acmRotateRandomAccel = 0x19,
            MomBase__acmRotateSin = 0x1A,
            MomBase__acmRotateRandomSin = 0x1B,
            MomBase__acmRotateGoal = 0x1C,
            MomBase__acmRotateRandomGoal = 0x1D,
            MomBase__acmRotateAddGoal = 0x1E,
            MomBase__acmRotateErase = 0x1F,
            MomBase__acmScaleAdd = 0x29,
            MomBase__acmScaleAccel = 0x2A,
            MomBase__acmScaleAddGoal = 0x32,
            MomBase__acmScaleGoal = 0x30,
            MomBase__acmScaleNormal = 0x28,
            MomBase__acmScaleRandomAccel = 0x2D,
            MomBase__acmScaleRandomAdd = 0x2C,
            MomBase__acmScaleRandomGoal = 0x31,
            MomBase__acmScaleRandomNormal = 0x2B,
            MomBase__acmScaleRandomSin = 0x2F,
            MomBase__acmScaleSin = 0x2E,
            MomBase__acmPlaceFanShaped = 0x3C,
            MomBase__acmPlaceLineShaped = 0x3D,
            MomBase__acmPlaceSelectLocation = 0x3E,
            MomBase__acmPlacePolygonShaped = 0x3F,
            MomBase__acmCameraPosition = 0x1C2,
            MomBase__acmCameraTarget = 0x1C3,
            MomBase__acmCameraRoll = 0x1C4,
            MomBase__acmTurnCamera = 0x96,
            MomBase__acmCameraContactLens = 0x264,
            MomBase__acmUvScrollByCameraDirection = 0x12C,
            MomBase__acmPointLight = 0x17C,
            MomBase__acmDirectionalLight = 0x17D,
            MomBase__acmAmbientLight = 0x17E,
            MomBase__acmDetachCoordinate = 0x25A,
            MomBase__acmUvScrollSin = 0x12E,
            MomBase__acmUvScrollNormal = 0x12D,
            MomBase__acmUvSet = 0x12F,
            MomBase__acmMaterialColorNormal = 0xC8,
            MomBase__acmMaterialColorSin = 0xC9,
            MomBase__acmMaterialColorRandomNormal = 0xCA,
            MomBase__acmMaterialColorRandomSin = 0xCB,
            MomBase__acmMaterialColorGoal = 0xCC,
            MomBase__acmMaterialColorRandomGoal = 0xCD,
            MomBase__acmMaterialColorSetByCamDist = 0xCE,
            MomBase__acmMaterialColorSetByCamDire = 0xCF,
            MomBase__acmMotionFrameSet = 0x1F6,
            MomBase__acmMotionSet = 0x1F4,
            MomBase__acmModelMotionSet = 0x1F5,
            MomBase__acmParticleBillboardEx = 0x65,
            MomBase__acmParticleBillboardPV = 0x67,
            MomBase__acmParticleBillboardND = 0x6A,
            MomBase__acmBlurBelt = 0x78,
            MomBase__acmSparklingTail = 0x69,
            MomBase__acmBillboardTail = 0x7A,
            MomBase__acmTexScreen = 0x263,
            MomBase__acmLineEx = 0x66,
            MomBase__acmTextureSurfaceAnimation = 0xFA,
            MomBase__acmTextureSurfaceAnimationEasy = 0xFD,
            MomBase__acmTextureSet = 0x100,
            MomBase__acmTexturePatternAnimation = 0xFB,
            MomBase__acmTexturePatternAnimationEasy = 0xFE,
            MomBase__acmTextureListAnimation = 0xFF,
            MomBase__acmScreenPerspective = 0x226,
            MomBase__acmTimeRate = 0x28A,
            MomBase__acmThunder = 0x258,
            MomBase__acmLensFlare = 0x265,
            MomBase__acmSendParamAccel = 0x2BE,
            MomBase__acmSendParamAdd = 0x2BD,
            MomBase__acmSendParamAddGoal = 0x2C5,
            MomBase__acmSendParamEqualSpace = 0x2C8,
            MomBase__acmSendParamGoal = 0x2C4,
            MomBase__acmSendParamNormal = 0x2BC,
            MomBase__acmSendParamRandomAccel = 0x2C2,
            MomBase__acmSendParamRandomAdd = 0x2C1,
            MomBase__acmSendParamRandomGoal = 0x2C7,
            MomBase__acmSendParamRandomNormal = 0x2C0,
            MomBase__acmSendParamRandomSin = 0x2C3,
            MomBase__acmSendParamSin = 0x2BF,
            MomBase__acmSendParamSpline = 0x2C6,
            MomBase__acmModelDrawMultitude = 0x269,
            MomBase__acmSoundPlay = 0x320, //+
            MomBase__acmSound3DPlay = 0x321,
            MomBase__acmParticlePlay = 0x32A,
            MomBase__acmParticleSet = 0x32B,
            MomBase__acmCellSpriteSceneSet = 0x334,
            MomBase__acmSubtitle = 0x33E,
            MomBase__acmClipPlane = 0x348,
            MomBase__acmModelJoin = 0xE7,
            MomBase__acmShadowOn = 0xE8,
            MomBase__acmFilterClassicBlur = 0x352,
            MomBase__acmFilterColorCorrection = 0x353,
            MomBase__acmFilterDepthOfField = 0x354,
            MomBase__acmSceneBloom = 0x366
        };

        static constexpr uint32_t ms_Signature = 0x414B4D53; // AKMS
        xpointer<_ackMomBase> m_aBases[5];
    };

    MARATHON_ASSERT_OFFSETOF(AckMomServer, m_aBases, 0x10);
    MARATHON_ASSERT_SIZEOF(AckMomServer, 0x24);
}
