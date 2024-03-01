#pragma once
#include <cstdint>
#pragma warning(disable: 4003) // not enough actual parameters for macro 'identifier' warning
#pragma warning(disable: 4172) // returning address of local variable or temporary

/*
// BACKUP
inline TValue* rluaO_nilobject = nullptr;
#define luaO_nilobject rluaO_nilobject

inline LuaNode* rluaH_dummynode = nullptr;
#define dummynode rluaH_dummynode
*/

#define PMEMBER1_VMVALUE VMVALUE_SUB_X_P
#define PMEMBER2_VMVALUE VMVALUE_ADD
#define PDEBUGNAME_VMVALUE VMVALUE_SUB_X_P
#define PDEBUGINSN_VMVALUE VMVALUE_XOR
#define PTYPE_INFO_VMVALUE VMVALUE_SUB_P_X

#define LGLOBAL_VMVALUE VMVALUE_XOR
#define LSTACKSIZE_VMVALUE VMVALUE_XOR

#define CPF_VMVALUE VMVALUE_SUB_P_X
#define CDEBUGNAME_VMVALUE VMVALUE_XOR
#define CCONT_VMVALUE VMVALUE_SUB_X_P

#define TMEMBER1_VMVALUE VMVALUE_SUB_P_X   // usually the same of METATABLE_VMVALUE
#define METATABLE_VMVALUE VMVALUE_SUB_P_X

#define TSHASH_VMVALUE VMVALUE_ADD
#define TSLEN_VMVALUE VMVALUE_SUB_P_X
// todo
#define GTTNAME_VMVALUE VMVALUE_SUB_P_X
#define GTMNAME_VMVALUE VMVALUE_SUB_P_X

#define LUAVM_COMMA_SEP ,
#define LUAVM_SEMICOLON_SEP ;
#define LUAVM_SHUFFLE2(s,a0,a1) a1 s a0
#define LUAVM_SHUFFLE3(s,a0,a1,a2) a2 s a1 s a0
#define LUAVM_SHUFFLE4(s,a0,a1,a2,a3) a1 s a3 s a2 s a0 
#define LUAVM_SHUFFLE5(s,a0,a1,a2,a3,a4) a2 s a4 s a3 s a0 s a1
#define LUAVM_SHUFFLE6(s,a0,a1,a2,a3,a4,a5) a1 s a0 s a3 s a5 s a4 s a2
#define LUAVM_SHUFFLE7(s,a0,a1,a2,a3,a4,a5,a6) a1 s a3 s a4 s a6 s a5 s a2 s a0
#define LUAVM_SHUFFLE8(s,a0,a1,a2,a3,a4,a5,a6,a7) a5 s a3 s a6 s a1 s a7 s a0 s a4 s a2
#define LUAVM_SHUFFLE9(s,a0,a1,a2,a3,a4,a5,a6,a7,a8) a4 s a2 s a3 s a6 s a7 s a8 s a1 s a5 s a0
/*
* PtrObf implementation
* author: uniquadev
*/
#define DEFINE_VMVALUE(name, obfBlock, deobfBlock) \
    template <typename T> class VMVALUE_##name \
    { \
    public: \
        operator const T() const deobfBlock \
        const T operator->() { \
            return operator const T(); \
        } \
        T* operator&() { \
            T value = operator const T(); \
            return &value; \
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