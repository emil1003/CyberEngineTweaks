#include <stdafx.h>

#include "WeakReference.h"
#include "RTTILocator.h"

#include "CET.h"

static RTTILocator s_sIScriptableType{RED4ext::FNV1a("IScriptable")};

WeakReference::WeakReference(const TiltedPhoques::Lockable<sol::state, std::recursive_mutex>::Ref& aView,
                             RED4ext::WeakHandle<RED4ext::IScriptable> aWeakHandle)
    : ClassType(aView, nullptr)
    , m_weakHandle(std::move(aWeakHandle))
{
    auto ref = m_weakHandle.Lock();
    if (ref)
    {
        m_pType = ref->GetType();
    }
}

WeakReference::WeakReference(const TiltedPhoques::Lockable<sol::state, std::recursive_mutex>::Ref& aView,
                             RED4ext::WeakHandle<RED4ext::IScriptable> aWeakHandle,
                             RED4ext::CWeakHandle* apWeakHandleType)
    : ClassType(aView, nullptr)
    , m_weakHandle(std::move(aWeakHandle))
{
    auto ref = m_weakHandle.Lock();
    if (ref)
    {
        auto const cpClass = reinterpret_cast<RED4ext::CClass*>(apWeakHandleType->GetInnerType());

        m_pType = cpClass->IsA(s_sIScriptableType) ? ref->GetType() : cpClass;
    }
}

WeakReference::~WeakReference()
{
    // Nasty hack so that the Lua VM doesn't try to release game memory on shutdown
    if (!CET::IsRunning())
    {
        m_weakHandle.instance = nullptr;
        m_weakHandle.refCount = nullptr;
    }
}


RED4ext::ScriptInstance WeakReference::GetHandle() const
{
    const auto ref = m_weakHandle.Lock();
    if (ref)
    {
        return m_weakHandle.instance;
    }

    return nullptr;
}

RED4ext::ScriptInstance WeakReference::GetValuePtr() const
{
    return const_cast<RED4ext::WeakHandle<RED4ext::IScriptable>*>(&m_weakHandle);
}
