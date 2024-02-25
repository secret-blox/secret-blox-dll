/*
* Example usage from 2016 source:
* LUAVM_SHUFFLE2(;,
*  unsigned short nCcalls,
*  unsigned short baseCcalls
* );
*/

// just for reference
#define LUAVM_COMMA_SEP ,
#define LUAVM_SEMICOLON_SEP ;
#define LUAVM_SHUFFLE2(sep,a0,a1) a1 sep a0
#define LUAVM_SHUFFLE3(sep,a0,a1,a2) a1 sep a2 sep a0
#define LUAVM_SHUFFLE4(sep,a0,a1,a2,a3) a3 sep a1 sep a0 sep a2
#define LUAVM_SHUFFLE5(sep,a0,a1,a2,a3,a4) a4 sep a0 sep a2 sep a1 sep a3
#define LUAVM_SHUFFLE6(sep,a0,a1,a2,a3,a4,a5) a3 sep a5 sep a2 sep a0 sep a1 sep a4
#define LUAVM_SHUFFLE7(sep,a0,a1,a2,a3,a4,a5,a6) a2 sep a3 sep a0 sep a4 sep a6 sep a1 sep a5
#define LUAVM_SHUFFLE8(sep,a0,a1,a2,a3,a4,a5,a6,a7) a7 sep a0 sep a5 sep a6 sep a3 sep a1 sep a2 sep a4
#define LUAVM_SHUFFLE9(sep,a0,a1,a2,a3,a4,a5,a6,a7,a8) a2 sep a6 sep a4 sep a7 sep a1 sep a8 sep a0 sep a3 sep a5

/*
* PtrObf implementation
* author: uniquadev
*/
#define COMMON_PTROBF_OPERATORS using RBX_PTROBF<T>::operator=; using RBX_PTROBF<T>::operator->; using RBX_PTROBF<T>::operator const T
template <typename T> class RBX_PTROBF
{
public:
    operator const T() const {
        return deobf();
    }
    const T operator->() {
        return deobf();
    }

    void operator=(const T& value) {
        storage = obf(value);
    }

    virtual T deobf() const = 0;
    virtual T obf(const T& value) const  = 0;
protected:
    T storage;
};

template <typename T> class RBX_PTROBF_ADD : RBX_PTROBF<T>
{
public:
    COMMON_PTROBF_OPERATORS;

    T deobf() const override {
        return (T)((uintptr_t)this->storage + (uintptr_t)this);
    }

    T obf(const T& value) const override {
        return (T)((uintptr_t)value - (uintptr_t)this);
    }
};

template <typename T> class RBX_PTROBF_SUB_X_P : RBX_PTROBF<T>
{
public:
    COMMON_PTROBF_OPERATORS;

    T deobf() const override {
        return (T)((uintptr_t)this->storage - (uintptr_t)this);
    }

    T obf(const T& value) const override {
        return (T)((uintptr_t)this + (uintptr_t)value);
    }
};

template <typename T> class RBX_PTROBF_SUB_P_X : RBX_PTROBF<T>
{
public:
    COMMON_PTROBF_OPERATORS;

    T deobf() const override {
        return (T)((uintptr_t)this - (uintptr_t)this->storage);
    }

    T obf(const T& value) const override {
        return (T)((uintptr_t)this - (uintptr_t)value);
    }
};

template <typename T> class RBX_PTROBF_XOR : RBX_PTROBF<T>
{
public:
    COMMON_PTROBF_OPERATORS;

    T deobf() const override {
        return (T)((uintptr_t)this->storage ^ (uintptr_t)this);
    }

    T obf(const T& value) const override {
        return (T)((uintptr_t)value ^ (uintptr_t)this);
    }
};