#include "sheap.h"

struct ABC {
    int a;
    float b;
    int c;
};

int main(int argc, char* argv[]) {

    const struct ABC* abc = shalloc(sizeof(struct ABC));

    // unlock before one more consecutive writes
    struct ABC* abc_w = sh_unlock(abc);
    abc_w->a = 1;
    abc_w->b = 2;
    abc_w->c = 3;
    // lock to remove write permissions
    sh_lock(abc_w);

    // validate before access
    sh_validate(abc);
    int x = abc->a;

    shfree(abc);
    return 0;
}
