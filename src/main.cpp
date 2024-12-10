#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "../include/vm.hpp"

std::string slurp_file(const std::string &file_path) {
    std::fstream file(file_path, std::ios::in);

    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    if (file_size < 1) {
        std::cerr << "Failed to open instruction file.\n";
        exit(1);
    } else {
        std::string contents(file_size, '\0');
        file.read(&contents[0], file_size);
        file.close();
        return contents;
    }
}

static VM vm{};

int main(int argc, char *argv[]) {
    
    for (size_t i = 0; i < argc; ++i) {

        //  Read in human-readable assembly instructions
        if (strcmp(argv[i], "-c") == 0) {
            std::string src = slurp_file(argv[i + 1]);
            vm.set_memory(src);
            vm.vm_translate_asm();
            vm.set_ip(0);
        }
        // Load in precompiled instructions
        if (strcmp(argv[i], "-i") == 0) {
            vm.vm_load_program_from_file(argv[i+1]);
            vm.set_ip(0);
        }

        // Save instructions to a file
        if (strcmp(argv[i], "-o") == 0) {
            vm.vm_save_program_to_file(argv[i + 1]);
        }
    }
    
    for (size_t i = 0; i < 69 && !vm.get_halt(); ++i) {
        if (vm.get_program().empty()) {
            std::cerr << "Error: Program is empty.\n";
            return EXIT_FAILURE;
        }
        
        //if (vm.ip >= vm.program.size()) {
        //    std::cerr << "Error: Instruction pointer out of bounds (ip=" << vm.ip
        //              << ", size=" << vm.program.size() << ").\n";
        //    return EXIT_FAILURE;
        //}
        
        Instruction curr_inst = vm.get_program().at(vm.get_ip());
        Trap trap = vm.vm_execute_inst(curr_inst);
        if (trap != Trap::TRAP_OK) {
            std::cerr << "ERROR: " << trap_as_str(trap) << '\n';
            return EXIT_FAILURE;
        }
    }

    // vm_dump_stack(vm);
    vm.vm_dump_stack();
    return EXIT_SUCCESS;
}
