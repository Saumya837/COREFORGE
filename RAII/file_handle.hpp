
#include <cstdio>
#include <cerrno>
#include <system_error>


class FileHandle {
    std::FILE* fp_ = nullptr;

public:
    explicit FileHandle(const char* path, const char* mode) 
    : fp_(std::fopen(path, mode)) {

        if(!fp_)
            throw std::system_error(errno, std::generic_category(), path);
    }

    ~FileHandle() { if (fp_) std::fclose(fp_); }

    FileHandle(const FileHandle&) = delete;               // no copy constructor
    FileHandle& operator = (const FileHandle&) = delete;  // no copy assignment

    std::FILE* get() const { return fp_; }
    bool is_open() const {return fp_ != nullptr; }

    int close() {
        if (!fp_) return 0;  
        int rc = std::fclose(fp_);
        fp_ = nullptr;
        return rc;
    }
};

