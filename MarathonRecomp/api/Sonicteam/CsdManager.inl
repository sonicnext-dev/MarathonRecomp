namespace Sonicteam
{
    inline CsdManager* CsdManager::GetInstance()
    {
        return *(xpointer<CsdManager>*)MmGetHostAddress(0x82D3BC58);
    }
}
