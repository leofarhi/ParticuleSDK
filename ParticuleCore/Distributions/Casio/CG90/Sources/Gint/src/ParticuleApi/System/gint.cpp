#include <Particule/Core/System/gint.hpp>

namespace gint
{
    extern "C"
    {
        char *getenv(const char *name) {
            (void)name;
            return nullptr; // Stub basique
        }

        void gint_load_onchip_sections(void);
        void gint_panic(uint32_t code);
        int gint_get_onchip_save_mode(void **ptr);

        static int _gint_wswitch = 0; // Variable statique pour indiquer si nous sommes dans un switch

        void gint_wswitch_enter(void)
        {
            if (_gint_wswitch++) return;
            extern void *gint_stack_top;
            extern gint_world_t gint_world_addin, gint_world_os;
            void *onchip_save_buffer;
            int onchip_save_mode = gint_get_onchip_save_mode(&onchip_save_buffer);
            gint_world_switch_out(gint_world_addin, gint_world_os);
            void *ILRAM = (void *)0xe5200000;
            void *XRAM  = (void *)0xe500e000;
            void *YRAM  = (void *)0xe5010000;

            uint32_t *canary = (uint32_t *)gint_stack_top;
            if(canary) *canary = 0xb7c0ffee;

            if(!isSH3() && onchip_save_mode == GINT_ONCHIP_BACKUP) {
                void *ptr = onchip_save_buffer;
                memcpy(ptr, ILRAM, 4096);
                ptr = (uint8_t *)ptr + 4096;
                memcpy(ptr, XRAM, 8192);
                ptr = (uint8_t *)ptr + 8192;
                memcpy(ptr, YRAM, 8192);
            }
        }

        void gint_wswitch_exit(void)
        {
            if (--_gint_wswitch) return; // Si nous ne sommes pas à 0, on sort
            if (_gint_wswitch < 0)
                gint_panic(0x1080); // Si on est négatif, c'est qu'il y a un problème
            _gint_wswitch = 0;
            extern void *gint_stack_top;
            extern gint_world_t gint_world_addin, gint_world_os;
            void *onchip_save_buffer;
            int onchip_save_mode = gint_get_onchip_save_mode(&onchip_save_buffer);
            void *ILRAM = (void *)0xe5200000;
            void *XRAM  = (void *)0xe500e000;
            void *YRAM  = (void *)0xe5010000;

            uint32_t *canary = (uint32_t *)gint_stack_top;

            if(!isSH3() && onchip_save_mode == GINT_ONCHIP_BACKUP) {
                void *ptr = onchip_save_buffer;
                memcpy(ILRAM, ptr, 4096);
                ptr = (uint8_t *)ptr + 4096;
                memcpy(XRAM, ptr, 8192);
                ptr = (uint8_t *)ptr + 8192;
                memcpy(YRAM, ptr, 8192);
            } else if(!isSH3() && onchip_save_mode == GINT_ONCHIP_REINITIALIZE) {
                gint_load_onchip_sections();
            }

            if(canary && *canary != 0xb7c0ffee)
                gint_panic(0x1080);

            gint_world_switch_in(gint_world_os, gint_world_addin);
        }

    }
}
