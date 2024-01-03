#pragma once

#include <sstream>
#include <unordered_map>
#include "Parser.h"
#include <cassert>

class Generator{

    public:

        inline Generator(NodeProg p_prog) : prog(std::move(p_prog)) {}

        inline void gen_term(const NodeTerm* term)
        {
            struct TermVisitor{

                Generator* gen;
                inline TermVisitor(Generator* p_gen) : gen(p_gen) {}

                void operator()(const NodeTermInt* term_int){
                    gen->output << "    mov rax, " << term_int->integer.value.value() << "\n";
                    gen->push("rax");
                }
                void operator()(const NodeTermIdent* term_ident){
                    if(!gen->variables.contains(term_ident->indent.value.value())){
                        std::cerr << "Undeclared variable : " << term_ident->indent.value.value() << std::endl;
                        exit(1);
                    }
                    const auto var = gen->variables.at(term_ident->indent.value.value());
                    std::stringstream offset;
                    offset << "QWORD [rsp + " << (gen->stack_size - var.stack_pos - 1) * 8 << "]";
                    gen->push(offset.str());
                }

            };

            TermVisitor visitor(this);
            std::visit(visitor, term->value);
        }

        inline void gen_expr(const NodeExpr* expr)
        {
            struct ExprVisitor{

                Generator* gen;
                inline ExprVisitor(Generator* p_gen) : gen(p_gen) {}

                void operator()(const NodeBinExpr* bin_expr){
                    gen->gen_expr(bin_expr->value->lhs);
                    gen->gen_expr(bin_expr->value->rhs);
                    gen->pop("rbx");
                    gen->pop("rax");
                    gen->output << "    add rax, rbx\n";
                    gen->push("rax");
                }
                void operator()(const NodeTerm* term){
                    gen->gen_term(term);
                }
            };

            ExprVisitor visitor(this);
            std::visit(visitor, expr->value);
        }

        inline void gen_stmt(const NodeStmt* stmt)
        {
            struct StmtVisitor{

                Generator* gen;
                inline StmtVisitor(Generator* p_gen) : gen(p_gen) {}

                void operator()(const NodeStmtExit* stmt){
                    gen->gen_expr(stmt->expr);
                    gen->output << "    mov rax, 60" << "\n";
                    gen->output << "    pop rdi" << "\n";
                    gen->output << "    syscall" << "\n";
                }
                void operator()(const NodeStmtLet* stmt){
                    if(gen->variables.contains(stmt->ident.value.value())){
                        std::cerr << "La variabile '" << stmt->ident.value.value() << "' è già stata dichiarata!" << std::endl;
                        exit(1);
                    }
                    gen->variables.insert({stmt->ident.value.value(), Var {.stack_pos = gen->stack_size}});
                    gen->gen_expr(stmt->expr);
                }
            };

            StmtVisitor visitor(this);
            std::visit(visitor, stmt->stmt);
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
            output << "    pop " << reg << "\n";
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