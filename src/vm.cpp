#include "../include/vm.hpp"
#include <cstddef>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>
#include <limits>

const std::string trap_as_str(Trap trap) noexcept {
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

const std::string inst_as_str(const Inst_type &type) noexcept {
    switch (type) {
        case Inst_type::INST_NOP:
            return "INST_NOP";
        case Inst_type::INST_PUSH:
            return "INST_PUSH";
        case Inst_type::INST_DUP:
            return "INST_DUP";
        case Inst_type::INST_SWAP:
            return "INST_SWAP";
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
        case Inst_type::INST_NOT:
            return "INST_NOT";
        case Inst_type::INST_RET:
            return "INST_RET";
        case Inst_type::INST_CALL:
            return "INST_CALL";
        case Inst_type::INST_OR:
            return "INST_OR";
        case Inst_type::INST_AND:
            return "INST_AND";
        case Inst_type::INST_XOR:
            return "INST_XOR";
        case Inst_type::INST_SHL:
            return "INST_SHL";
        case Inst_type::INST_SHR:
            return "INST_SHR";
        case Inst_type::INST_PRINT_DEBUG:
            return "INST_TYPE_PRINT_DEBUG";
        default:
            assert(0 && "inst_as_str() Unreachable");
            return "Unreachable";
    }
}

Instruction inst_nop() noexcept { return Instruction{.type = Inst_type::INST_NOP}; }
Instruction inst_push(i64 operand) noexcept { return Instruction{.type = Inst_type::INST_PUSH, .operand = operand}; }
Instruction inst_push(f64 operand) noexcept { return Instruction{.type = Inst_type::INST_PUSH, .operand = operand}; }
Instruction inst_swap(i64 operand) noexcept { return Instruction{.type = Inst_type::INST_SWAP, .operand = operand}; }
Instruction inst_dup(i64 addr) noexcept { return Instruction{.type = Inst_type::INST_DUP, .operand = addr}; }
Instruction inst_drop() noexcept { return Instruction{.type = Inst_type::INST_DROP}; }
Instruction inst_plus() noexcept { return Instruction{.type = Inst_type::INST_PLUS}; }
Instruction inst_minus() noexcept { return Instruction{.type = Inst_type::INST_MINUS }; }
Instruction inst_mult() noexcept { return Instruction{.type = Inst_type::INST_MULT}; }
Instruction inst_div() noexcept { return Instruction{.type = Inst_type::INST_DIV}; }
Instruction inst_jmp(i64 addr) noexcept { return Instruction{.type = Inst_type::INST_JMP, .operand = addr}; }
Instruction inst_jmp(const std::string &label) noexcept { return Instruction{.type = Inst_type::INST_JMP, .operand = label}; }
Instruction inst_jmp_if(const std::string &operand) noexcept { return Instruction{.type = Inst_type::INST_JMP_IF, .operand = operand}; }
Instruction inst_halt() noexcept { return Instruction{.type = Inst_type::INST_HALT}; }
Instruction inst_not() noexcept { return Instruction{.type = Inst_type::INST_NOT}; }
Instruction inst_ret() noexcept { return Instruction{.type = Inst_type::INST_RET}; }
Instruction inst_call(const std::string &f) noexcept { return Instruction{.type = Inst_type::INST_CALL, .operand = f};}
Instruction inst_xor() noexcept { return Instruction{.type = Inst_type::INST_XOR}; }
Instruction inst_and() noexcept { return Instruction{.type = Inst_type::INST_AND}; }
Instruction inst_or() noexcept { return Instruction{.type = Inst_type::INST_OR}; }
Instruction inst_shl(const i64 index, const i64 shift_amount) noexcept { return Instruction{.type = Inst_type::INST_SHL, .operand = std::make_pair(index, shift_amount)}; }
Instruction inst_shr(const i64 index, const i64 shift_amount) noexcept { return Instruction{.type = Inst_type::INST_SHR, .operand = std::make_pair(index, shift_amount)}; }

VM::VM() :m_ip(0), m_halt(0) {}

VM::VM(const std::vector<Instruction> &program) : m_program(program) {}

void VM::set_stack(const std::vector<f64> &stack) & { m_stack = stack; }
const std::vector<f64> VM::get_stack() const& { return m_stack; }

void VM::set_program(const std::vector<Instruction>& prog) & { m_program = prog; }
const std::vector<Instruction> VM::get_program() const& { return m_program; }

void VM::set_ip(const i64 val) & { m_ip = val; }
i64 VM::get_ip() const& { return m_ip; }

void VM::set_memory(const std::string &src)& { m_memory = src; }
const std::string VM::get_memory() const& { return m_memory; }

void VM::set_halt(int halt) & { m_halt = halt; }
const int VM::get_halt() const& { return m_halt; }

i64 VM::get_label_loc(const std::string &l) & {
    return((*m_labels)[l]);
}

void VM::vm_push_inst(const Instruction &inst) {
    m_program.emplace_back(inst);
}

Trap VM::vm_execute_inst(const Instruction &inst) {
    if (m_ip < 0) {
        return Trap::TRAP_ILLEGAL_INST_ACCESS;
    }
    
    switch (inst.type) {
        case Inst_type::INST_NOP:
            m_ip += 1;
            break;

        case Inst_type::INST_PUSH: {
            if (std::holds_alternative<i64>(inst.operand)) {
                i64 value = std::get<i64>(inst.operand);
                m_stack.emplace_back(value); // Push integer to the stack
            } else if (std::holds_alternative<f64>(inst.operand)) {
                f64 value = std::get<f64>(inst.operand);
                m_stack.emplace_back(value); // Push double to the stack
            } else {
                return Trap::TRAP_ILLEGAL_INST; // Invalid operand type
            }
            m_ip += 1;
            break;
        }

        case Inst_type::INST_DUP: {
             if (!std::holds_alternative<i64>(inst.operand)) {
                 return Trap::TRAP_ILLEGAL_INST; // Ensure operand is an integer
             }

             if (const i64 operand = std::get<i64>(inst.operand); m_stack.size() <= static_cast<size_t>(operand)) {
                 return Trap::TRAP_STACK_UNDERFLOW;
             } else {
                 if (operand < 0) {
                     return Trap::TRAP_ILLEGAL_INST;
                 } else {
                     m_stack.emplace_back(m_stack.at(m_stack.size() - 1 - operand));
                     m_ip += 1;
                 }
             }
             break;
        }

        case Inst_type::INST_DROP:
            m_stack.resize(m_stack.size() - 1);
            m_ip += 1;
            break;

        case Inst_type::INST_SWAP: {
            if (m_stack.size() < 2) {
                std::cout << "SWAP";
                return Trap::TRAP_STACK_UNDERFLOW;
            }
            const i64 oper = std::get<i64>(inst.operand);
            std::swap(m_stack.at(m_stack.size() - 1),
                      m_stack.at(m_stack.size() - 1 - oper));
            m_ip += 1;
            break;
        }
        case Inst_type::INST_PLUS:
            if (m_stack.size() < 2) {
                std::cout << "PLUS";
                return Trap::TRAP_STACK_UNDERFLOW;
            } else {
                m_stack.at(m_stack.size() - 2) += m_stack.at(m_stack.size() - 1);
                m_stack.pop_back();
                m_ip += 1;
            }
            break;

        case Inst_type::INST_MINUS:
            if (m_stack.size() < 2) {
                std::cout << "MINUS!";
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

        case Inst_type::INST_JMP: {
            if (!std::holds_alternative<std::string>(inst.operand)) {
                return Trap::TRAP_ILLEGAL_INST;
            }
            if (auto label = std::get<std::string>(inst.operand); m_labels.has_value() && m_labels->find(label) != m_labels->end()) {
                m_ip = (*m_labels)[label]; // Subscript operator finds the "second" member/value from the corresponding key
            } else {
                return Trap::TRAP_ILLEGAL_INST_ACCESS;
            }
            break;
        }
        case Inst_type::INST_EQ:
            if (m_stack.size() < 2) {
                return Trap::TRAP_STACK_UNDERFLOW;
            } else {
                m_stack.at(m_stack.size() - 2) = m_stack.at(m_stack.size() - 1) == m_stack.at(m_stack.size() - 2);
                m_stack.pop_back();
                m_ip += 1;
            }
            break;

        case Inst_type::INST_JMP_IF: {
            if (m_stack.empty()) {
                return Trap::TRAP_STACK_UNDERFLOW;
            }

            // Pop the top element of the stack
            const i64 cond = static_cast<i64>(m_stack.back());
            m_stack.pop_back();

            if (cond != 0) { // Jump if the condition is true
                if (const auto &label = std::get<std::string>(inst.operand); m_labels.has_value() && m_labels->find(label) != m_labels->end()) {
                    m_ip = (*m_labels)[label]; // Set IP to the label's instruction
                    break; // Exit this case without further processing
                } else {
                    return Trap::TRAP_ILLEGAL_INST_ACCESS;
                }
            } else {
                m_ip += 1; // Move to the next instruction if the condition is false
            }
            break;
        }

        case Inst_type::INST_NOT:
            if (m_stack.empty()) {
                return Trap::TRAP_STACK_UNDERFLOW;
            }
            m_stack.back() = !m_stack.back();
            m_ip += 1;
            break;

        case Inst_type::INST_RET: {
            if (m_call_stack.empty()) {
                return Trap::TRAP_STACK_UNDERFLOW;  // No saved instruction pointer
            }
            m_ip = m_call_stack.back();  // Restore the instruction pointer
            m_call_stack.pop_back();
            break;
        }

        case Inst_type::INST_CALL: {
            if (!std::holds_alternative<std::string>(inst.operand)) {
                return Trap::TRAP_ILLEGAL_INST;
            }
            if (const auto &label = std::get<std::string>(inst.operand); m_labels.has_value() && m_labels->find(label) != m_labels->end()) {
                m_call_stack.emplace_back(m_ip + 1);  // Save the next instruction pointer
                m_ip = (*m_labels)[label];  // Jump to the function
            } else {
                return Trap::TRAP_ILLEGAL_INST_ACCESS;
            }
            break;
        }
        case Inst_type::INST_PRINT_DEBUG:
            if (m_stack.empty()) {
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

        case Inst_type::INST_XOR: {
            if (m_stack.size() < 2) {
                return Trap::TRAP_STACK_UNDERFLOW;
            } else {
                const i64 back_1 = static_cast<i64>(m_stack.back());
                m_stack.pop_back();
                const i64 back_2 = static_cast<i64>(m_stack.back());
                m_stack.pop_back();
                m_stack.emplace_back(back_1 ^ back_2);
                m_ip += 1;
                break;
            }
        }

        case Inst_type::INST_AND: {
            if (m_stack.size() < 2) {
                return Trap::TRAP_STACK_UNDERFLOW;
            } else {
                const i64 back_1 = static_cast<i64>(m_stack.back());
                m_stack.pop_back();
                const i64 back_2 = static_cast<i64>(m_stack.back());
                m_stack.pop_back();
                m_stack.emplace_back(back_1 & back_2);
                m_ip += 1;
                break;
            }
        }

        case Inst_type::INST_OR: {
            if (m_stack.size() < 2) {
                return Trap::TRAP_STACK_UNDERFLOW;
            } else {
                const i64 back_1 = static_cast<i64>(m_stack.back());
                m_stack.pop_back();
                const i64 back_2 = static_cast<i64>(m_stack.back());
                m_stack.pop_back();
                m_stack.emplace_back(back_1 | back_2);
                m_ip += 1;
                break;
            }
        }

        case Inst_type::INST_SHL: {
            if (m_stack.empty()) {
                return Trap::TRAP_STACK_UNDERFLOW;
            } else {
                if (!std::holds_alternative<std::pair<i64, i64>>(inst.operand)) {
                    return Trap::TRAP_ILLEGAL_INST;
                }
                else {
                    auto [index, shift_amount] = std::get<std::pair<i64, i64>>(inst.operand);
                    if (index < 0 || static_cast<i64>(index) > m_stack.size()) {
                        return Trap::TRAP_ILLEGAL_INST_ACCESS;
                    }
                    f64 &value = m_stack.at(m_stack.size() - index - 1);
                    if (static_cast<i64>(value) != value) {
                        return Trap::TRAP_ILLEGAL_INST;
                    }
                    else {
                        value = static_cast<f64>(static_cast<i64>(value) << shift_amount);
                        m_ip += 1;
                        break;
                    }
                }
            }
        }

        case Inst_type::INST_SHR: {
            if (m_stack.empty()) {
                return Trap::TRAP_STACK_UNDERFLOW;
            } else {
                if (!std::holds_alternative<std::pair<i64, i64>>(inst.operand)) {
                    return Trap::TRAP_ILLEGAL_INST;
                }
                else {
                    auto [index, shift_amount] = std::get<std::pair<i64, i64>>(inst.operand);
                    if (index < 0 || static_cast<i64>(index) > m_stack.size()) {
                        return Trap::TRAP_ILLEGAL_INST_ACCESS;
                    }
                    f64 &value = m_stack.at(m_stack.size() - index - 1);
                    if (static_cast<i64>(value) != value) {
                        return Trap::TRAP_ILLEGAL_INST;
                    }
                    else {
                        value = static_cast<f64>(static_cast<i64>(value) >> shift_amount);
                        m_ip += 1;
                        break;
                    }
                }
            }
        }

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
        i64 operand;

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

        auto inst_type = static_cast<Inst_type>(type_val);
        m_program.emplace_back(Instruction{.type = inst_type, .operand = operand});
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
    std::vector<std::string> lines{};

    // Tokenize m_memory into words
    for (size_t i = 0; i < m_memory.size(); ++i) {
        if (isspace(m_memory.at(i))) {
            while (i < m_memory.size() && isspace(m_memory.at(i))) {
                ++i;
            }
            --i;
        } else if (isdigit(m_memory.at(i)) || m_memory.at(i) == '.') {
            size_t start = i;
            while (i < m_memory.size() && (isdigit(m_memory.at(i)) || m_memory.at(i) == '.')) {
                ++i;
            }
            lines.emplace_back(m_memory.substr(start, i - start));
            --i;
        } else if (isalnum(m_memory.at(i)) || m_memory.at(i) == ':'  || m_memory.at(i) == '_') {
            size_t start = i;
            while (i < m_memory.size() && (isalnum(m_memory.at(i)) || m_memory.at(i) == ':' || m_memory.at(i) == '_')) {
                ++i;
            }
            lines.emplace_back(m_memory.substr(start, i - start));
            --i;
        } else if (m_memory.at(i) == ';') {
            while (i < m_memory.size() && m_memory.at(i) != '\n') {
                ++i;
            }
        } else if (m_memory.at(i) == '#') {
            std::string hash;
            hash.push_back(m_memory.at(i));
            lines.emplace_back(hash);
        }
    }

    /*
    for (const auto &token : lines) {
        std::cout << "Token: [" << token << "]" << std::endl;
    }
    */
    
    if (!m_labels.has_value()) {
        m_labels = std::unordered_map<std::string, int>{};
    }

    if (!m_macros.has_value()) {
        m_macros = std::unordered_map<std::string, std::variant<int, double, std::string>>{};
    }

    Instruction inst;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].back() == ':') {
            std::string label = lines[i].substr(0, lines[i].size() - 1);
            (*m_labels)[label] = m_program.size();
            continue;
        } else if (lines[i] == "#") {
            if (i + 3 >= lines.size() || lines[i + 1] != "define") {
                std::cerr << "Error: Invalid macro definition. Expected format: # define MACRO_NAME VALUE\n";
                exit(1);
            }
            const std::string &macro_name = lines[i + 2];
            const std::string &macro_value = lines[i + 3];
            ++i; // For skipping 'define'

            if (isdigit(macro_value[0]) || macro_value[0] == '-') {
                if (macro_value.find('.') != std::string::npos) {
                    (*m_macros)[macro_name] = std::stod(macro_value);
                } else {
                    (*m_macros)[macro_name] = std::stoi(macro_value);
                }
            } else {
                (*m_macros)[macro_name] = macro_value;
            }
            i += 2;
        } else if (lines[i] == "nop") {
            inst = inst_nop();
            m_program.emplace_back(inst);
        }else if (lines[i] == "push") {
            if (i + 1 >= lines.size()) {
                std::cerr << "Error: 'push' missing operand.\n";
                exit(1);
            }

            // Check if the operand is a macro
            const std::string &operand = lines[i + 1];
            if (m_macros.has_value() && m_macros->find(operand) != m_macros->end()) {
                const auto &macro_value = (*m_macros)[operand];
                if (std::holds_alternative<int>(macro_value)) {
                    inst = inst_push(static_cast<i64>(std::get<int>(macro_value))); // Use std::get<int>
                } else if (std::holds_alternative<double>(macro_value)) {
                    inst = inst_push(std::get<double>(macro_value));
                } else {
                    std::cerr << "Error: Macro '" << operand << "' cannot be used as a numeric operand for 'push'.\n";
                    exit(1);
                }
            } else {
                // Handle direct numeric values
                if (operand.find('.') != std::string::npos) {
                    try {
                        inst = inst_push(std::stod(operand));
                    } catch (const std::invalid_argument &) {
                        std::cerr << "Error: Invalid floating-point value for 'push': " << operand << '\n';
                        exit(1);
                    }
                } else {
                    try {
                        inst = inst_push(static_cast<i64>(std::stoi(operand))); // Use std::stoi and cast to i64
                    } catch (const std::invalid_argument &) {
                        std::cerr << "Error: Invalid integer value for 'push': " << operand << '\n';
                        exit(1);
                    }
                }
            }
            m_program.emplace_back(inst);
            ++i; // Skip the operand
        } else if (lines[i] == "swap") {
            int operand{};
            try {
                operand = std::stoi(lines.at(i + 1));
            } catch (const std::invalid_argument &e) {
                std::cerr << "Error: swap has invalid operand. " << e.what() << '\n';
                exit(1);
            }
            inst = inst_swap(operand);
            m_program.emplace_back(inst);
            ++i;
        } else if (lines[i] == "dup") {
            if (i + 1 >= lines.size()) {
                std::cerr << "Error: 'dup' missing operand.\n";
                exit(1);
            }
            int val{};
            try {
                val = std::stoi(lines.at(i + 1));
            } catch (const std::invalid_argument &e) {
                std::cerr << "Failed to parse operand for 'dup': " << e.what() << '\n';
                exit(1);
            }
            inst = inst_dup(val);
            m_program.emplace_back(inst);
            ++i;
        } else if (lines[i] == "drop") {
          inst = inst_drop();
          m_program.emplace_back(inst);
        } else if (lines[i] == "plus") {
            inst = inst_plus();
            m_program.emplace_back(inst);
        } else if (lines[i] == "minus") {
            inst = inst_minus();
            m_program.emplace_back(inst);
        } else if (lines[i] == "mult") {
            inst = inst_mult();
            m_program.emplace_back(inst);
        } else if (lines[i] == "div") {
            inst = inst_div();
            m_program.emplace_back(inst);
        } else if (lines[i] == "jmp") {
            if (i + 1 >= lines.size()) {
                std::cerr << "Error: 'jmp' missing operand.\n";
                exit(1);
            }
            const std::string &operand = lines[i + 1];
            if (std::isdigit(operand[0])) {
                int addr = std::stoi(operand);
                inst = inst_jmp(addr);
            } else {
                inst = inst_jmp(operand);
            }
            m_program.emplace_back(inst);
            ++i;
        } else if (lines[i] == "jmp_if") {
            if (i + 1 >= lines.size()) {
                std::cerr << "Error: 'jmp_if' missing operand.\n";
                exit(1);
            }
            const std::string &operand = lines[i + 1];
            inst = Instruction{.type = Inst_type::INST_JMP_IF, .operand = operand};
            m_program.emplace_back(inst);
            ++i;
        }
        else if (lines[i] == "not") {
            inst = inst_not();
            m_program.emplace_back(inst);
        } else if (lines[i] == "halt") {
            inst = inst_halt();
            m_program.emplace_back(inst);
        } else if (lines[i] == "ret") {
            inst = inst_ret();
            m_program.emplace_back(inst);
        } else if (lines[i] == "call") {
            if (i + 1 >= lines.size()) {
                std::cerr << "Error: 'call' missing operand.\n";
                exit(1);
            }
            const std::string &operand = lines[i + 1];
            inst = inst_call(operand);
            m_program.emplace_back(inst);
            i += 1;
        } else if (lines[i] == "xor") {
            inst = inst_xor();
            m_program.emplace_back(inst);
        } else if (lines[i] == "and") {
            inst = inst_and();
            m_program.emplace_back(inst);
        } else if (lines[i] == "or") {
            inst = inst_or();
            m_program.emplace_back(inst);
        } else if (lines[i] == "shl") {
            if (i + 1 >= lines.size()) {
                std::cerr << "Error: 'shr' missing operand.\n";
            } else {
                const std::string &indx = lines[i + 1];
                const std::string &value = lines[i + 2];
                i64 i_val {}, v_val {};
                try {
                    i_val= std::stoi(indx);
                    v_val = std::stoi(value);
                } catch (const std::invalid_argument &e) {
                    std::cerr << "Error: Invalid integer value for 'shr': " << e.what() << '\n';
                    exit(1);
                }
                inst = inst_shl(i_val, v_val);
                m_program.emplace_back(inst);
                i += 2;
            }
        } else if (lines[i] == "shr") {
            if (i + 1 >= lines.size()) {
                std::cerr << "Error: 'shr' missing operand.\n";
            } else {
                const std::string &indx = lines[i + 1];
                const std::string &value = lines[i + 2];
                i64 i_val {}, v_val {};
                try {
                    i_val= std::stoi(indx);
                    v_val = std::stoi(value);
                } catch (const std::invalid_argument &e) {
                    std::cerr << "Error: Invalid integer value for 'shr': " << e.what() << '\n';
                    exit(1);
                }
                inst = inst_shr(i_val, v_val);
                m_program.emplace_back(inst);
                i += 2;
            }
        }
        else {
            std::cerr << "Unknown instruction: " << lines[i] << '\n';
            exit(1);
        }
    }
}

void VM::vm_dump_stack() const {
    std::cout << "Stack:\n";
    if(!m_stack.empty()) {
        for (size_t i = 0; i < m_stack.size(); ++i) {
            std::cout << "  " << m_stack.at(i) << '\n';
        }
    } else {
        std::cout << "[empty]\n";
    }
}
