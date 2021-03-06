#include "sheap.hpp"

struct ABC {
    int a;
    float b;
    int c;

    ABC(int a1, float a2, int a3) : a(a1), b(a2), c(a3) {}
};

int main() {

    secure_pointer<ABC> abc = make_secure<ABC>(11, 12.1, 13);
    {
        unlocked_pointer<ABC> abcW = abc.unlock();
        abcW->a = 3;
    } // abcW is automatically locked on leaving scope


    // -> validates pointer before access
    abc->a;
}