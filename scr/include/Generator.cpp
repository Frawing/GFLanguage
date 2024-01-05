#include "Generator.h"

void Generator::gen_term(const NodeTerm* term)
{
    struct TermVisitor
    {
        Generator* gen;
        inline TermVisitor(Generator* p_gen) : gen(p_gen) {}

        const void operator()(const NodeTermInt* term_int){
            gen->output << "    mov rax, " << term_int->integer.value.value() << "\n";
            gen->push("rax");
        }

        const void operator()(const NodeTermIdent* term_ident)
        {
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

        const void operator()(const NodeTermParen* node_paren){
            gen->gen_expr(node_paren->expr);
        }
    };

    TermVisitor visitor(this);
    std::visit(visitor, term->value);
}

// ----------------------------------------

void Generator::gen_bin_expr(const NodeBinExpr* bin_expr)
{
    struct BinExprVisitor
    {
        Generator* gen;
        inline BinExprVisitor(Generator* p_gen) : gen(p_gen) {}

        const void operator()(const NodeBinExprAdd* bin_expr_add)
        {
            gen->gen_expr(bin_expr_add->lhs);
            gen->gen_expr(bin_expr_add->rhs);
            gen->pop("rbx");
            gen->pop("rax");
            gen->output << "    add rax, rbx\n";
            gen->push("rax");
        }

        const void operator()(const NodeBinExprSub* bin_expr_sub)
        {
            gen->gen_expr(bin_expr_sub->lhs);
            gen->gen_expr(bin_expr_sub->rhs);
            gen->pop("rbx");
            gen->pop("rax");
            gen->output << "    sub rax, rbx\n";
            gen->push("rax");
        }

        const void operator()(const NodeBinExprMul* bin_expr_mul)
        {
            gen->gen_expr(bin_expr_mul->lhs);
            gen->gen_expr(bin_expr_mul->rhs);
            gen->pop("rbx");
            gen->pop("rax");
            gen->output << "    mul rbx\n";
            gen->push("rax");
        }

        const void operator()(const NodeBinExprDiv* bin_expr_div)
        {
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

// ----------------------------------------

void Generator::gen_expr(const NodeExpr* expr)
{
    struct ExprVisitor
    {
        Generator* gen;
        inline ExprVisitor(Generator* p_gen) : gen(p_gen) {}

        const void operator()(const NodeBinExpr* bin_expr){
            gen->gen_bin_expr(bin_expr);
        }

        const void operator()(const NodeTerm* term){
            gen->gen_term(term);
        }
    };

    ExprVisitor visitor(this);
    std::visit(visitor, expr->value);
}

// ----------------------------------------

void Generator::gen_scope(const NodeScope* scope)
{
    begin_scope();

    for(const NodeStmt* stmt : scope->stmts){
        gen_stmt(stmt);
    }

    end_scope();
}

void Generator::gen_stmt(const NodeStmt* stmt)
{
    struct StmtVisitor
    {
        Generator* gen;
        inline StmtVisitor(Generator* p_gen) : gen(p_gen) {}

        const void operator()(const NodeStmtExit* stmt)
        {
            gen->gen_expr(stmt->expr);
            gen->output << "    mov rax, 60" << "\n";
            gen->output << "    pop rdi" << "\n";
            gen->output << "    syscall" << "\n";
        }

        const void operator()(const NodeStmtLet* stmt)
        {
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

        const void operator()(const NodeScope* scope) {
            gen->gen_scope(scope);
        }

        const void operator()(const NodeStmtIf* if_stmt)
        {
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