# Luau.Common Sources
# Note: Until 3.19, INTERFACE targets couldn't have SOURCES property set
if(NOT ${CMAKE_VERSION} VERSION_LESS "3.19")
    target_sources(Luau.Common PRIVATE
        Common/include/Luau/Common.h
        Common/include/Luau/Bytecode.h
        Common/include/Luau/BytecodeUtils.h
        Common/include/Luau/DenseHash.h
        Common/include/Luau/ExperimentalFlags.h
        Common/include/Luau/VecDeque.h
    )
endif()

# Luau.Ast Sources
target_sources(Luau.Ast PRIVATE
    Ast/include/Luau/Ast.h
    Ast/include/Luau/Confusables.h
    Ast/include/Luau/Lexer.h
    Ast/include/Luau/Location.h
    Ast/include/Luau/ParseOptions.h
    Ast/include/Luau/Parser.h
    Ast/include/Luau/ParseResult.h
    Ast/include/Luau/StringUtils.h
    Ast/include/Luau/TimeTrace.h

    Ast/src/Ast.cpp
    Ast/src/Confusables.cpp
    Ast/src/Lexer.cpp
    Ast/src/Location.cpp
    Ast/src/Parser.cpp
    Ast/src/StringUtils.cpp
    Ast/src/TimeTrace.cpp
)

# Luau.Compiler Sources
target_sources(Luau.Compiler PRIVATE
    Compiler/include/Luau/BytecodeBuilder.h
    Compiler/include/Luau/Compiler.h
    Compiler/include/luacode.h

    Compiler/src/BytecodeBuilder.cpp
    Compiler/src/Compiler.cpp
    Compiler/src/Builtins.cpp
    Compiler/src/BuiltinFolding.cpp
    Compiler/src/ConstantFolding.cpp
    Compiler/src/CostModel.cpp
    Compiler/src/TableShape.cpp
    Compiler/src/Types.cpp
    Compiler/src/ValueTracking.cpp
    Compiler/src/lcode.cpp
    Compiler/src/Builtins.h
    Compiler/src/BuiltinFolding.h
    Compiler/src/ConstantFolding.h
    Compiler/src/CostModel.h
    Compiler/src/TableShape.h
    Compiler/src/Types.h
    Compiler/src/ValueTracking.h
)

# Luau.VM Sources
target_sources(Luau.VM PRIVATE
    VM/include/lsecurity.hpp # This is our custom header

    VM/include/lua.h
    VM/include/luaconf.h
    VM/include/lualib.h

    VM/src/lapi.cpp
    VM/src/laux.cpp
    VM/src/lbaselib.cpp
    VM/src/lbitlib.cpp
    VM/src/lbuffer.cpp
    VM/src/lbuflib.cpp
    VM/src/lbuiltins.cpp
    VM/src/lcorolib.cpp
    VM/src/ldblib.cpp
    VM/src/ldebug.cpp
    VM/src/ldo.cpp
    VM/src/lfunc.cpp
    VM/src/lgc.cpp
    VM/src/lgcdebug.cpp
    VM/src/linit.cpp
    VM/src/lmathlib.cpp
    VM/src/lmem.cpp
    VM/src/lnumprint.cpp
    VM/src/lobject.cpp
    VM/src/loslib.cpp
    VM/src/lperf.cpp
    VM/src/lstate.cpp
    VM/src/lstring.cpp
    VM/src/lstrlib.cpp
    VM/src/ltable.cpp
    VM/src/ltablib.cpp
    VM/src/ltm.cpp
    VM/src/ludata.cpp
    VM/src/lutf8lib.cpp
    VM/src/lvmexecute.cpp
    VM/src/lvmload.cpp
    VM/src/lvmutils.cpp

    VM/src/lapi.h
    VM/src/lbuffer.h
    VM/src/lbuiltins.h
    VM/src/lbytecode.h
    VM/src/lcommon.h
    VM/src/ldebug.h
    VM/src/ldo.h
    VM/src/lfunc.h
    VM/src/lgc.h
    VM/src/lmem.h
    VM/src/lnumutils.h
    VM/src/lobject.h
    VM/src/lstate.h
    VM/src/lstring.h
    VM/src/ltable.h
    VM/src/ltm.h
    VM/src/ludata.h
    VM/src/lvm.h
)