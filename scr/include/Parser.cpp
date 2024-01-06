#include "Parser.h"

std::optional<NodeTerm*> Parser::parse_term()
{
    if(peek().has_value() && peek().value().type == TokenType::INTEGER)
    {
        auto node_term_int = allocator.emplace<NodeTermInt>(consume());
        auto node_term = allocator.emplace<NodeTerm>(node_term_int);
        return node_term;
    }

    else if(peek().has_value() && peek().value().type == TokenType::IDENT)
    {
        auto node_term_ident = allocator.emplace<NodeTermIdent>(consume());
        auto node_term = allocator.emplace<NodeTerm>(node_term_ident);
        return node_term;
    }

    else if(auto token_open_paren = try_consume(TokenType::OPEN_PAREN))
    {
        if(auto node_expr = parse_expr())
        {
            if(try_consume(TokenType::CLOSE_PAREN, ")", token_open_paren.value().line))
            {
                auto node_term_paren = allocator.emplace<NodeTermParen>(node_expr.value());
                auto node_term = allocator.emplace<NodeTerm>(node_term_paren);
                return node_term;
            }

        }else{
            error_expected("expression", token_open_paren.value().line);
        }
    }

    return {};
}

// ----------------------------------------

std::optional<NodeExpr*> Parser::parse_expr(int min_prec = 0)
{
    std::optional<NodeTerm*> term_lhs = parse_term();
    if(!term_lhs.has_value()){
        return {};
    }
    
    auto expr_lhs = allocator.emplace<NodeExpr>(term_lhs.value());

    while(true)
    {
        std::optional<Token> curr_tok = peek();
        std::optional<int> prec;
        if(curr_tok.has_value())
        {
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
            std::cerr << "Unable to parse expression!" << std::endl;
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

    if(auto token_open_curly = try_consume(TokenType::OPEN_CURLY))
    {
        while (auto node_stmt = parse_stmt())
        {
            scope->stmts.push_back(node_stmt.value());
        }

        try_consume(TokenType::CLOSE_CURLY, "}", token_open_curly.value().line);

        return scope;
    }

    return {};
}

// ----------------------------------------

std::optional<NodeIfPred*> Parser::parse_if_pred()
{
    if(auto token_ifpred_elif = try_consume(TokenType::ELIF))
    {
        try_consume(TokenType::OPEN_PAREN, "(", token_ifpred_elif.value().line);

        auto node_ifpred_elif = allocator.alloc<NodeIfPredElif>();
        if(auto expr = parse_expr()){
            node_ifpred_elif->cond = expr.value();
        }else{
            error_expected("expression", token_ifpred_elif.value().line);
        }

        try_consume(TokenType::CLOSE_PAREN, ")", token_ifpred_elif.value().line);

        if(auto scope = parse_scope()){
            node_ifpred_elif->scope = scope.value();
        }else{
            error_invalid("scope", token_ifpred_elif.value().line);
        }

        node_ifpred_elif->pred = parse_if_pred();
        auto node_ifpred = allocator.emplace<NodeIfPred>(node_ifpred_elif);
        return node_ifpred;
    }
    else if(auto token_ifpred_else = try_consume(TokenType::ELSE))
    {
        if(auto node_scope = parse_scope()){
            auto node_ifpred_else = allocator.emplace<NodeIfPredElse>(node_scope.value());
            auto node_ifpred = allocator.emplace<NodeIfPred>(node_ifpred_else);
            return node_ifpred;

        }else{
            error_invalid("scope", token_ifpred_else.value().line);
        }
    }

    return {};
}

// ----------------------------------------

std::optional<NodeStmt*> Parser::parse_stmt()
{
    if(auto token_stmt_exit = try_consume(TokenType::EXIT))
    {
        try_consume(TokenType::OPEN_PAREN, "(", token_stmt_exit.value().line);

        if(auto node_expr = parse_expr())
        {
            auto node_stmt_exit = allocator.emplace<NodeStmtExit>(node_expr.value());

            try_consume(TokenType::CLOSE_PAREN, ")", token_stmt_exit.value().line);
            try_consume(TokenType::SEMI, ";", token_stmt_exit.value().line);

            auto node_stmt = allocator.emplace<NodeStmt>(node_stmt_exit);
            return node_stmt;
        }
        else{
            error_invalid("expression", token_stmt_exit.value().line);
        }
    }

    else if(auto token_stmt_let = try_consume(TokenType::LET))
    {
        if(peek().has_value() && peek().value().type == TokenType::IDENT)
        {
            if(peek(1).has_value() && peek(1).value().type == TokenType::EQUAL)
            {
                auto node_stmt_let = allocator.alloc<NodeStmtLet>();
                node_stmt_let->ident = consume();
                consume();

                if(auto node_expr = parse_expr()){
                    node_stmt_let->expr = node_expr.value();

                    try_consume(TokenType::SEMI, ";", token_stmt_let.value().line);

                    auto stmt = allocator.emplace<NodeStmt>(node_stmt_let);
                    return stmt;
                }
                else{
                    error_invalid("expression", token_stmt_let.value().line);
                }                
            }
        }

        return {};
    }
    else if(peek().has_value() && peek().value().type == TokenType::IDENT)
    {
        if(peek(1).has_value() && peek(1).value().type == TokenType::EQUAL)
        {
            auto node_stmt_assign = allocator.alloc<NodeStmtAssign>();
            node_stmt_assign->ident = consume();
            consume();

            if(auto node_expr = parse_expr()){
                node_stmt_assign->expr = node_expr.value();

                try_consume(TokenType::SEMI, ";", node_stmt_assign->ident.line);

                auto node_stmt = allocator.emplace<NodeStmt>(node_stmt_assign);
                return node_stmt;
            }
            else{
                std::cerr << "Invalid expression!" << std::endl;
                exit(1);
            }
        }
    }

    else if(auto token_stmt_if = try_consume(TokenType::IF))
    {
        try_consume(TokenType::OPEN_PAREN, "(", token_stmt_if.value().line);

        auto node_stmt_if = allocator.alloc<NodeStmtIf>();
        if(auto expr = parse_expr()){
            node_stmt_if->cond = expr.value();
        }else{
            error_expected("expression", token_stmt_if.value().line);
        }

        try_consume(TokenType::CLOSE_PAREN, ")", token_stmt_if.value().line);

        if(auto scope = parse_scope()){
            node_stmt_if->scope = scope.value();
            node_stmt_if->pred = parse_if_pred();

            auto node_stmt = allocator.emplace<NodeStmt>(node_stmt_if);
            return node_stmt;
        }else{
            error_invalid("scope", token_stmt_if.value().line);
        }
    }

    else if(peek().has_value() && peek().value().type == TokenType::OPEN_CURLY)
    {
        if(auto scope = parse_scope()){
            auto node_stmt = allocator.emplace<NodeStmt>(scope.value());
            return node_stmt;
        }else{
            error_invalid("scope");
        }
    }
    
    return {};
}