namespace Sonicteam
{
    inline AppMarathon* AppMarathon::GetInstance()
    {
        return *(xpointer<AppMarathon>*)MmGetHostAddress(0x82D3B348);
    }
}
