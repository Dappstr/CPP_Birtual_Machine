#pragma once

#include <cstdint>
#include <unordered_map>
#include <variant>
#include <vector>
#include <string>
#include <optional>
#include <unordered_map>

enum class Trap {
    TRAP_OK = 0,
    TRAP_STACK_OVERFLOW,
    TRAP_STACK_UNDERFLOW,
    TRAP_ILLEGAL_INST,
    TRAP_DIV_BY_ZERO,
    TRAP_ILLEGAL_INST_ACCESS,
};

const std::string trap_as_str(Trap trap) noexcept;

enum class Inst_type {
    INST_NOP,
    INST_PUSH,
    INST_DUP,
    INST_DROP,
    INST_SWAP,
    INST_PLUS,
    INST_MINUS,
    INST_MULT,
    INST_DIV,
    INST_JMP,
    INST_JMP_IF,
    INST_EQ,
    INST_HALT,
    INST_NOT,
    INST_RET,
    INST_CALL,
    INST_PRINT_DEBUG,
};

using i64 = int64_t;
using f64 = double;
using Operand = std::variant<i64, f64, std::string>;

struct Instruction {
    Inst_type type;
    Operand operand;
};

const std::string inst_as_str(const Inst_type &type) noexcept;

[[nodiscard]] Instruction inst_nop() noexcept ;
[[nodiscard]] Instruction inst_push(i64) noexcept ;
[[nodiscard]] Instruction inst_push(f64) noexcept ;
[[nodiscard]] Instruction inst_swap(i64) noexcept ;
[[nodiscard]] Instruction inst_dup(i64) noexcept;
[[nodiscard]] Instruction inst_drop() noexcept;
[[nodiscard]] Instruction inst_plus() noexcept;
[[nodiscard]] Instruction inst_minus() noexcept;
[[nodiscard]] Instruction inst_mult() noexcept;
[[nodiscard]] Instruction inst_div() noexcept;
[[nodiscard]] Instruction inst_jmp(i64) noexcept;
[[nodiscard]] Instruction inst_jmp(const std::string &) noexcept;
[[nodiscard]] Instruction inst_jmp_if(const std::string&) noexcept;
[[nodiscard]] Instruction inst_halt() noexcept;
[[nodiscard]] Instruction inst_not() noexcept;
[[nodiscard]] Instruction inst_ret() noexcept;
[[nodiscard]] Instruction inst_call(const std::string &) noexcept;

class VM final {
private:
    std::vector<f64> m_stack{};
    std::vector<Instruction> m_program{};
    i64 m_ip{}; // Instruction Pointer
    std::string m_memory{};
    int m_halt{};

    std::optional<std::unordered_map<std::string, int>> m_labels{};
    std::vector<i64> m_call_stack{};
    
public:
    VM ();
    explicit VM(const std::vector<Instruction> &);

    void set_stack(const std::vector<f64>&) &;
    const std::vector<f64> get_stack() const&;

    void set_program(const std::vector<Instruction>&) &;
    const std::vector<Instruction> get_program() const&;
    
    void set_ip(const i64) &;
    i64 get_ip() const&;
    
    void set_memory(const std::string &) &;
    const std::string get_memory() const&;

    void set_halt(int) &;
    const int get_halt() const&;

    i64 get_label_loc(const std::string &) &;
    
    void vm_push_inst(const Instruction &);
    Trap vm_execute_inst(const Instruction &);
    void vm_load_program_from_memory(const std::vector<Instruction> &);
    void vm_load_program_from_file(const std::string &);
    void vm_save_program_to_file(const std::string &);
    void vm_translate_asm();
    void vm_parse_labels(const std::vector<std::string> &);
    void vm_dump_stack() const;
};
