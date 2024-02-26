#pragma once
#include <cstdint>
#pragma warning(disable: 4003) // not enough actual parameters for macro 'identifier' warning

#define LUAVM_COMMA_SEP ,
#define LUAVM_SEMICOLON_SEP ;
#define LUAVM_SHUFFLE2(sep,a0,a1) a1 sep a0
#define LUAVM_SHUFFLE3(sep,a0,a1,a2) a1 sep a2 sep a0
#define LUAVM_SHUFFLE4(sep,a0,a1,a2,a3) a3 sep a1 sep a0 sep a2
#define LUAVM_SHUFFLE5(sep,a0,a1,a2,a3,a4) a0 sep a1 sep a3 sep a2 sep a4
#define LUAVM_SHUFFLE6(sep,a0,a1,a2,a3,a4,a5) a0 sep a3 sep a2 sep a5 sep a1 sep a4
#define LUAVM_SHUFFLE7(sep,a0,a1,a2,a3,a4,a5,a6) a2 sep a3 sep a0 sep a4 sep a6 sep a1 sep a5
#define LUAVM_SHUFFLE8(sep,a0,a1,a2,a3,a4,a5,a6,a7) a7 sep a0 sep a5 sep a6 sep a3 sep a1 sep a2 sep a4
#define LUAVM_SHUFFLE9(sep,a0,a1,a2,a3,a4,a5,a6,a7,a8) a2 sep a6 sep a4 sep a7 sep a1 sep a8 sep a0 sep a3 sep a5

/*
* PtrObf implementation
* author: uniquadev
*/
#define DEFINE_VMVALUE(name, obfBlock, deobfBlock) \
    template <typename T> class RBX_VMVALUE_##name \
    { \
    public: \
        operator const T() const deobfBlock \
        const T operator->() { \
            return operator const T(); \
        } \
        void operator=(const T& value) obfBlock \
    protected: \
        T storage; \
    }; \

DEFINE_VMVALUE(ADD,
{
    storage = (T)((uintptr_t)value - (uintptr_t)this);
},
{
    return (T)((uintptr_t)this->storage + (uintptr_t)this);
});

DEFINE_VMVALUE(SUB_X_P,
{
    storage = (T)((uintptr_t)this + (uintptr_t)value);
},
{
    return (T)((uintptr_t)this->storage - (uintptr_t)this);
});

DEFINE_VMVALUE(SUB_P_X,
{
    storage = (T)((uintptr_t)this - (uintptr_t)value);
},
{
    return (T)((uintptr_t)this - (uintptr_t)this->storage);
});

DEFINE_VMVALUE(XOR,
{
    storage = (T)((uintptr_t)value ^ (uintptr_t)this);
},
{
    return (T)((uintptr_t)this->storage ^ (uintptr_t)this);
});

// new implementation of their obfuscation
// ex: l_global = L + *(_QWORD *)(L + 32);
// pseudo c: l_global = L + *(uintptr_t*)(L->global);

#define DEFINE_VMVALUE_BASE_SUB(name, fieldName) \
    template <typename T, typename C> class RBX_VMVALUE_BASE_SUB_##name \
    { \
    public: \
        operator const T() const \
        { \
            constexpr uintptr_t OFFSET = offsetof(C, fieldName); \
            return (T)((uintptr_t)this->storage + ((uintptr_t)this - OFFSET)); \
        } \
        const T operator->() { \
            return operator const T(); \
        } \
        void operator=(const T& value) \
        { \
            constexpr uintptr_t OFFSET = offsetof(C, fieldName); \
            storage = (T)((uintptr_t)value - ((uintptr_t)this - OFFSET)); \
        } \
    protected: \
        T storage; \
    }