/**
* Author: lebaoworks@gmail.com
* Date: 2022/02/15
* 
* Simple zip library for 32-bit zip archive.
* 
* General structure:
*     ________________
*    |     Entry 1    |
*    |----------------|
*    |     Entry 2    |
*    |----------------|
*    |       ...      |
*    |----------------|
*    |     Entry N    |
*    |----------------|
*    | Decrypt Header |
*    |----------------|
*    |   Extra Data   |
*    |----------------|
*    |   Central Dir  |
*    |________________|
*/
#pragma once

#include <stdint.h>
#include <string>
#include <sstream>
#include <vector>

enum class ZipCompressionMethod {
    NoCompression = 0,
    Shrunk = 1,
    Factor1 = 2,
    Factor2 = 3,
    Factor3 = 4,
    Factor4 = 5,
    Imploded = 6,
    Reserved1 = 7,
    Deflated = 8,
    EnhancedDeflated = 9,
    PKWareDCLImploded = 10,
    Reserved2 = 11,
    BZIP2 = 12,
    Reserved3 = 13,
    LZMA = 14,
    Reserved4 = 15,
    Reserved5 = 16,
    Reserved6 = 17,
    IBMTERSE = 18,
    IBMLZ77 = 19,
    PPMdVer1 = 98
};

class ZipEntryHeader
{
    friend class ZipEntry;
    friend class Zip;
    uint32_t signature{0x04034B50};
    uint16_t version{};
    uint16_t flags{};
    uint16_t compression{};
    uint16_t mod_time{};
    uint16_t mod_date{};
    uint32_t crc32{};
    uint32_t compressed_size{};
    uint32_t uncompressed_size{};
    uint16_t name_len{};
    uint16_t extra_len{};
    std::string name;
    std::string extra;

    bool from_bytes(const char* data, size_t data_len);

public:
    size_t size();
};

class ZipEntry
{
    friend class Zip;
    ZipEntryHeader header;
    std::stringstream data;
    struct {
        uint32_t crc32{};
        uint32_t compressed_size{};
        uint32_t uncompressed_size{};
    } data_descriptor;

public:
    bool from_file(std::wstring external_path, std::wstring internal_path, ZipCompressionMethod compress_method);
    bool from_bytes(const char* data, size_t data_len);
    bool to_bytes(std::stringstream& out);
    size_t size();
private:
    bool use_data_descriptor();
};

class ZipCentralEntryHeader
{
    friend class Zip;
    uint32_t signature{};
    uint16_t version{};
    uint16_t version_needed{};
    uint16_t flags{};
    uint16_t compression{};
    uint16_t mod_time{};
    uint16_t mod_date{};
    uint32_t crc32{};
    uint32_t compressed_size{};
    uint32_t uncompressed_size{};
    uint16_t name_len{};
    uint16_t extra_len{};
    uint16_t comment_len{};
    uint16_t disk_num_start{};
    uint16_t internal_attr{};
    uint32_t external_attr{};
    uint32_t local_header_offset{};
    std::string name;
    std::string extra;
    std::string comment;

    bool to_bytes(std::stringstream& out); 
};

class ZipCentralEnd
{
    friend class Zip;
    uint32_t signature{};
    uint16_t disk_num{};
    uint16_t disk_num_start{};
    uint16_t disk_entries{};
    uint16_t total_entries{};
    uint32_t directory_size{};
    uint32_t directory_offset{};
    uint16_t comment_len{};
    std::string comment;

    bool to_bytes(std::stringstream& out);
};

class Zip
{
    std::vector<ZipEntry*> entries;
public:
    Zip();
    ~Zip();
    bool add(std::wstring external_path, std::wstring internal_path, ZipCompressionMethod compress_method);
    bool to_bytes(std::stringstream& out);
};


