### Security != Undetected
In light of recent commits from Nano addressing security concerns, 
I would like to emphasize that this library will encompass source code and header files dedicated to 
fortifying our DLL against various forms of analysis.

Before implementing any security measures, it is imperative to thoroughly discuss them with 
the team and ensure they pass rigorous testing.


### TODO: Currently there are two types of str obfuscation, we should consider renaming or use only one of them
### xor.hpp seem good, remember to rename macros in caps


### TODO: UD spoof func
Currently Nano pushed to the main branch without consulting anyone also during the commit review I found 
unnecessire SPOOF_FUNC macros usage even in functions with no actual calls...
After a brief discussion with 7ap we decided to keep the SPOOF_FUNC macro but instead customize it to overwrite the return address 
to point to a win thread cleanup function. However stack frames will still be present but this is better than aving return addresses 
pointing to random memory locations.
(About stack frames we can consider encrypting the saved RBP on stack and decrypt it only on function end)