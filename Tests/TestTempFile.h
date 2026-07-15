#pragma once

// RAII helper for DataSource tests: creates a temporary JSON file under the
// system temp directory so tests can exercise reload() against real files
// (missing file / malformed content / content changed between reloads)
// without touching storedData/*.json. Removes the file on destruction.

#include <filesystem>
#include <fstream>
#include <string>

class TestTempFile
{
public:
    TestTempFile(const std::string& fileName, const std::string& content)
        : path_(std::filesystem::temp_directory_path() / ("DataMonitorTests_" + fileName))
    {
        write(content);
    }

    ~TestTempFile()
    {
        std::error_code ignored;
        std::filesystem::remove(path_, ignored);
    }

    TestTempFile(const TestTempFile&) = delete;
    TestTempFile& operator=(const TestTempFile&) = delete;

    void write(const std::string& content) const
    {
        std::ofstream file(path_, std::ios::trunc);
        file << content;
    }

    const std::filesystem::path& path() const
    {
        return path_;
    }

private:
    std::filesystem::path path_;
};
