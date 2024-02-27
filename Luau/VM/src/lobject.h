// This file is part of the Luau programming language and is licensed under MIT License; see LICENSE.txt for details
// This code is based on Lua 5.x implementation licensed under MIT License; see lua_LICENSE.txt for details
#pragma once

#include "lsecurity.hpp"
#include "lua.h"
#include "lcommon.h"

LUAU_FASTFLAG(LuauTaggedLuData)

/*
** Union of all collectible objects
*/
typedef union GCObject GCObject;

/*
** Common Header for all collectible objects (in macro form, to be included in other objects)
*/
// clang-format off
#define CommonHeader \
     LUAVM_SHUFFLE3(LUAVM_SEMICOLON_SEP, uint8_t tt, uint8_t marked, uint8_t memcat)
// clang-format on

/*
** Common header in struct form
*/
typedef struct GCheader
{
    CommonHeader;
} GCheader;

/*
** Union of all Lua values
*/
typedef union
{
    GCObject* gc;
    void* p;
    double n;
    int b;
    float v[2]; // v[0], v[1] live here; v[2] lives in TValue::extra
} Value;

/*
** Tagged Values
*/

typedef struct lua_TValue
{
    Value value;
    int extra[LUA_EXTRA_SIZE];
    int tt;
} TValue;

// Macros to test type
#define ttisnil(o) (ttype(o) == LUA_TNIL)
#define ttisnumber(o) (ttype(o) == LUA_TNUMBER)
#define ttisstring(o) (ttype(o) == LUA_TSTRING)
#define ttistable(o) (ttype(o) == LUA_TTABLE)
#define ttisfunction(o) (ttype(o) == LUA_TFUNCTION)
#define ttisboolean(o) (ttype(o) == LUA_TBOOLEAN)
#define ttisuserdata(o) (ttype(o) == LUA_TUSERDATA)
#define ttisthread(o) (ttype(o) == LUA_TTHREAD)
#define ttisbuffer(o) (ttype(o) == LUA_TBUFFER)
#define ttislightuserdata(o) (ttype(o) == LUA_TLIGHTUSERDATA)
#define ttisvector(o) (ttype(o) == LUA_TVECTOR)
#define ttisupval(o) (ttype(o) == LUA_TUPVAL)

// Macros to access values
#define ttype(o) ((o)->tt)
#define gcvalue(o) check_exp(iscollectable(o), (o)->value.gc)
#define pvalue(o) check_exp(ttislightuserdata(o), (o)->value.p)
#define nvalue(o) check_exp(ttisnumber(o), (o)->value.n)
#define vvalue(o) check_exp(ttisvector(o), (o)->value.v)
#define tsvalue(o) check_exp(ttisstring(o), &(o)->value.gc->ts)
#define uvalue(o) check_exp(ttisuserdata(o), &(o)->value.gc->u)
#define clvalue(o) check_exp(ttisfunction(o), &(o)->value.gc->cl)
#define hvalue(o) check_exp(ttistable(o), &(o)->value.gc->h)
#define bvalue(o) check_exp(ttisboolean(o), (o)->value.b)
#define thvalue(o) check_exp(ttisthread(o), &(o)->value.gc->th)
#define bufvalue(o) check_exp(ttisbuffer(o), &(o)->value.gc->buf)
#define upvalue(o) check_exp(ttisupval(o), &(o)->value.gc->uv)

#define l_isfalse(o) (ttisnil(o) || (ttisboolean(o) && bvalue(o) == 0))

#define lightuserdatatag(o) check_exp(ttislightuserdata(o), (o)->extra[0])

// Internal tags used by the VM
#define LU_TAG_ITERATOR LUA_UTAG_LIMIT

/*
** for internal debug only
*/
#define checkconsistency(obj) LUAU_ASSERT(!iscollectable(obj) || (ttype(obj) == (obj)->value.gc->gch.tt))

#define checkliveness(g, obj) LUAU_ASSERT(!iscollectable(obj) || ((ttype(obj) == (obj)->value.gc->gch.tt) && !isdead(g, (obj)->value.gc)))

// Macros to set values
#define setnilvalue(obj) ((obj)->tt = LUA_TNIL)

#define setnvalue(obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.n = (x); \
        i_o->tt = LUA_TNUMBER; \
    }

#if LUA_VECTOR_SIZE == 4
#define setvvalue(obj, x, y, z, w) \
    { \
        TValue* i_o = (obj); \
        float* i_v = i_o->value.v; \
        i_v[0] = (x); \
        i_v[1] = (y); \
        i_v[2] = (z); \
        i_v[3] = (w); \
        i_o->tt = LUA_TVECTOR; \
    }
#else
#define setvvalue(obj, x, y, z, w) \
    { \
        TValue* i_o = (obj); \
        float* i_v = i_o->value.v; \
        i_v[0] = (x); \
        i_v[1] = (y); \
        i_v[2] = (z); \
        i_o->tt = LUA_TVECTOR; \
    }
#endif

#define setpvalue(obj, x, tag) \
    { \
        TValue* i_o = (obj); \
        i_o->value.p = (x); \
        i_o->extra[0] = (tag); \
        i_o->tt = LUA_TLIGHTUSERDATA; \
    }

#define setbvalue(obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.b = (x); \
        i_o->tt = LUA_TBOOLEAN; \
    }

#define setsvalue(L, obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.gc = cast_to(GCObject*, (x)); \
        i_o->tt = LUA_TSTRING; \
        checkliveness(L->global, i_o); \
    }

#define setuvalue(L, obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.gc = cast_to(GCObject*, (x)); \
        i_o->tt = LUA_TUSERDATA; \
        checkliveness(L->global, i_o); \
    }

#define setthvalue(L, obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.gc = cast_to(GCObject*, (x)); \
        i_o->tt = LUA_TTHREAD; \
        checkliveness(L->global, i_o); \
    }

#define setbufvalue(L, obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.gc = cast_to(GCObject*, (x)); \
        i_o->tt = LUA_TBUFFER; \
        checkliveness(L->global, i_o); \
    }

#define setclvalue(L, obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.gc = cast_to(GCObject*, (x)); \
        i_o->tt = LUA_TFUNCTION; \
        checkliveness(L->global, i_o); \
    }

#define sethvalue(L, obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.gc = cast_to(GCObject*, (x)); \
        i_o->tt = LUA_TTABLE; \
        checkliveness(L->global, i_o); \
    }

#define setptvalue(L, obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.gc = cast_to(GCObject*, (x)); \
        i_o->tt = LUA_TPROTO; \
        checkliveness(L->global, i_o); \
    }

#define setupvalue(L, obj, x) \
    { \
        TValue* i_o = (obj); \
        i_o->value.gc = cast_to(GCObject*, (x)); \
        i_o->tt = LUA_TUPVAL; \
        checkliveness(L->global, i_o); \
    }

#define setobj(L, obj1, obj2) \
    { \
        const TValue* o2 = (obj2); \
        TValue* o1 = (obj1); \
        *o1 = *o2; \
        checkliveness(L->global, o1); \
    }

/*
** different types of sets, according to destination
*/

// to stack
#define setobj2s setobj
// from table to same table (no barrier)
#define setobjt2t setobj
// to table (needs barrier)
#define setobj2t setobj
// to new object (no barrier)
#define setobj2n setobj

#define setttype(obj, tt) (ttype(obj) = (tt))

#define iscollectable(o) (ttype(o) >= LUA_TSTRING)

typedef TValue* StkId; // index to stack elements

/*
** String headers for string table
*/
typedef struct TString
{
    CommonHeader;
    // 1 byte padding

    int16_t atom;

    // 2 byte padding

    TString* next; // next string in the hash table bucket

    RBX_VMVALUE_SUB_P_X<unsigned int> hash; // NOTE: this is vmvalued encrypted aka ptr encrypted
    RBX_VMVALUE_SUB_X_P<unsigned int> len; // NOTE: this is vmvalued encrypted aka ptr encrypted

    char data[1]; // string data is allocated right after the header
} TString;


#define getstr(ts) (ts)->data
#define svalue(o) getstr(tsvalue(o))

typedef struct Udata
{
    CommonHeader;

    uint8_t tag;

    int len;

    struct Table* metatable; // NOTE: this is vmvalued encrypted aka ptr encrypted

    union
    {
        char data[1];      // userdata is allocated right after the header
        L_Umaxalign dummy; // ensures maximum alignment for data
    };
} Udata;

/* I suggest you looking into this making sure there is now encryptions found or shuffles found */
typedef struct Buffer
{
    CommonHeader;

    unsigned int len;

    union
    {
        char data[1];      // buffer is allocated right after the header
        L_Umaxalign dummy; // ensures maximum alignment for data
    };
} Buffer;

/*
** Function Prototypes
*/
// clang-format off
typedef struct Proto
{
    CommonHeader;

    LUAVM_SHUFFLE5(LUAVM_SEMICOLON_SEP,
    uint8_t nups, // number of upvalues
    uint8_t numparams,
    uint8_t is_vararg,
    uint8_t maxstacksize,
    uint8_t flags);

    // NOTE: all these are part of a encryption family called pMember1Enc (all 4 of these have the same encryption)
    LUAVM_SHUFFLE4(LUAVM_SEMICOLON_SEP,
    RBX_VMVALUE_XOR<TValue*> k,              // constants used by the function
    RBX_VMVALUE_XOR<Instruction*> code,      // function bytecode
    RBX_VMVALUE_XOR<struct Proto**> p,       // functions defined inside the function
    RBX_VMVALUE_XOR<const Instruction*> codeentry);

    void* execdata;
    uintptr_t exectarget;

    // NOTE: all these are part of a encryption family called pMember2Enc (all 5 of these have the same encryption)
    LUAVM_SHUFFLE5(LUAVM_SEMICOLON_SEP,
    RBX_VMVALUE_SUB_P_X<uint8_t*> lineinfo,      // for each instruction, line number as a delta from baseline
    RBX_VMVALUE_SUB_P_X<int*> abslineinfo,       // baseline line info, one entry for each 1<<linegaplog2 instructions; allocated after lineinfo
    RBX_VMVALUE_SUB_P_X<struct LocVar*> locvars, // information about local variables
    RBX_VMVALUE_SUB_P_X<TString**> upvalues,     // upvalue names
    RBX_VMVALUE_SUB_P_X<TString*> source);

    RBX_VMVALUE_XOR<TString*> debugname; // NOTE: this is vmvalued encrypted aka ptr encrypted
    uint8_t* debuginsn; // a copy of code[] array with just opcodes | NOTE: this is vmvalued encrypted aka ptr encrypted

    RBX_VMVALUE_ADD<uint8_t*> typeinfo; // NOTE: this is vmvalued encrypted aka ptr encrypted

    void* userdata;
    GCObject* gclist;

    LUAVM_SHUFFLE9(LUAVM_SEMICOLON_SEP,
    int sizecode,
    int sizep,
    int sizelocvars,
    int sizeupvalues,
    int sizek,
    int sizelineinfo,
    int linegaplog2,
    int linedefined,
    int bytecodeid);
} Proto;
// clang-format on

typedef struct LocVar
{
    TString* varname;
    int startpc; // first point where variable is active
    int endpc;   // first point where variable is dead
    uint8_t reg; // register slot, relative to base, where variable is stored
} LocVar;

/*
** Upvalues
*/

typedef struct UpVal
{
    CommonHeader;
    uint8_t markedopen; // set if reachable from an alive thread (only valid during atomic)

    // 4 byte padding (x64)

    TValue* v; // points to stack or to its own value
    union
    {
        TValue value; // the value (when closed)
        struct
        {
            // global double linked list (when open)
            struct UpVal* prev;
            struct UpVal* next;

            // thread linked list (when open)
            struct UpVal* threadnext;
        } open;
    } u;
} UpVal;

#define upisopen(up) ((up)->v != &(up)->u.value)

/*
** Closures
*/

typedef struct Closure
{
    CommonHeader;

    uint8_t isC;
    uint8_t nupvalues;
    uint8_t stacksize;
    uint8_t preload;

    GCObject* gclist;
    struct Table* env;

    union
    {
        struct
        {
            /* all three are different besides p and f being the same */
            lua_CFunction f; // NOTE: this is vmvalued encrypted aka ptr encrypted (same as p)
            lua_Continuation cont; // NOTE: this is vmvalued encrypted aka ptr encrypted
            const char* debugname; // NOTE: this is vmvalued encrypted aka ptr encrypted
            TValue upvals[1];
        } c;

        struct
        {
            RBX_VMVALUE_SUB_X_P<Proto*> p; // NOTE: this is vmvalued encrypted aka ptr encrypted (same as f)
            TValue uprefs[1];
        } l;
    };
} Closure;

#define iscfunction(o) (ttype(o) == LUA_TFUNCTION && clvalue(o)->isC)
#define isLfunction(o) (ttype(o) == LUA_TFUNCTION && !clvalue(o)->isC)

/*
** Tables
*/

typedef struct TKey
{
    ::Value value;
    int extra[LUA_EXTRA_SIZE];
    unsigned tt : 4;
    int next : 28; // for chaining
} TKey;

typedef struct LuaNode
{
    TValue val;
    TKey key;
} LuaNode;

// copy a value into a key
#define setnodekey(L, node, obj) \
    { \
        LuaNode* n_ = (node); \
        const TValue* i_o = (obj); \
        n_->key.value = i_o->value; \
        memcpy(n_->key.extra, i_o->extra, sizeof(n_->key.extra)); \
        n_->key.tt = i_o->tt; \
        checkliveness(L->global, i_o); \
    }

// copy a value from a key
#define getnodekey(L, obj, node) \
    { \
        TValue* i_o = (obj); \
        const LuaNode* n_ = (node); \
        i_o->value = n_->key.value; \
        memcpy(i_o->extra, n_->key.extra, sizeof(i_o->extra)); \
        i_o->tt = n_->key.tt; \
        checkliveness(L->global, i_o); \
    }

// clang-format off
typedef struct Table
{
    CommonHeader;

    LUAVM_SHUFFLE5(LUAVM_SEMICOLON_SEP,
    uint8_t tmcache,    // 1<<p means tagmethod(p) is not present
    uint8_t readonly,   // sandboxing feature to prohibit writes to table
    uint8_t safeenv,    // environment doesn't share globals with other scripts
    uint8_t lsizenode,  // log2 of size of `node' array
    uint8_t nodemask8); // (1<<lsizenode)-1, truncated to 8 bits

    int sizearray; // size of `array' array
    union
    {
        int lastfree;  // any free position is before this position
        int aboundary; // negated 'boundary' of `array' array; iff aboundary < 0
    };

    /* these 4 are part of the same encryption group called tMemberEnc (they have the possibility to change outside of the group) */
    LUAVM_SHUFFLE4(LUAVM_SEMICOLON_SEP,
    RBX_VMVALUE_SUB_X_P<struct Table*> metatable,
    RBX_VMVALUE_SUB_X_P<TValue*> array,  // array part
    RBX_VMVALUE_SUB_X_P<LuaNode*> node,
    RBX_VMVALUE_SUB_X_P<GCObject*> gclist);
} Table;
// clang-format on

/*
** `module' operation for hashing (size is always a power of 2)
*/
#define lmod(s, size) (check_exp((size & (size - 1)) == 0, (cast_to(int, (s) & ((size)-1)))))

#define twoto(x) ((int)(1 << (x)))
#define sizenode(t) (twoto((t)->lsizenode))

inline TValue* rluaO_nilobject = nullptr;
#define luaO_nilobject rluaO_nilobject

inline LuaNode* rluaH_dummynode = nullptr;
#define dummynode rluaH_dummynode

// #define luaO_nilobject (&luaO_nilobject_)
// LUAI_DATA const TValue luaO_nilobject_;

#define ceillog2(x) (luaO_log2((x)-1) + 1)

LUAI_FUNC int luaO_log2(unsigned int x);
LUAI_FUNC int luaO_rawequalObj(const TValue* t1, const TValue* t2);
LUAI_FUNC int luaO_rawequalKey(const TKey* t1, const TValue* t2);
LUAI_FUNC int luaO_str2d(const char* s, double* result);
LUAI_FUNC const char* luaO_pushvfstring(lua_State* L, const char* fmt, va_list argp);
LUAI_FUNC const char* luaO_pushfstring(lua_State* L, const char* fmt, ...);
LUAI_FUNC const char* luaO_chunkid(char* buf, size_t buflen, const char* source, size_t srclen);
