#include <fcntl.h>
#include <cstdio>
#include <unistd.h>   
#include <filesystem>
#include <iostream>

constexpr const char* FD_DIR = "/proc/self/fd";  // Linux

bool write_run(const char* path, bool fail_early){
    int fp = ::open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fp == -1){
        std::perror("Error while opening the file");
        return false;
    }

    if (fail_early){
        return false;
    }

    ::close(fp);
    return true;
}


int open_fd_count(){
    int n = 0;
    for (const auto& entry : std::filesystem::directory_iterator(FD_DIR)) {
        (void)entry;   // we only count; silences -Wunused-variable
        ++n;
    }
    return n - 1;
}

int main(){
    for(int i = 0; i<10000 ; i++){
        write_run("leak_test.dat", true);
        //printing fd count and 
        if(i % 10 == 0){
            std::cout << "iteration: " << i << "  open fds: " << open_fd_count() << std::endl;
        }
    }
}
