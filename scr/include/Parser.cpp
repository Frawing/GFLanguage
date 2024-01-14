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
        if(auto node_int_expr = parse_int_expr())
        {
            if(try_consume(TokenType::CLOSE_PAREN, ")", token_open_paren.value().line))
            {
                auto node_term_paren = allocator.emplace<NodeTermParen>(node_int_expr.value());
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

std::optional<NodeExpr*> Parser::parse_expr()
{
    if(auto node_text_expr = parse_text_expr())
    {
        auto node_expr = allocator.emplace<NodeExpr>(node_text_expr.value());
        return node_expr;
    }
    else if(auto node_int_expr = parse_int_expr()){
        auto node_expr = allocator.emplace<NodeExpr>(node_int_expr.value());
        return node_expr;
    }

    return {};
}

// ----------------------------------------

std::optional<NodeTextExpr*> Parser::parse_text_expr()
{
    if(auto token_string = try_consume(TokenType::STRING))
    {
        auto node_text_string = allocator.emplace<NodeTextString>(token_string.value());

        auto node_text_expr = allocator.emplace<NodeTextExpr>(node_text_string);
        return node_text_expr;
    }
    else if(auto token_ident = try_consume(TokenType::IDENT))
    {
        auto node_text_ident = allocator.emplace<NodeTextIdent>(token_ident.value());
        auto node_text_expr = allocator.emplace<NodeTextExpr>(node_text_ident);
        return node_text_expr;
    }
    
    return {};
}

// ----------------------------------------

std::optional<NodeIntExpr*> Parser::parse_int_expr(int min_prec = 0)
{
    std::optional<NodeTerm*> term_lhs = parse_term();
    if(!term_lhs.has_value()){
        return {};
    }
    
    auto expr_lhs = allocator.emplace<NodeIntExpr>(term_lhs.value());

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
        auto expr_rhs = parse_int_expr(next_min_prec);
        if(!expr_rhs.has_value()){
            std::cerr << "Unable to parse expression!" << std::endl;
            exit(1);
        }

        auto bin_expr = allocator.alloc<NodeBinExpr>();
        auto new_expr_lhs = allocator.alloc<NodeIntExpr>();
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
        if(auto node_int_expr = parse_int_expr()){
            node_ifpred_elif->int_expr_1 = node_int_expr.value();
        }else{
            error_expected("expression", token_ifpred_elif.value().line);
        }

        if(peek().has_value()){
            if(peek().value().type != TokenType::MAJOR && peek().value().type != TokenType::MINOR &&
               peek().value().type != TokenType::DBL_EQUAL && peek().value().type != TokenType::NOT_EQUAL)
            {
                error_expected("compare simbol", token_ifpred_elif.value().line);
            }
            auto node_comb_simb = allocator.emplace<NodeCompSimb>(consume());
            node_ifpred_elif->comp_simb = node_comb_simb;
        }

        if(auto node_int_expr = parse_int_expr()){
            node_ifpred_elif->int_expr_2 = node_int_expr.value();
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

        if(auto node_int_expr = parse_int_expr())
        {
            auto node_stmt_exit = allocator.emplace<NodeStmtExit>(node_int_expr.value());

            try_consume(TokenType::CLOSE_PAREN, ")", token_stmt_exit.value().line);
            try_consume(TokenType::SEMI, ";", token_stmt_exit.value().line);

            auto node_stmt = allocator.emplace<NodeStmt>(node_stmt_exit);
            return node_stmt;
        }
        else{
            error_invalid("expression", token_stmt_exit.value().line);
        }
    }

    else if(auto token_stmt_print = try_consume(TokenType::PRINT))
    {
        try_consume(TokenType::OPEN_PAREN, "(", token_stmt_print.value().line);

        if(auto node_text_expr = parse_text_expr()){
            auto node_stmt_print = allocator.alloc<NodeStmtPrint>();
            node_stmt_print->new_line = false;
            node_stmt_print->text_expr = node_text_expr.value();

            try_consume(TokenType::CLOSE_PAREN, ")", token_stmt_print.value().line);
            try_consume(TokenType::SEMI, ";", token_stmt_print.value().line);

            auto node_stmt = allocator.emplace<NodeStmt>(node_stmt_print);
            return node_stmt;
        }
        else{
            error_invalid("expression", token_stmt_print.value().line);
        }
    }
    else if(auto token_stmt_println = try_consume(TokenType::PRINTLN))
    {
        try_consume(TokenType::OPEN_PAREN, "(", token_stmt_println.value().line);

        if(auto node_text_expr = parse_text_expr()){
            auto node_stmt_println = allocator.alloc<NodeStmtPrint>();
            node_stmt_println->new_line = true;
            node_stmt_println->text_expr = node_text_expr.value();

            try_consume(TokenType::CLOSE_PAREN, ")", token_stmt_println.value().line);
            try_consume(TokenType::SEMI, ";", token_stmt_println.value().line);

            auto node_stmt = allocator.emplace<NodeStmt>(node_stmt_println);
            return node_stmt;
        }
        else{
            error_invalid("expression", token_stmt_print.value().line);
        }
    }

    else if(auto token_stmt_let = try_consume(TokenType::LET))
    {
        if(peek().has_value() && peek().value().type == TokenType::IDENT)
        {
            if(peek(1).has_value() && peek(1).value().type == TokenType::EQUAL)
            {
                auto node_stmt_let = allocator.alloc<NodeStmtLet>();
                node_stmt_let->token_ident = consume();
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
            node_stmt_assign->token_ident = consume();
            consume();

            if(auto node_expr = parse_expr()){
                node_stmt_assign->expr = node_expr.value();

                try_consume(TokenType::SEMI, ";", node_stmt_assign->token_ident.line);

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
        if(auto node_int_expr = parse_int_expr()){
            node_stmt_if->int_expr_1 = node_int_expr.value();
        }else{
            error_expected("expression", token_stmt_if.value().line);
        }

        if(peek().has_value()){
            if(peek().value().type != TokenType::MAJOR && peek().value().type != TokenType::MINOR &&
               peek().value().type != TokenType::DBL_EQUAL && peek().value().type != TokenType::NOT_EQUAL)
            {
                error_expected("compare simbol", token_stmt_if.value().line);
            }
            auto node_comb_simb = allocator.emplace<NodeCompSimb>(consume());
            node_stmt_if->comp_simb = node_comb_simb;
        }

        if(auto node_int_expr = parse_int_expr()){
            node_stmt_if->int_expr_2 = node_int_expr.value();
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