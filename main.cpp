#include <iostream>
#include <vector>
#include "masm.h"

int main() {

    masm{"ts.asm"}.createListingFile();
    return 0;
}