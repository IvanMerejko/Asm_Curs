#include <iostream>
#include <vector>
#include "masm.h"


static std::map<std::string , std::vector<size_t>> map;
void push(const std::string& name , size_t line){
    map[name].push_back(line);
}
int main() {
    

    masm{"ts.asm"}.createListingFile();




    return 0;
}