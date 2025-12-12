#pragma once

namespace Sonicteam
{
    inline Actor* ActorManager::FindActor(uint32_t ActorID)
    {
        unsigned int left = 0;
        unsigned int right = this->m_LastActorIndex - 1;
        int Index = 0;

        while (left <= right) {
            unsigned int mid = left + (right - left) / 2;
            if (this->m_ActorID[mid] == ActorID) {
                Index = mid;
                break;
            }
            if (this->m_ActorID[mid] < ActorID) {
                left = mid + 1;
            }
            else {
                right = mid - 1;
            }
        }
        if (!this->m_Actor[Index]) return 0;
        return this->m_Actor[Index];
    }
}
