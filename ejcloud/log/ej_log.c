#include "ej_log.h"
int _EJ_DbgHeader(EJ_DebugLevel level, const char* file, int line)
{
    uint32_t logTimeSecond;
    uint32_t logTimeMS;
    if (level <= EJ_DbgLevel) {
        logTimeSecond = 0;
		logTimeMS = 0;
        if (file) {
            const char* fn = file;
            while (*fn) {
                if ((*fn == '/') || (*fn == '\\')) {
                    file = fn + 1;
                }
                ++fn;
            }
           
        } 
        return true;
    } else {
        return false;
    }
}


EJ_DebugLevel EJ_DbgLevel = EJ_DEBUG_ALL;
uint8_t dbgALL = 0;

