#include <iostream>

int compileFile(std::string file) {
    std::string cmd = "./cpp/compile.sh " + file + ".cpp";
    const char *cstr = cmd.c_str();
    return system(cstr);
}

int main(int argc, char *argv[]) {
    std::cout << "EXIT CODE: " << compileFile(argv[1]) << std::endl;
    return 0;
}