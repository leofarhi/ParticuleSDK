#ifndef FILE_HPP
#define FILE_HPP

#include <cstdio>
#include <string>
#include <stdexcept>
#include <vector>
#include <functional>
#include <type_traits>
#include <cstring>

namespace Particule::Core
{
    enum class FileMode {
        Read,
        Write,
        Append
    };
    
    enum class Endian {
        BigEndian,
        LittleEndian
    };
    
    class File {
    private:
        FILE* file;
        FileMode mode;
        Endian endian;
    
        void ReadBigEndian(void* data, size_t size);
        void WriteBigEndian(const void* data, size_t size);
        void ReadLittleEndian(void* data, size_t size);
        void WriteLittleEndian(const void* data, size_t size);
    
        File(const std::string& path, FileMode mode, Endian endian = Endian::LittleEndian);
        File(const File& other) = delete;
        File& operator=(const File& other) = delete;
        File(File&& other) noexcept;
    public:
        ~File();
    
        template<typename Func>
        static bool OpenWith(const std::string& path, FileMode mode, Endian endian, Func&& func);
        //Not recommended to use this function, prefer OpenWith
        static File* Open(const std::string& path, FileMode mode, Endian endian = Endian::LittleEndian);
        static void Close(File* file);
    
        template<typename T>
        void Write(const T& data);
        template<typename T>
        void Write(const T* data, size_t size);
        template<typename T>
        void Read(T& data);
        template<typename T>
        void Read(T* data, size_t size);
    
        std::string ReadLine();
        std::string ReadAll();
    };
}

#endif // FILE_HPP