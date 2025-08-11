#ifndef GINT_HPP
#define GINT_HPP
#include <cstring>
#include <cstdlib>
#include <libprof.h>
namespace gint
{
    extern "C"
    {
        #include <gint/gint.h>
        #include <gint/defs/call.h>
        #include <gint/dma.h>
        #include <gint/drivers.h>
        #include <gint/defs/call.h>
        #include <gint/display.h>
        #include <gint/keyboard.h>
        #include <gint/rtc.h>
        #include <gint/timer.h>
        #include <gint/clock.h>
        #include <gint/kmalloc.h>
        
        char *getenv(const char *name);
        void gint_wswitch_enter(void);
        void gint_wswitch_exit(void);
    }
}
#endif // GINT_HPP