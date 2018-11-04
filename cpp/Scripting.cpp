#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <map>

std::string exec(const char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
        if(fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}

//<<
//void parse (std::map<size_t, std::string> &map1, std::map<std::string, size_t> &map2,std::string line){
// size_t firstColonIndex = 0;
// size_t secondColonIndex = 0;
// size_t thirdColonIndex = 0;
// size_t colonCount = 0;
// size_t index = 0;
// while (index != line.length() - 1){
//     if (line[index] == ':'){
//         colonCount++;
//         if (colonCount == 1){
//             firstColonIndex = index;
//         }
//         else if (colonCount == 2){
//             secondColonIndex = index;
//         }
//         else if (colonCount == 3){
//             thirdColonIndex = index;
//         }
//     }
//     index++;
// }
////    std::cout << "first colon index = " << firstColonIndex << std::endl;
////    std::cout << "second colon index = " << secondColonIndex << std::endl;
////    std::cout << "third colon index = " << thirdColonIndex << std::endl;
//
//    //time to map things
//    int lineNum1 = stoi(line.substr(firstColonIndex + 1, (secondColonIndex - (firstColonIndex + 1))));
//    size_t lineNum2 = (size_t) lineNum1;
//    //std::cout << "line number: " << lineNum2 << std::endl;
//    std::string messageType = line.substr(secondColonIndex + 2, (thirdColonIndex - (secondColonIndex + 2)));
//    //std::cout << "message type: " << messageType << std::endl;
//    std::string message = line.substr(thirdColonIndex + 2, line.length() - 1);
//    //std::cout << "message: " << message << std::endl;
//
//}

int main(int argc, char *argv[]) {
    std::string linterCmd = "cppcheck --enable=all --template=\"{file}:{line}: {severity}: {message}\" "
            "--suppress=missingIncludeSystem ./cpp/";
    linterCmd.append(argv[1]);
    linterCmd.append(".cpp 2>&1");
    std::string linterRes = exec(linterCmd.c_str());
    std::istringstream f(linterRes);
    std::string line;
//    std::map<size_t, std::string> lineAndDescription;
//    std::map<std::string, size_t> typeAndCount;
//    size_t errorCount = 0;
    while (std::getline(f, line)) {
        std::cout << line << std::endl;
//        if (errorCount != 0){
//            parse(lineAndDescription, typeAndCount, line);
//        }
//        errorCount++;
    }

    return 0;
}
//int main() {
//    const std::string FILENAME = "main";
//    std::cout << "EXIT CODE: " << compileFile(FILENAME) << std::endl;
//
//    std::string outputCmd = "../compile.sh " + FILENAME + ".cpp";
//    std::string output = exec(outputCmd.c_str());
//    std::cout << "OUTPUT: " << output;
//
//    std::string linterCmd = "cppcheck --enable=all --template=\"{file}:{line}: {severity}: {message}\" --suppress=missingIncludeSystem ../main.cpp 2>&1";
//>>>>>>> 0e4c5858cd1d3eb08cad144557926454a7e0c5c6
//    std::string linterRes = exec(linterCmd.c_str());
//    std::istringstream f(linterRes);
//    std::string line;
//    std::map<size_t, std::string> lineAndDescription;
//    std::map<std::string, size_t> typeAndCount;
//    size_t errorCount = 0;
//    while (std::getline(f, line)) {
//        std::cout << line << std::endl;
//        if (errorCount != 0){
//            parse(lineAndDescription, typeAndCount, line);
//        }
//        errorCount++;
//    }
//
//    return 0;
//}
//
