include ksamd64.inc

.code
    ALIGN 16
    srcCodecave  db 1000h dup(90h) ; nop padding

PUBLIC srcCodecave
END
