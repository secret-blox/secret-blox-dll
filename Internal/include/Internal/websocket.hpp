namespace SB::Websocket
{
    enum OpCode
    {
        WS_NOP = 77,
        WS_EXEC,
        WS_SETFPS,
        WS_MSG
    };

    extern bool listening;

    void socketLoop();

    void setup();
    void unload();
}