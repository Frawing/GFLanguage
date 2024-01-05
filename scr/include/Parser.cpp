#include "Parser.h"

std::optional<NodeTerm*> Parser::parse_term()
{
    if(peek().has_value() && peek().value().type == TokenType::INTEGER)
    {
        auto node_term = allocator.alloc<NodeTerm>();
        auto node_term_int = allocator.alloc<NodeTermInt>();
        node_term_int->integer = consume();
        node_term->value = node_term_int;
        return node_term;
    }

    else if(peek().has_value() && peek().value().type == TokenType::IDENT)
    {
        auto node_term = allocator.alloc<NodeTerm>();
        auto node_term_ident = allocator.alloc<NodeTermIdent>();
        node_term_ident->indent = consume();
        node_term->value = node_term_ident;
        return node_term;
    }

    else if(try_consume(TokenType::OPEN_PAREN))
    {
        if(auto expr = parse_expr())
        {
            if(try_consume(TokenType::CLOSE_PAREN))
            {
                auto node_term_paren = allocator.alloc<NodeTermParen>();
                node_term_paren->expr = expr.value();
                auto node_term = allocator.alloc<NodeTerm>();
                node_term->value = node_term_paren;
                return node_term;
            }else{
                std::cerr << "Expected ')'" << std::endl;
                exit(1);
            }
        }else{
            std::cout << "Expected expression!" << std::endl;
            exit(1);
        }
    }

    else{
        return {};
    }
}

// ----------------------------------------

std::optional<NodeExpr*> Parser::parse_expr(int min_prec = 0)
{
    std::optional<NodeTerm*> term_lhs = parse_term();
    if(!term_lhs.has_value()){
        return {};
    }
    auto expr_lhs = allocator.alloc<NodeExpr>();
    expr_lhs->value = term_lhs.value();

    while(true)
    {
        std::optional<Token> curr_tok = peek();
        std::optional<int> prec;
        if(curr_tok.has_value()){
            prec = bin_prec(curr_tok.value().type);
            if(!prec.has_value() || prec < min_prec){
                break;
            }
        }else{
            break;
        }

        Token op = consume();
        int next_min_prec = prec.value() + 1;
        auto expr_rhs = parse_expr(next_min_prec);
        if(!expr_rhs.has_value()){
            std::cerr << "Unable to parse expression" << std::endl;
            exit(1);
        }

        auto bin_expr = allocator.alloc<NodeBinExpr>();
        auto new_expr_lhs = allocator.alloc<NodeExpr>();
        if(op.type == TokenType::PLUS)
        {
            auto bin_expr_add = allocator.alloc<NodeBinExprAdd>();
            new_expr_lhs->value = expr_lhs->value;
            bin_expr_add->lhs = new_expr_lhs;
            bin_expr_add->rhs = expr_rhs.value();
            bin_expr->value = bin_expr_add;
        }
        else if(op.type == TokenType::MINUS)
        {
            auto bin_expr_sub = allocator.alloc<NodeBinExprSub>();
            new_expr_lhs->value = expr_lhs->value;
            bin_expr_sub->lhs = new_expr_lhs;
            bin_expr_sub->rhs = expr_rhs.value();
            bin_expr->value = bin_expr_sub;
        }
        else if(op.type == TokenType::STAR)
        {
            auto bin_expr_mul = allocator.alloc<NodeBinExprMul>();
            new_expr_lhs->value = expr_lhs->value;
            bin_expr_mul->lhs = new_expr_lhs;
            bin_expr_mul->rhs = expr_rhs.value();
            bin_expr->value = bin_expr_mul;
        }
        else if(op.type == TokenType::SLASH)
        {
            auto bin_expr_div = allocator.alloc<NodeBinExprDiv>();
            new_expr_lhs->value = expr_lhs->value;
            bin_expr_div->lhs = new_expr_lhs;
            bin_expr_div->rhs = expr_rhs.value();
            bin_expr->value = bin_expr_div;
        }
        expr_lhs->value = bin_expr;
    }
    return expr_lhs;
}

// ----------------------------------------

std::optional<NodeScope*> Parser::parse_scope()
{
    auto scope = allocator.alloc<NodeScope>();
    
    if(!try_consume(TokenType::OPEN_CURLY)){
        return {};
    }

    while (auto stmt = parse_stmt())
    {
        scope->stmts.push_back(stmt.value());
    }

    if(!try_consume(TokenType::CLOSE_CURLY)){
        std::cerr << "Expected '}" << std::endl;
        exit(1);
    }

    return scope;
}

// ----------------------------------------

std::optional<NodeStmt*> Parser::parse_stmt()
{
    auto stmt = allocator.alloc<NodeStmt>();

    if(try_consume(TokenType::EXIT))
    {

        if(!try_consume(TokenType::OPEN_PAREN)){
            std::cerr << "Expected '('" << std::endl;
            exit(1);
        }

        if(auto node_expr = parse_expr())
        {
            auto stmt_exit = allocator.alloc<NodeStmtExit>();
            stmt_exit->expr = node_expr.value();
            stmt->stmt = stmt_exit;
        }
        else{
            std::cerr << "Unexpected value!" << std::endl;
            exit(1);
        }


        if(!try_consume(TokenType::CLOSE_PAREN)){
            std::cerr << "Expected ')'" << std::endl;
            exit(1);
        }

        if(!try_consume(TokenType::SEMI)){
            std::cerr << "Expected ';'" << std::endl;
            exit(1);
        }

        return stmt;
    }

    else if(try_consume(TokenType::LET))
    {

        if(peek().has_value() && peek().value().type == TokenType::IDENT)
        {
            if(peek(1).has_value() && peek(1).value().type == TokenType::EQUAL)
            {
                auto stmt_let = allocator.alloc<NodeStmtLet>();
                stmt_let->ident = consume();
                consume();

                if(auto node_expr = parse_expr()){
                    stmt_let->expr = node_expr.value();
                    stmt->stmt = stmt_let;
                }else{
                    std::cerr << "Invalid expression!" << std::endl;
                    exit(1);
                }
            }
        }

        if(!try_consume(TokenType::SEMI)){
            std::cerr << "Expected ';'" << std::endl;
            exit(1);
        }

        return stmt;
    }

    else if(try_consume(TokenType::IF))
    {
        if(!try_consume(TokenType::OPEN_PAREN)){
            std::cerr << "Expected '('" << std::endl;
            exit(1);
        }

        auto stmt = allocator.alloc<NodeStmt>();
        auto if_stmt = allocator.alloc<NodeStmtIf>();
        if(auto expr = parse_expr()){
            if_stmt->cond = expr.value();
        }else{
            std::cerr << "Expected expression!" << std::endl;
            exit(1);
        }

        if(!try_consume(TokenType::CLOSE_PAREN)){
            std::cerr << "Expected ')'" << std::endl;
            exit(1);
        }

        if(auto scope = parse_scope()){
            if_stmt->scope = scope.value();
        }else{
            std::cerr << "Invalid scope!" << std::endl;
            exit(1);
        }

        stmt->stmt = if_stmt;
        return stmt;
    }

    else if(peek().has_value() && peek().value().type == TokenType::OPEN_CURLY)
    {
        auto stmt = allocator.alloc<NodeStmt>();
        
        if(auto scope = parse_scope()){
            stmt->stmt = scope.value();
        }

        return stmt;
    }
    
    else{
        return {};
    }
}