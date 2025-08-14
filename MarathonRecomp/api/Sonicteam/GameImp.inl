namespace Sonicteam
{
    template <typename T>
    inline T* GameImp::GetPhysicsWorld()
    {
        return (T*)m_pPhysicsWorld.get();
    }
}
