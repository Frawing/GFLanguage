#pragma once

#include <sstream>
#include <cassert>
#include <algorithm>
#include "Parser.h"

class Generator
{
    public:

        explicit inline Generator(NodeProg p_prog) : prog(std::move(p_prog)) {}

        void gen_term(const NodeTerm* term);
        void gen_bin_expr(const NodeBinExpr* bin_expr);
        void gen_text_expr(const NodeTextExpr* text_expr);
        void gen_int_expr(const NodeIntExpr* int_expr);
        void gen_expr(const NodeExpr* expr);
        void gen_scope(const NodeScope* scope);
        void gen_if_pred(const NodeIfPred* pred, const std::string end_label);
        void gen_stmt(const NodeStmt* stmt);
        const inline std::string gen_prog()
        {
            /* Base Data */
            output_data << "section .data\n";
            output_data << "    new_line:\n";
            output_data << "        db 10, 0\n";

            /* Base Text */
            output_text << "\nsection .text\n\n";
            output_text << "global _start\n\n";

            /* Functions */
            /* Str Len */
            output_text << "_strlen:\n";
            push("rbx");
            push("rcx");
            output_text << "    mov rbx, rdi\n";
            output_text << "    xor al, al\n";
            output_text << "    mov rcx, 0xffffffff\n";
            output_text << "    repne scasb\n";
            output_text << "    sub rdi, rbx\n";
            output_text << "    mov rax, rdi\n";
            pop("rcx");
            pop("rbx");
            output_text << "    ret\n";

            /* Start */
            output_text << "\n_start:\n";

            for(auto s : prog.stmts){
                gen_stmt(s);
            }

            output_text << "    mov rax, 60\n";
            output_text << "    mov rdi, 0\n";
            output_text << "    syscall\n";

            std::stringstream output;
            output << output_data.str() << "\n" << output_text.str();
            return output.str();
        }

    private:

        const inline void push(std::string reg){
            output_text << "    push " << reg << "\n";
            stack_size++;
        }
        const inline void pop(std::string reg){
            output_text << "    pop " << reg << "\n";
            stack_size--;
        }

        const inline void begin_scope(){
            scopes.push_back(variables.size());
        }
        const inline void end_scope(){
            size_t pop_count = variables.size() - scopes.back();
            output_text << "    add rsp, " << pop_count * 8 << "\n";
            stack_size -= pop_count;
            for(int i = 0; i < pop_count; i++){
                variables.pop_back();
            }
            scopes.pop_back();
        }

        const inline std::string create_label(){
            std::stringstream ss;
            ss << "label" << label_size++;
            return ss.str();
        }

        struct Var{
            std::string name;
            size_t stack_pos;
        };

        const NodeProg prog;

        std::stringstream output_text;
        std::stringstream output_data;
        std::stringstream string_section;
        
        size_t stack_size;
        std::vector<Var> variables;
        std::vector<size_t> scopes;
        size_t label_size = 0;
};