#include <cassert>
#include <charconv>
#include <string>
#include <iostream>

bool parse_int(const std::string& s, int& out) {
    int tmp; 
    auto last = s.data() + s.size();
    auto r = std::from_chars(s.data(), last, tmp);
    if((r.ec == std::errc{}) && (r.ptr == last)){
        out = tmp;
        return true;
    }
    return false;
}

int main() {
    int out;

    out = 7; 
    assert(parse_int("4096", out) && out == 4096);

    out = 7; 
    assert(parse_int("-15", out) && out == -15);

    out = 7; 
    assert(parse_int("0", out) && out == 0);

    out = 7; 
    assert(parse_int("007", out) && out == 7);

    out = 7; 
    assert(parse_int("-2147483648", out) && out == -2147483648);

    out = 7;
    assert(!parse_int("", out) && out == 7);

    out = 7;
    assert(!parse_int("12abc", out) && out == 7);   // the Q10 trap

    out = 7;
    assert(!parse_int(" 12", out) && out == 7);

    out = 7; 
    assert(!parse_int("12 ", out) && out == 7);

    out = 7; 
    assert(!parse_int("+12", out) && out == 7);

    out = 7; 
    assert(!parse_int("-", out) && out == 7);

    out = 7; 
    assert(!parse_int("2147483648", out) && out == 7);

    std::cout << "All tests passed\n";
}
