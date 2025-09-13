namespace Sonicteam::SoX
{
    inline ApplicationXenon* ApplicationXenon::GetInstance()
    {
        return *(xpointer<ApplicationXenon>*)MmGetHostAddress(0x82D3B348);
    }
}
