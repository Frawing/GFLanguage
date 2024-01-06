#pragma once

#include <sstream>
#include <cassert>
#include <algorithm>
#include "Parser.h"

class Generator{

    public:

        explicit inline Generator(NodeProg p_prog) : prog(std::move(p_prog)) {}

        void gen_term(const NodeTerm* term);
        void gen_bin_expr(const NodeBinExpr* bin_expr);
        void gen_expr(const NodeExpr* expr);
        void gen_scope(const NodeScope* scope);
        void gen_if_pred(const NodeIfPred* pred, const std::string end_label);
        void gen_stmt(const NodeStmt* stmt);
        const inline std::string gen_prog()
        {
            output << "global _start\n_start:\n";

            for(auto s : prog.stmts){
                gen_stmt(s);
            }

            output << "    mov rax, 60\n";
            output << "    mov rdi, 0\n";
            output << "    syscall\n";
            return output.str();
        }

    private:

        const inline void push(std::string reg){
            output << "    push " << reg << "\n";
            stack_size++;
        }
        const inline void pop(std::string reg){
            output << "    pop " << reg << "\n";
            stack_size--;
        }

        const inline void begin_scope(){
            scopes.push_back(variables.size());
        }
        const inline void end_scope(){
            size_t pop_count = variables.size() - scopes.back();
            output << "    add rsp, " << pop_count * 8 << "\n";
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
        std::stringstream output;
        size_t stack_size;
        std::vector<Var> variables;
        std::vector<size_t> scopes;
        size_t label_size = 0;

};