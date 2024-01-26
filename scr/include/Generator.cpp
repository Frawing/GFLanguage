#include "Generator.h"

void Generator::gen_term(const NodeTerm* term)
{
    struct TermVisitor
    {
        Generator* gen;
        inline TermVisitor(Generator* p_gen) : gen(p_gen) {}

        const void operator()(const NodeTermInt* term_int){
            gen->output_text << "    mov rax, " << term_int->token_integer.value.value() << "\n";
            gen->push("rax");
        }

        const void operator()(const NodeTermIdent* term_ident)
        {
            auto it = std::find_if(gen->variables.cbegin(), gen->variables.cend(),
                        [&](const Var& var) { return var.name == term_ident->token_ident.value.value(); });

            if(it == gen->variables.cend()){
                std::cerr << "Undeclared variable : " << term_ident->token_ident.value.value() << std::endl;
                exit(1);
            }

            std::stringstream offset;
            offset << "QWORD [rsp + " << (gen->stack_size - (*it).stack_pos - 1) * 8 << "]";
            gen->push(offset.str());
        }

        const void operator()(const NodeTermParen* node_paren){
            gen->gen_int_expr(node_paren->int_expr);
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
            gen->gen_int_expr(bin_expr_add->lhs);
            gen->gen_int_expr(bin_expr_add->rhs);
            gen->pop("rbx");
            gen->pop("rax");
            gen->output_text << "    add rax, rbx\n";
            gen->push("rax");
        }

        const void operator()(const NodeBinExprSub* bin_expr_sub)
        {
            gen->gen_int_expr(bin_expr_sub->lhs);
            gen->gen_int_expr(bin_expr_sub->rhs);
            gen->pop("rbx");
            gen->pop("rax");
            gen->output_text << "    sub rax, rbx\n";
            gen->push("rax");
        }

        const void operator()(const NodeBinExprMul* bin_expr_mul)
        {
            gen->gen_int_expr(bin_expr_mul->lhs);
            gen->gen_int_expr(bin_expr_mul->rhs);
            gen->pop("rbx");
            gen->pop("rax");
            gen->output_text << "    mul rbx\n";
            gen->push("rax");
        }

        const void operator()(const NodeBinExprDiv* bin_expr_div)
        {
            gen->gen_int_expr(bin_expr_div->lhs);
            gen->gen_int_expr(bin_expr_div->rhs);
            gen->pop("rbx");
            gen->pop("rax");
            gen->output_text << "    div rbx\n";
            gen->push("rax");
        }
    };

    BinExprVisitor visitor(this);
    std::visit(visitor, bin_expr->value);
}

// ----------------------------------------

void Generator::gen_text_expr(const NodeTextExpr* text_expr)
{
    struct TextExprVisitor
    {
        Generator* gen;
        inline TextExprVisitor(Generator* p_gen) : gen(p_gen) {}

        const void operator()(const NodeTextString* text_string)
        {
            std::string label = gen->create_label();
            gen->output_data << "    " << label << ":\n";
            gen->output_data << "        db \"" << text_string->token_string.value.value() << "\", 0\n";
            gen->output_text << "    mov rax, " << label << "\n";
            gen->push("rax");
            gen->output_text << "    mov rax, " << label << "\n";
            gen->push("rax");
        }
        const void operator()(const NodeTextIdent* text_ident)
        {
            auto it = std::find_if(gen->variables.cbegin(), gen->variables.cend(),
                        [&](const Var& var) { return var.name == text_ident->token_ident.value.value(); });

            if(it == gen->variables.cend()){
                std::cerr << "Undeclared variable : " << text_ident->token_ident.value.value() << std::endl;
                exit(1);
            }

            std::stringstream offset;
            offset << "QWORD [rsp + " << ((gen->stack_size - (*it).stack_pos - 1) * 8) - 8 << "]";
            gen->push(offset.str());
        }
    };

    TextExprVisitor visitor(this);
    std::visit(visitor, text_expr->value);
}

// ----------------------------------------

void Generator::gen_int_expr(const NodeIntExpr* int_expr)
{
    struct IntExprVisitor
    {
        Generator* gen;
        inline IntExprVisitor(Generator* p_gen) : gen(p_gen) {}

        const void operator()(const NodeBinExpr* bin_expr){
            gen->gen_bin_expr(bin_expr);
        }

        const void operator()(const NodeTerm* term){
            gen->gen_term(term);
        }
    };

    IntExprVisitor visitor(this);
    std::visit(visitor, int_expr->value);
}

// ----------------------------------------

void Generator::gen_expr(const NodeExpr* expr)
{
    struct ExprVisitor
    {
        Generator* gen;
        inline ExprVisitor(Generator* p_gen) : gen(p_gen) {}

        const void operator()(const NodeTextExpr* text_expr){
            gen->gen_text_expr(text_expr);
        }

        const void operator()(const NodeIntExpr* int_expr){
            gen->gen_int_expr(int_expr);
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

// ----------------------------------------

void Generator::gen_if_pred(const NodeIfPred* pred, const std::string end_label)
{
    struct PredVisitor{
        Generator& gen;
        const std::string end_label = end_label;

        const void operator()(const NodeIfPredElif* pred_elif){
            gen.gen_int_expr(pred_elif->int_expr_1);
            gen.gen_int_expr(pred_elif->int_expr_2);
            gen.pop("rbx");
            gen.pop("rax");
            std::string label = gen.create_label();

            switch (pred_elif->comp_simb->token_simb.type)
            {
                case TokenType::MAJOR:
                    gen.output_text << "    cmp rax, rbx\n";
                    gen.output_text << "    jle " << label << "\n"; // jump if less or equal
                    break;
                case TokenType::MINOR:
                    gen.output_text << "    cmp rax, rbx\n";
                    gen.output_text << "    jge " << label << "\n"; // jump if greater or equal
                    break;
                case TokenType::DBL_EQUAL:
                    gen.output_text << "    cmp rax, rbx\n";
                    gen.output_text << "    jne " << label << "\n"; // jump if greater or equal
                    break;
                case TokenType::NOT_EQUAL:
                    gen.output_text << "    cmp rax, rbx\n";
                    gen.output_text << "    je " << label << "\n"; // jump if greater or equal
                    break;

                default:
                    break;
            }

            gen.gen_scope(pred_elif->scope);
            gen.output_text << "    jmp " << end_label << "\n";
            if(pred_elif->pred.has_value()){
                gen.output_text << label << ":\n";
                gen.gen_if_pred(pred_elif->pred.value(), end_label);
            }
        }
        const void operator()(const NodeIfPredElse* pred_else){
            gen.gen_scope(pred_else->scope);
        }
    };

    PredVisitor visitor{.gen = *this, .end_label = end_label };
    std::visit(visitor, pred->value);
}

// ----------------------------------------

void Generator::gen_stmt(const NodeStmt* stmt)
{
    struct StmtVisitor
    {
        Generator& gen;

        const void operator()(const NodeStmtExit* stmt)
        {
            gen.gen_int_expr(stmt->int_expr);
            gen.output_text << "    mov rax, 60" << "\n";
            gen.output_text << "    pop rdi" << "\n";
            gen.output_text << "    syscall" << "\n";
        }

        const void operator()(const NodeStmtPrint* stmt)
        {
            gen.gen_text_expr(stmt->text_expr);
            gen.pop("rdi");
            gen.output_text << "    call GF__strlen\n";
            gen.pop("rsi");
            gen.output_text << "    mov rdx, rax\n";
            gen.output_text << "    mov rax, 1\n";
            gen.output_text << "    mov rdi, 1\n";
            gen.output_text << "    syscall\n";

            if(stmt->new_line){
                gen.output_text << "    mov rsi, GF_new_line\n";
                gen.output_text << "    mov rdx, 2\n";
                gen.output_text << "    mov rax, 1\n";
                gen.output_text << "    mov rdi, 1\n";
                gen.output_text << "    syscall\n";
            }
        }

        const void operator()(const NodeStmtInput* stmt)
        {
            std::string label = gen.create_label();
            gen.output_data << "    " << label << " resb 255\n";
            gen.output_text << "    mov rdx, 255\n";
            gen.output_text << "    mov rsi," << label << "\n";
            gen.output_text << "    mov rdi, 1\n";
            gen.output_text << "    mov rax, 0\n";
            gen.output_text << "    syscall\n";
            
            if(stmt->var.has_value())
            {
                auto it = std::find_if(gen.variables.cbegin(), gen.variables.cend(),
                            [&](const Var& var) { return var.name == stmt->var.value()->token_ident.value.value(); });
                if(it != gen.variables.cend())
                {
                    std::cerr << "La variabile '" << stmt->var.value()->token_ident.value.value() << "' è già stata dichiarata!" << std::endl;
                    exit(1);
                }
                gen.variables.push_back({stmt->var.value()->token_ident.value.value(), gen.stack_size});
                gen.push(label);
            }
        }

        const void operator()(const NodeStmtLet* stmt)
        {
            auto it = std::find_if(gen.variables.cbegin(), gen.variables.cend(),
                        [&](const Var& var) { return var.name == stmt->token_ident.value.value(); });
            
            if(it != gen.variables.cend())
            {
                std::cerr << "La variabile '" << stmt->token_ident.value.value() << "' è già stata dichiarata!" << std::endl;
                exit(1);
            }
            gen.variables.push_back({stmt->token_ident.value.value(), gen.stack_size});
            gen.gen_expr(stmt->expr);
        }

        const void operator()(const NodeStmtAssign* stmt)
        {
            auto it = std::find_if(gen.variables.cbegin(), gen.variables.cend(),
                        [&](const Var& var) { return var.name == stmt->token_ident.value.value(); });
            
            if(it == gen.variables.cend())
            {
                std::cerr << "Undeclared variable : '" << stmt->token_ident.value.value() << "'!" << std::endl;
                exit(1);
            }
            gen.gen_expr(stmt->expr);
            gen.pop("rax");
            gen.output_text << "    mov [rsp + " << (gen.stack_size - it->stack_pos - 1) * 8 << "], rax\n";
        }

        const void operator()(const NodeScope* scope) {
            gen.gen_scope(scope);
        }

        const void operator()(const NodeStmtIf* if_stmt)
        {
            gen.gen_int_expr(if_stmt->int_expr_1);
            gen.gen_int_expr(if_stmt->int_expr_2);
            gen.pop("rbx");
            gen.pop("rax");
            std::string label = gen.create_label();

            switch (if_stmt->comp_simb->token_simb.type)
            {
                case TokenType::MAJOR:
                    gen.output_text << "    cmp rax, rbx\n";
                    gen.output_text << "    jle " << label << "\n"; // jump if less or equal
                    break;
                case TokenType::MINOR:
                    gen.output_text << "    cmp rax, rbx\n";
                    gen.output_text << "    jge " << label << "\n"; // jump if greater or equal
                    break;
                case TokenType::DBL_EQUAL:
                    gen.output_text << "    cmp rax, rbx\n";
                    gen.output_text << "    jne " << label << "\n"; // jump if greater or equal
                    break;
                case TokenType::NOT_EQUAL:
                    gen.output_text << "    cmp rax, rbx\n";
                    gen.output_text << "    je " << label << "\n"; // jump if greater or equal
                    break;

                default:
                    break;
            }
            
            gen.gen_scope(if_stmt->scope);
            std::string end_label = gen.create_label();
            gen.output_text << "    jmp " << end_label << "\n";
            gen.output_text << label << ":\n";
            if(if_stmt->pred.has_value()){
                gen.gen_if_pred(if_stmt->pred.value(), end_label);
                gen.output_text <<  end_label << ":\n";
            }
        }
    };

    StmtVisitor visitor{ .gen = *this };
    std::visit(visitor, stmt->value);
}