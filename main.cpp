#include <iostream>
#include <cstring>
#include <sys/mman.h>


typedef unsigned char (* functionType)(unsigned char);

functionType function;

void* memory;

unsigned int POSITION = 12;


/**
 * Code for function:
 *
 *   unsigned char multBy5(unsigned char a) {
 *       unsigned char five = 5;
 *       return a * five;
 *   }
 *
 */
unsigned char code[] = {0x55,
                        0x48, 0x89, 0xe5,
                        0x89, 0xf8,
                        0x88, 0x45, 0xec,
                        0xc6, 0x45, 0xff,  /* five is here. index 12 */ 0x05,
                        0x0f, 0xb6, 0x45, 0xec,
                        0xf6, 0x65, 0xff,
                        0x5d,
                        0xc3};

bool allocate() {
    memory = mmap(nullptr, sizeof(code), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (function == MAP_FAILED) {
        perror("memory mapping@failed");
        return false;
    }
    return true;
}

bool toggleMemory(int type) {
    if (mprotect(memory, sizeof(code), PROT_READ | type) == -1) {
        perror("toggle protect@failed");
        return false;
    }
    return true;
}

bool makeWrite() {
    return toggleMemory(PROT_WRITE);
}

bool makeExec() {
    return toggleMemory(PROT_EXEC);
}


bool inject() {
    // map
    if (!makeWrite()) {
        return false;
    }
    // copy
    memcpy(memory, code, sizeof(code));
    //change exec
    return makeExec();
}


bool modify(unsigned char five) {
    if (!makeWrite()) {
        return false;
    }
    ((unsigned char*) memory)[POSITION] = five;
    return true;
}


void clear() {
    if (munmap(memory, sizeof(code)) == -1) {
        perror("clear@fail");
    }
}

void help() {
    std::cout << "it's simple program which shows injecting byte code runtime\n";
    std::cout << "For example, F: unsigned char -> unsigned char, F(x) = 5 * x\n";
    std::cout << "###\n\n";
    std::cout
            << "Press:\n\tquit - for stop program\n\texec - for execute program\n\tmod - modify 'five' in function\n\n";

}

int main() {

    help();
    std::cout << std::endl;
    std::cout << "Setting up..." << std::endl;
    allocate();
    inject();
    std::cout << "Injected" << std::endl;
    std::string arg;

    std::cout << "jit@example: ";
    while (true) {
        if (!std::getline(std::cin, arg)) {
            break;
        }

        if (arg == "\n" || arg.empty()) {
            continue;
        }

        if (arg == "quit") {
            break;
        }
        if (arg == "exec") {
            if (makeExec()) {
                int x;
                std::cout << "input 'x': ";
                std::cin >> x;
                std::cout << "Result: " << static_cast<int>(((functionType) memory)(x)) << std::endl;
            }
        }
        if (arg == "mod") {
            if (makeWrite()) {
                int x, five;
                std::cout << "input 'five': ";
                std::cin >> five;
                if (modify(five) && makeExec()) {
                    std::cout << "input 'x': ";
                    std::cin >> x;
                    std::cout << "Result: " << static_cast<int>(((functionType) memory)(x)) << std::endl;
                }
            }
        }
        std::cout << "jit@example: ";
    }


    clear();
    return 0;
}
