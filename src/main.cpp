#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

enum class Trap {
    TRAP_OK = 0,
    TRAP_STACK_OVERFLOW,
    TRAP_STACK_UNDERFLOW,
    TRAP_ILLEGAL_INST,
    TRAP_DIV_BY_ZERO,
    TRAP_ILLEGAL_INST_ACCESS,
};

const std::string trap_as_str(Trap trap) {
    switch (trap) {
    case Trap::TRAP_OK:
      return "TRAP_OK";
    case Trap::TRAP_STACK_OVERFLOW:
      return "TRAP_STACK_OVERFLOW";
    case Trap::TRAP_STACK_UNDERFLOW:
      return "TRAP_STACK_UNDERFLOW";
    case Trap::TRAP_ILLEGAL_INST:
        return "TRAP_ILLEGAL_INST";
    case Trap::TRAP_DIV_BY_ZERO:
        return "TRAP_DIV_BY_ZERO";
    case Trap::TRAP_ILLEGAL_INST_ACCESS:
        return "TRAP_ILLEGAL_INST ACCESS";
    default:
        assert(0 && "trap_as_str() Unreachable");
    }
}

using Word = int64_t;

enum class Inst_type {
    INST_NOP,
    INST_PUSH,
    INST_DUP,
    INST_PLUS,
    INST_MINUS,
    INST_MULT,
    INST_DIV,
    INST_JMP,
    INST_JMP_IF,
    INST_EQ,
    INST_HALT,
    INST_PRINT_DEBUG,
};

const std::string inst_as_str(const Inst_type &type) {
    switch (type) {
    case Inst_type::INST_NOP:
        return "INST_NOP";
    case Inst_type::INST_PUSH:
        return "INST_PUSH";
    case Inst_type::INST_DUP:
        return "INST_DUP";
    case Inst_type::INST_PLUS:
        return "INST_PLUS";
    case Inst_type::INST_MINUS:
        return "INST_TYPE_MINUS";
    case Inst_type::INST_MULT:
        return "INST_TYPE_MULT";
    case Inst_type::INST_DIV:
        return "INST_TYPE_DIV";
    case Inst_type::INST_JMP:
        return "INST_TYPE_JMP";
    case Inst_type::INST_JMP_IF:
      return "INST_JMP_IF";
    case Inst_type::INST_EQ:
        return "INST_EQUAL";
    case Inst_type::INST_HALT:
        return "INST_TYPE_HALT";
    case Inst_type::INST_PRINT_DEBUG:
        return "INST_TYPE_PRINT_DEBUG";
    default:
        assert(0 && "inst_as_str() Unreachable");
    }
}

struct Instruction {
    Inst_type type;
    Word operand;
};

struct VM {
    std::vector<Word> stack{};
    size_t stack_size{}; // May not need this since `std::vector` contains its
                         // own size methods
    std::vector<Instruction> program {};
    Word ip {}; // Instruction pointer

    std::string memory {};
    int halt;
};

constexpr Instruction inst_push(Word operand) { return Instruction{.type = Inst_type::INST_PUSH, .operand = operand}; }
constexpr Instruction inst_dup(Word addr) { return Instruction{.type = Inst_type::INST_DUP, .operand = addr}; }
constexpr Instruction inst_plus(void) { return Instruction{.type = Inst_type::INST_PLUS}; }
constexpr Instruction inst_minus(void) { return Instruction{.type = Inst_type::INST_MINUS }; }
constexpr Instruction inst_mult(void) { return Instruction{.type = Inst_type::INST_MULT}; }
constexpr Instruction inst_div(void) { return Instruction{.type = Inst_type::INST_DIV}; }
constexpr Instruction inst_jmp(Word addr) { return Instruction{.type = Inst_type::INST_JMP, .operand = addr}; }
constexpr Instruction inst_halt(void) { return Instruction{.type = Inst_type::INST_HALT}; }

static VM vm{};

void vm_push_inst(VM &vm, const Instruction &inst) {
    vm.program.emplace_back(inst);
}

Trap vm_execute_inst(VM &vm, const Instruction &inst) {
    if (vm.ip < 0) {
        return Trap::TRAP_ILLEGAL_INST_ACCESS;
    }

    //Instruction instruction = vm.program.at(vm.ip);

    switch (inst.type) {
    case Inst_type::INST_NOP:
        vm.ip += 1;
        break;
    case Inst_type::INST_PUSH:
        //vm.stack.at(vm.stack_size++) = inst.operand;
        vm.stack.emplace_back(inst.operand);
        vm.ip += 1;
        break;

    case Inst_type::INST_DUP:
        if (vm.stack.size() - inst.operand <= 0) {
            return Trap::TRAP_STACK_UNDERFLOW;
        } else {
            if (inst.operand < 0) {
                return Trap::TRAP_ILLEGAL_INST;
            } else {
                vm.stack.emplace_back(vm.stack.at(vm.stack.size() - 1 - inst.operand));
                vm.ip += 1;
            }
        }
        break;
        
    case Inst_type::INST_PLUS:
        if (vm.stack.size() < 2) {
            return Trap::TRAP_STACK_UNDERFLOW;
        } else {
            vm.stack.at(vm.stack.size() - 2) += vm.stack.at(vm.stack.size() - 1);
            vm.stack.pop_back();
            vm.ip += 1;
        }
        break;
        
    case Inst_type::INST_MINUS:
        if (vm.stack.size() < 2) {
            return Trap::TRAP_STACK_UNDERFLOW;
        } else {
            vm.stack.at(vm.stack.size() - 2) -= vm.stack.at(vm.stack.size() - 1);
            vm.stack.pop_back();
            vm.ip += 1;
        }
        break;
        
    case Inst_type::INST_MULT:
        if (vm.stack.size() < 2) {
            return Trap::TRAP_STACK_UNDERFLOW;
        } else {
            vm.stack.at(vm.stack.size() - 2) *= vm.stack.at(vm.stack.size() - 1);
            vm.stack.pop_back();
            vm.ip += 1;
        }
        break;
        
    case Inst_type::INST_DIV:
        if (vm.stack.size() < 2) {
            return Trap::TRAP_STACK_UNDERFLOW;
        } else {
            if (vm.stack.at(vm.stack.size() - 1) == 0) {
                return Trap::TRAP_DIV_BY_ZERO;
            }
            vm.stack.at(vm.stack.size() - 2) /= vm.stack.at(vm.stack.size() - 1);
            vm.stack.pop_back();
            vm.ip += 1;
        }
        break;

    case Inst_type::INST_JMP:
        if (inst.operand >= 0 && inst.operand < vm.program.size()) {
            vm.ip = inst.operand;
        }
        break;
    case Inst_type::INST_EQ:
        if (vm.stack.size() < 2) {
            return Trap::TRAP_STACK_UNDERFLOW;
        } else {
            vm.stack.at(vm.stack.size() - 2) = vm.stack.at(vm.stack.size() - 1) ==
                vm.stack.at(vm.stack.size() - 2);
            vm.stack.pop_back();
            vm.ip += 1;
        }
        break;

    case Inst_type::INST_JMP_IF:
        if (vm.stack.size() < 1) {
            return Trap::TRAP_STACK_UNDERFLOW;
        } else {
            if (vm.stack.at(vm.stack.size() - 1)) {
                vm.stack.pop_back();
                vm.ip = inst.operand;
            } else {
                vm.ip += 1;
            }
        }
        break;
    case Inst_type::INST_PRINT_DEBUG:
        if (vm.stack.size() < 1) {
            return Trap::TRAP_STACK_UNDERFLOW;
        } else {
            std::cout << vm.stack.at(vm.stack.size() - 1) << '\n';
            vm.stack.pop_back();
            vm.ip += 1;
        }
        break;
    case Inst_type::INST_HALT:
        vm.halt = 1;
        break;
    default:
        return Trap::TRAP_ILLEGAL_INST;
    }
    return Trap::TRAP_OK;
}

void vm_load_program_from_memory(VM &vm, const std::vector<Instruction> &program) {
    std::copy(program.begin(), program.end(), std::back_inserter(vm.program));
}

void vm_load_program_from_file(VM &vm, const std::string &file_name) {
    std::ifstream file(file_name, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_name << '\n';
        return;
    }

    vm.program.clear();

    while (true) {
        std::underlying_type_t<Inst_type> type_val;
        Word operand;

        file.read(reinterpret_cast<char *>(&type_val), sizeof(type_val));
        if (file.eof()) break;

        if (!file) {
            std::cerr << "Error reading instruction type from file.\n";
            break;
        }

        file.read(reinterpret_cast<char *>(&operand), sizeof(operand));
        if (file.eof()) break;

        if (!file) {
            std::cerr << "Error reading instruction operand from file.\n";
            break;
        }

        Inst_type inst_type = static_cast<Inst_type>(type_val);
        vm.program.emplace_back(Instruction{.type = inst_type, .operand = operand});

        // Debug loaded instruction
        //std::cout << "Loaded: " << inst_as_str(inst_type) << " " << operand << '\n';
    }

    file.close();
    std::cout << "Program successfully loaded from " << file_name << '\n';
}

void vm_save_program_to_file(const std::vector<Instruction> &program, const std::string &file_path) {
    std::ofstream file(file_path, std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file to save program to: " << file_path << '\n';
        return;
    }

    if (program.empty()) {
        std::cerr << "Error: Program is empty, nothing to save.\n";
        file.close();
        return;
    }

    for (const auto &inst : program) {
        auto type_val = static_cast<std::underlying_type_t<Inst_type>>(inst.type);
        file.write(reinterpret_cast<const char *>(&type_val), sizeof(type_val));
        file.write(reinterpret_cast<const char *>(&inst.operand), sizeof(inst.operand));

    }

    file.close();
    std::cout << "Program successfully saved to " << file_path << '\n';
}

void vm_translate_asm(const std::string &source,
                        std::vector<Instruction> &program) {
    std::vector<std::string> lines {};
    
    for (size_t i = 0; i < source.size(); ++i) {
        if (isspace(source.at(i))) {
            while (i < source.size() && isspace(source.at(i))) {
                ++i;
            }
            --i;
        } else if (isalpha(source.at(i))) {
            size_t start = i;
            while (i < source.size() && isalpha(source.at(i))) {
                ++i;
            }
            //std::cout << source.substr(start, i - start) << '\n';
            lines.emplace_back(source.substr(start, i - start));
            --i;
        } else if (isdigit(source.at(i))) {
            size_t start = i;
            while (i < source.size() && isdigit(source.at(i))) {
                ++i;
            }
            // std::cout << source.substr(start, i - start) << '\n';
            lines.emplace_back(source.substr(start, i - start));         
            --i;
        }
    }

    Instruction inst;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (lines.at(i) == "push") {
            int val {};
            try {
                val = std::stoi(lines.at(i + 1));
            } catch (const std::invalid_argument &e) {
                std::cerr << "Failed to push number to stack: " << e.what() << '\n';
            }
            inst = {.type = Inst_type::INST_PUSH, .operand = val};
            program.emplace_back(inst);
            ++i;
        } else if (lines.at(i) == "dup") {
            int val {};
            try {
                val = std::stoi(lines.at(i + 1));
            } catch (const std::invalid_argument &e) {
                std::cerr << "Failed to dup number to stack: " << e.what() << '\n';
            }
            inst = {.type = Inst_type::INST_DUP, .operand = val};
            program.emplace_back(inst);
            ++i;
        } else if (lines.at(i) == "plus") {
            inst = {.type = Inst_type::INST_PLUS};
            program.emplace_back(inst);
        } else if (lines[i] == "jmp") {
            if (i + 1 >= lines.size()) {
                std::cerr << "Error: 'jmp' missing operand.\n";
                exit(1);
            }
            int addr = std::stoi(lines[i + 1]);
            inst = inst_jmp(addr);
            program.emplace_back(inst);
            ++i;
        } else {
            std::cerr << "Unknown instruction: " << lines[i] << '\n';
            exit(1);
        }
    }
}

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

void vm_dump_stack(const VM &vm) {
    
    std::cout << "Stack:\n";
    if(vm.stack.size() > 0) {
        for (size_t i = 0; i < vm.stack.size(); ++i) {
            std::cout << "  " << vm.stack.at(i) << '\n';
        }
    } else {
        std::cout << "[empty]\n";
    }
}

int main(int argc, char *argv[]) {
    for (size_t i = 0; i < argc; ++i) {

        //  Read in human-readable assembly instructions
        if (strcmp(argv[i], "-c") == 0) {
            vm.memory = slurp_file(argv[i + 1]);
            vm_translate_asm(vm.memory, vm.program);
            vm.ip = 0;
        }
        // Load in precompiled instructions
        if (strcmp(argv[i], "-i") == 0) {
            vm_load_program_from_file(vm, argv[i + 1]);
            vm.ip = 0;
        }

        // Save instructions to a file
        if (strcmp(argv[i], "-o") == 0) {
            vm_save_program_to_file(vm.program, argv[i + 1]);
        }
    }
    
    for (size_t i = 0; i < 69 && !vm.halt; ++i) {
        if (vm.program.empty()) {
            std::cerr << "Error: Program is empty.\n";
            return EXIT_FAILURE;
        }
        
        if (vm.ip >= vm.program.size()) {
            std::cerr << "Error: Instruction pointer out of bounds (ip=" << vm.ip
                      << ", size=" << vm.program.size() << ").\n";
            return EXIT_FAILURE;
        }
        
        Instruction curr_inst = vm.program.at(vm.ip);
        Trap trap = vm_execute_inst(vm, curr_inst);
        if (trap != Trap::TRAP_OK) {
            std::cerr << "ERROR: " << trap_as_str(trap) << '\n';
            return EXIT_FAILURE;
        }
    }
    
    vm_dump_stack(vm);
    return EXIT_SUCCESS;
}
