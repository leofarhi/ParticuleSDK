#include <gint/dma.h>
#include <Particule/Core/System/gint.hpp>
#include <Particule/Core/System/File.hpp>



namespace Particule::Core
{
    #include <type_traits>

    template<typename Func>
    bool File::OpenWith(const std::string& path, FileMode mode, Endian endian, Func&& func) {
        gint_wswitch_enter();
        FILE* fd_file = File::open_file(path, mode);
        bool is_open = fd_file != nullptr;
        if (is_open)
        {
            File file(fd_file, mode, endian);
            func(&file);
            fclose(fd_file);
            file.file = nullptr; // Prevent destructor from closing the file again
        }
        gint_wswitch_exit();
        return is_open;
    }

    template<typename T>
    void File::Write(const T& data) {
        static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");
        if (endian == Endian::BigEndian)
            WriteBigEndian(&data, sizeof(T));
        else
            WriteLittleEndian(&data, sizeof(T));
    }

    template<typename T>
    void File::Write(const T* data, size_t size) {
        static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");
        for (size_t i = 0; i < size; ++i)
            Write(data[i]);
    }

    template<typename T>
    void File::Read(T& data) {
        static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");
        if (endian == Endian::BigEndian)
            ReadBigEndian(&data, sizeof(T));
        else
            ReadLittleEndian(&data, sizeof(T));
    }

    template<typename T>
    void File::Read(T* data, size_t size) {
        static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");
        for (size_t i = 0; i < size; ++i)
            Read(data[i]);
    }

}