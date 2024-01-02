#pragma once

#include <sstream>
#include <unordered_map>
#include "Parser.h"

class Generator{

    public:

        inline Generator(NodeProg p_prog) : prog(std::move(p_prog)) {}

        inline void gen_expr(NodeExpr expr)
        {
            struct ExprVisitor{

                Generator* gen;
                inline ExprVisitor(Generator* p_gen) : gen(p_gen) {}

                void operator()(NodeExprInt expr){
                    gen->output << "    mov rax, " << expr.integer.value.value() << "\n";
                    gen->push("rax");
                }
                void operator()(NodeExprIdent expr){
                    if(!gen->variables.contains(expr.indent.value.value())){
                        std::cerr << "Variabile inesistente : '" << expr.indent.value.value() << "'" << std::endl;
                        exit(1);
                    }

                    const auto var = gen->variables.at(expr.indent.value.value()); 
                    std::stringstream offset;
                    offset << " QWORD [rsp + " << (gen->stack_size - var.stack_pos - 1) * 8 << "]\n";
                    gen->push(offset.str());
                }
            };

            ExprVisitor visitor(this);
            std::visit(visitor, expr.value);
        }

        inline void gen_stmt(NodeStmt stmt)
        {
            struct StmtVisitor{

                Generator* gen;
                inline StmtVisitor(Generator* p_gen) : gen(p_gen) {}

                void operator()(const NodeStmtExit stmt){
                    gen->gen_expr(stmt.expr);
                    gen->output << "    mov rax, 60" << "\n";
                    gen->output << "    pop rdi" << "\n";
                    gen->output << "    syscall" << "\n";
                }
                void operator()(const NodeStmtLet stmt){
                    if(gen->variables.contains(stmt.ident.value.value())){
                        std::cerr << "La variabile '" << stmt.ident.value.value() << "' è già stata dichiarata!" << std::endl;
                        exit(1);
                    }
                    gen->variables.insert({stmt.ident.value.value(), Var {.stack_pos = gen->stack_size}});
                    gen->gen_expr(stmt.expr);
                }
            };

            StmtVisitor visitor(this);
            std::visit(visitor, stmt.stmt);
        }

        inline std::string gen_prog()
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

        void push(std::string reg){
            output << "    push " << reg << "\n";
            stack_size++;
        }
        void pop(std::string reg){
            output << "    push " << reg << "\n";
            stack_size--;
        }

        struct Var{
            size_t stack_pos;
        };

        const NodeProg prog;
        std::stringstream output;
        size_t stack_size;
        std::unordered_map<std::string, Var> variables;

};