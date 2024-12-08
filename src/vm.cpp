#include "../include/vm.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>

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



constexpr Instruction inst_push(Word operand) { return Instruction{.type = Inst_type::INST_PUSH, .operand = operand}; }
constexpr Instruction inst_dup(Word addr) { return Instruction{.type = Inst_type::INST_DUP, .operand = addr}; }
constexpr Instruction inst_plus(void) { return Instruction{.type = Inst_type::INST_PLUS}; }
constexpr Instruction inst_minus(void) { return Instruction{.type = Inst_type::INST_MINUS }; }
constexpr Instruction inst_mult(void) { return Instruction{.type = Inst_type::INST_MULT}; }
constexpr Instruction inst_div(void) { return Instruction{.type = Inst_type::INST_DIV}; }
constexpr Instruction inst_jmp(Word addr) { return Instruction{.type = Inst_type::INST_JMP, .operand = addr}; }
constexpr Instruction inst_halt(void) { return Instruction{.type = Inst_type::INST_HALT}; }

VM::VM() :m_halt(0), m_ip(0) {}

VM::VM(const std::vector<Instruction> &program) : m_program(program) {}

void VM::set_stack(const std::vector<Word> &stack) { m_stack = stack; }
const std::vector<Word> VM::get_stack() const { return m_stack; }

void VM::set_program(const std::vector<Instruction>& prog) { m_program = prog; }
const std::vector<Instruction> VM::get_program() const { return m_program; }

void VM::set_ip(const Word val) { m_ip = val; }
Word VM::get_ip() const { return m_ip; }

void VM::set_memory(const std::string &src) { m_memory = src; }
const std::string VM::get_memory() const { return m_memory; }

void VM::set_halt(int halt) { m_halt = halt; }
const int VM::get_halt() const { return m_halt; }

void VM::vm_push_inst(const Instruction &inst) {
    m_program.emplace_back(inst);
}

Trap VM::vm_execute_inst(const Instruction &inst) {
     if (m_ip < 0) {
        return Trap::TRAP_ILLEGAL_INST_ACCESS;
    }

    //Instruction instruction = vm.program.at(vm.ip);

    switch (inst.type) {
    case Inst_type::INST_NOP:
        m_ip += 1;
        break;
    case Inst_type::INST_PUSH:
        //vm.stack.at(vm.stack_size++) = inst.operand;
        m_stack.emplace_back(inst.operand);
        m_ip += 1;
        break;

    case Inst_type::INST_DUP:
        if (m_stack.size() - inst.operand <= 0) {
            return Trap::TRAP_STACK_UNDERFLOW;
        } else {
            if (inst.operand < 0) {
                return Trap::TRAP_ILLEGAL_INST;
            } else {
                m_stack.emplace_back(m_stack.at(m_stack.size() - 1 - inst.operand));
                m_ip += 1;
            }
        }
        break;
        
    case Inst_type::INST_PLUS:
        if (m_stack.size() < 2) {
            return Trap::TRAP_STACK_UNDERFLOW;
        } else {
            m_stack.at(m_stack.size() - 2) += m_stack.at(m_stack.size() - 1);
            m_stack.pop_back();
            m_ip += 1;
        }
        break;
        
    case Inst_type::INST_MINUS:
        if (m_stack.size() < 2) {
            return Trap::TRAP_STACK_UNDERFLOW;
        } else {
            m_stack.at(m_stack.size() - 2) -= m_stack.at(m_stack.size() - 1);
            m_stack.pop_back();
            m_ip += 1;
        }
        break;
        
    case Inst_type::INST_MULT:
        if (m_stack.size() < 2) {
            return Trap::TRAP_STACK_UNDERFLOW;
        } else {
            m_stack.at(m_stack.size() - 2) *= m_stack.at(m_stack.size() - 1);
            m_stack.pop_back();
            m_ip += 1;
        }
        break;
        
    case Inst_type::INST_DIV:
        if (m_stack.size() < 2) {
            return Trap::TRAP_STACK_UNDERFLOW;
        } else {
            if (m_stack.at(m_stack.size() - 1) == 0) {
                return Trap::TRAP_DIV_BY_ZERO;
            }
            m_stack.at(m_stack.size() - 2) /= m_stack.at(m_stack.size() - 1);
            m_stack.pop_back();
            m_ip += 1;
        }
        break;

    case Inst_type::INST_JMP:
        if (inst.operand >= 0 && inst.operand < m_program.size()) {
            m_ip = inst.operand;
        }
        break;
    case Inst_type::INST_EQ:
        if (m_stack.size() < 2) {
            return Trap::TRAP_STACK_UNDERFLOW;
        } else {
            m_stack.at(m_stack.size() - 2) = m_stack.at(m_stack.size() - 1) ==
                m_stack.at(m_stack.size() - 2);
            m_stack.pop_back();
            m_ip += 1;
        }
        break;

    case Inst_type::INST_JMP_IF:
        if (m_stack.size() < 1) {
            return Trap::TRAP_STACK_UNDERFLOW;
        } else {
            if (m_stack.at(m_stack.size() - 1)) {
                m_stack.pop_back();
                m_ip = inst.operand;
            } else {
                m_ip += 1;
            }
        }
        break;
    case Inst_type::INST_PRINT_DEBUG:
        if (m_stack.size() < 1) {
            return Trap::TRAP_STACK_UNDERFLOW;
        } else {
            std::cout << m_stack.at(m_stack.size() - 1) << '\n';
            m_stack.pop_back();
            m_ip += 1;
        }
        break;
    case Inst_type::INST_HALT:
        m_halt = 1;
        break;
    default:
        return Trap::TRAP_ILLEGAL_INST;
    }
    return Trap::TRAP_OK;
}

void VM::vm_load_program_from_memory(const std::vector<Instruction> &program) {
    std::copy(program.begin(), program.end(), std::back_inserter(m_program));
}

void VM::vm_load_program_from_file(const std::string &file_name) {
    std::ifstream file(file_name, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_name << '\n';
        return;
    }

    m_program.clear();

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
        m_program.emplace_back(Instruction{.type = inst_type, .operand = operand});

        // Debug loaded instruction
        //std::cout << "Loaded: " << inst_as_str(inst_type) << " " << operand << '\n';
    }

    file.close();
    std::cout << "Program successfully loaded from " << file_name << '\n';
}

void VM::vm_save_program_to_file(const std::string &file_path) {
    std::ofstream file(file_path, std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file to save program to: " << file_path << '\n';
        return;
    }

    if (m_program.empty()) {
        std::cerr << "Error: Program is empty, nothing to save.\n";
        file.close();
        return;
    }

    for (const auto &inst : m_program) {
        auto type_val = static_cast<std::underlying_type_t<Inst_type>>(inst.type);
        file.write(reinterpret_cast<const char *>(&type_val), sizeof(type_val));
        file.write(reinterpret_cast<const char *>(&inst.operand), sizeof(inst.operand));

    }
    
    file.close();
    std::cout << "Program successfully saved to " << file_path << '\n';
}



void VM::vm_translate_asm() {
    std::vector<std::string> lines {};
    
    for (size_t i = 0; i < m_memory.size(); ++i) {
        if (isspace(m_memory.at(i))) {
            while (i < m_memory.size() && isspace(m_memory.at(i))) {
                ++i;
            }
            --i;
        } else if (isalpha(m_memory.at(i))) {
            size_t start = i;
            while (i < m_memory.size() && isalpha(m_memory.at(i))) {
                ++i;
            }
            //std::cout << m_memory.substr(start, i - start) << '\n';
            lines.emplace_back(m_memory.substr(start, i - start));
            --i;
        } else if (isdigit(m_memory.at(i))) {
            size_t start = i;
            while (i < m_memory.size() && isdigit(m_memory.at(i))) {
                ++i;
            }
            // std::cout << m_memory.substr(start, i - start) << '\n';
            lines.emplace_back(m_memory.substr(start, i - start));         
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
            m_program.emplace_back(inst);
            ++i;
        } else if (lines.at(i) == "dup") {
            int val {};
            try {
                val = std::stoi(lines.at(i + 1));
            } catch (const std::invalid_argument &e) {
                std::cerr << "Failed to dup number to stack: " << e.what() << '\n';
            }
            inst = {.type = Inst_type::INST_DUP, .operand = val};
            m_program.emplace_back(inst);
            ++i;
        } else if (lines.at(i) == "plus") {
            inst = {.type = Inst_type::INST_PLUS};
            m_program.emplace_back(inst);
        } else if (lines[i] == "jmp") {
            if (i + 1 >= lines.size()) {
                std::cerr << "Error: 'jmp' missing operand.\n";
                exit(1);
            }
            int addr = std::stoi(lines[i + 1]);
            inst = inst_jmp(addr);
            m_program.emplace_back(inst);
            ++i;
        } else {
            std::cerr << "Unknown instruction: " << lines[i] << '\n';
            exit(1);
        }
    }

}

void VM::vm_dump_stack() {
    std::cout << "Stack:\n";
    if(m_stack.size() > 0) {
        for (size_t i = 0; i < m_stack.size(); ++i) {
            std::cout << "  " << m_stack.at(i) << '\n';
        }
    } else {
        std::cout << "[empty]\n";
    }
}