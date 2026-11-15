#include <cstdio>
#include <vector>
#include <cstdint>
#include <charconv>
#include <cstring>

bool parse_u64(const char* s, uint64_t &out){
    const char* last = s + std::strlen(s);
    auto [ptr, ec] = std::from_chars(s, last, out);
    return ec == std::errc{} && ptr == last;
}

bool write_record(const char* path, uint64_t N){
    FILE* fp = std::fopen(path, "wb");
    static char buf[1 << 20];
    std::setvbuf(fp, buf, _IOFBF, sizeof(buf)); 

    if (!fp){
        std::perror("Error opening file");
        return false;
    }

    for(uint64_t i = 0; i<N; i++){
        size_t status  = std::fwrite(&i, sizeof(uint64_t), 1, fp);
        if(status != 1){
            std::perror("Error while writing the data");
            std::fclose(fp);
            return false;
        }
    }

    if(std::fclose(fp)){
        std::perror("Error while closing file");
        return false;
    }
    return true;
} 

int main(int argc, char** argv){
    if (argc != 2){
        std::fprintf(stderr, "Usage: %s <N>\n", argv[0]);
        return 1;
    }
    uint64_t N = 0;
    if(!parse_u64(argv[1], N)){
        std::fprintf(stderr, "invalid N: '%s' (expected a non-negative integer)\n", argv[1]);
        return 1;
    }
    if(!write_record("data.bin", N)){
        return 1;
    };

    return 0;
}