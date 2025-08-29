#include <Particule/Core/System/File.hpp>
#include <sstream>
#include <algorithm>

namespace Particule::Core
{
    FILE* File::open_file(const std::string& path, FileMode mode)
    {
        const char* modeStr = nullptr;
        switch (mode) {
            case FileMode::Read: modeStr = "rb"; break;
            case FileMode::Write: modeStr = "wb"; break;
            case FileMode::Append: modeStr = "ab"; break;
        }
        FILE* file = fopen(path.c_str(), modeStr);
        return file;
    }

    File::File(FILE* file, FileMode mode, Endian endian) : file(file), mode(mode), endian(endian) {}

    File::File(File&& other) noexcept
        : file(other.file), mode(other.mode), endian(other.endian) {
        other.file = nullptr; // Prevent double close
    }

    File::~File() {
        if (file)
            fclose(file);
    }

    File* File::Open(const std::string& path, FileMode mode, Endian endian) {
        gint_wswitch_enter();
        FILE* file = File::open_file(path, mode);
        if (!file)
        {
            gint_wswitch_exit();
            return nullptr;
        }
        return new File(file, mode, endian);
    }

    void File::Close(File* file) {
        delete file;
        gint_wswitch_exit();
    }

    void File::ReadBigEndian(void* data, size_t size) {
        fread(data, 1, size, file);
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        std::reverse(reinterpret_cast<char*>(data), reinterpret_cast<char*>(data) + size);
    #endif
    }

    void File::WriteBigEndian(const void* data, size_t size) {
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        std::vector<char> buffer(reinterpret_cast<const char*>(data), reinterpret_cast<const char*>(data) + size);
        std::reverse(buffer.begin(), buffer.end());
        fwrite(buffer.data(), 1, size, file);
    #else
        fwrite(data, 1, size, file);
    #endif
    }

    void File::ReadLittleEndian(void* data, size_t size) {
        fread(data, 1, size, file);
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        std::reverse(reinterpret_cast<char*>(data), reinterpret_cast<char*>(data) + size);
    #endif
    }

    void File::WriteLittleEndian(const void* data, size_t size) {
    #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        std::vector<char> buffer(reinterpret_cast<const char*>(data), reinterpret_cast<const char*>(data) + size);
        std::reverse(buffer.begin(), buffer.end());
        fwrite(buffer.data(), 1, size, file);
    #else
        fwrite(data, 1, size, file);
    #endif
    }

    std::string File::ReadLine() {
        std::string line;
        char ch;
        while (fread(&ch, 1, 1, file) == 1 && ch != '\n') {
            if (ch != '\r') line += ch;
        }
        return line;
    }

    std::string File::ReadAll() {
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fseek(file, 0, SEEK_SET);
        std::string content(size, '\0');
        fread(&content[0], 1, size, file);
        return content;
    }

}