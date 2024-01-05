#pragma once

#include <sstream>
#include <cassert>
#include "Parser.h"

class Generator{

    public:

        inline Generator(NodeProg p_prog) : prog(std::move(p_prog)) {}

        inline void gen_term(const NodeTerm* term)
        {
            struct TermVisitor{

                Generator* gen;
                inline TermVisitor(Generator* p_gen) : gen(p_gen) {}

                void operator()(const NodeTermInt* term_int) const {
                    gen->output << "    mov rax, " << term_int->integer.value.value() << "\n";
                    gen->push("rax");
                }
                void operator()(const NodeTermIdent* term_ident) const {
                    auto it = std::find_if(gen->variables.cbegin(), gen->variables.cend(),
                              [&](const Var& var) { return var.name == term_ident->indent.value.value(); });

                    if(it == gen->variables.cend()){
                        std::cerr << "Undeclared variable : " << term_ident->indent.value.value() << std::endl;
                        exit(1);
                    }

                    std::stringstream offset;
                    offset << "QWORD [rsp + " << (gen->stack_size - (*it).stack_pos - 1) * 8 << "]";
                    gen->push(offset.str());
                }
                void operator()(const NodeTermParen* node_paren) const {
                    gen->gen_expr(node_paren->expr);
                }

            };

            TermVisitor visitor(this);
            std::visit(visitor, term->value);
        }

        inline void gen_bin_expr(const NodeBinExpr* bin_expr){
            struct BinExprVisitor{

                Generator* gen;
                inline BinExprVisitor(Generator* p_gen) : gen(p_gen) {}

                void operator()(const NodeBinExprAdd* bin_expr_add) const {
                    gen->gen_expr(bin_expr_add->lhs);
                    gen->gen_expr(bin_expr_add->rhs);
                    gen->pop("rbx");
                    gen->pop("rax");
                    gen->output << "    add rax, rbx\n";
                    gen->push("rax");
                }
                void operator()(const NodeBinExprSub* bin_expr_sub) const {
                    gen->gen_expr(bin_expr_sub->lhs);
                    gen->gen_expr(bin_expr_sub->rhs);
                    gen->pop("rbx");
                    gen->pop("rax");
                    gen->output << "    sub rax, rbx\n";
                    gen->push("rax");
                }
                void operator()(const NodeBinExprMul* bin_expr_mul) const {
                    gen->gen_expr(bin_expr_mul->lhs);
                    gen->gen_expr(bin_expr_mul->rhs);
                    gen->pop("rbx");
                    gen->pop("rax");
                    gen->output << "    mul rbx\n";
                    gen->push("rax");
                }
                void operator()(const NodeBinExprDiv* bin_expr_div) const {
                    gen->gen_expr(bin_expr_div->lhs);
                    gen->gen_expr(bin_expr_div->rhs);
                    gen->pop("rbx");
                    gen->pop("rax");
                    gen->output << "    div rbx\n";
                    gen->push("rax");
                }

            };

            BinExprVisitor visitor(this);
            std::visit(visitor, bin_expr->value);
        }

        inline void gen_expr(const NodeExpr* expr)
        {
            struct ExprVisitor{

                Generator* gen;
                inline ExprVisitor(Generator* p_gen) : gen(p_gen) {}

                void operator()(const NodeBinExpr* bin_expr) const {
                    gen->gen_bin_expr(bin_expr);
                }
                void operator()(const NodeTerm* term) const {
                    gen->gen_term(term);
                }
            };

            ExprVisitor visitor(this);
            std::visit(visitor, expr->value);
        }

        inline void gen_scope(const NodeScope* scope) {
            begin_scope();
            for(const NodeStmt* stmt : scope->stmts){
                gen_stmt(stmt);
            }
            end_scope();
        }

        inline void gen_stmt(const NodeStmt* stmt)
        {
            struct StmtVisitor{

                Generator* gen;
                inline StmtVisitor(Generator* p_gen) : gen(p_gen) {}

                void operator()(const NodeStmtExit* stmt) const {
                    gen->gen_expr(stmt->expr);
                    gen->output << "    mov rax, 60" << "\n";
                    gen->output << "    pop rdi" << "\n";
                    gen->output << "    syscall" << "\n";
                }
                void operator()(const NodeStmtLet* stmt) const {
                    auto it = std::find_if(gen->variables.cbegin(), gen->variables.cend(),
                              [&](const Var& var) { return var.name == stmt->ident.value.value(); });
                    
                    if(it != gen->variables.cend())
                    {
                        std::cerr << "La variabile '" << stmt->ident.value.value() << "' è già stata dichiarata!" << std::endl;
                        exit(1);
                    }
                    gen->variables.push_back({stmt->ident.value.value(), gen->stack_size});
                    gen->gen_expr(stmt->expr);
                }
                void operator()(const NodeScope* scope) const {
                    gen->gen_scope(scope);
                }
                void operator()(const NodeStmtIf* if_stmt) const {
                    gen->gen_expr(if_stmt->cond);
                    gen->pop("rax");
                    std::string label = gen->create_label();
                    gen->output << "    test rax, rax\n";
                    gen->output << "    jz " << label << "\n";
                    gen->gen_scope(if_stmt->scope);
                    gen->output << label << ":\n";
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
            std::string name;
            size_t stack_pos;
        };

        void begin_scope(){
            scopes.push_back(variables.size());
        }

        void end_scope(){
            size_t pop_count = variables.size() - scopes.back();
            output << "    add rsp, " << pop_count * 8 << "\n";
            stack_size -= pop_count;
            for(int i = 0; i < pop_count; i++){
                variables.pop_back();
            }
            scopes.pop_back();
        }

        std::string create_label(){
            std::stringstream ss;
            ss << "label" << label_size++;
            return ss.str();
        }

        const NodeProg prog;
        std::stringstream output;
        size_t stack_size;
        std::vector<Var> variables;
        std::vector<size_t> scopes;
        size_t label_size = 0;

};