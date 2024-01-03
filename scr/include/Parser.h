#pragma once

#include <variant>
#include "Token.h"

struct NodeExpr;

struct NodeTermInt{
    Token integer;
};

struct NodeTermIdent{
    Token indent;
};

struct NodeTerm{
    std::variant<NodeTermInt*,
                 NodeTermIdent*> value;
};

struct NodeBinExprAdd{
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExpr{
    NodeBinExprAdd* value;
};

struct NodeExpr{
    std::variant<NodeTerm*,
                 NodeBinExpr*> value;
};

struct NodeStmtExit{
    NodeExpr* expr;
};

struct NodeStmtLet{
    Token ident;
    NodeExpr* expr;
};

struct NodeStmt{
    std::variant<NodeStmtExit*,
                 NodeStmtLet*> stmt;
};

struct NodeProg{
    std::vector<NodeStmt*> stmts;
};

class Parser
{

    public:

        inline explicit Parser(const std::vector<Token> p_tokens) 
            : tokens(p_tokens), allocator(1024 * 1024 * 4) 
        {

        }

        inline std::optional<NodeTerm*> parse_term(){
            if(peek().has_value() && peek().value().type == TokenType::INTEGER){
                auto node_term = allocator.alloc<NodeTerm>();
                auto node_term_int = allocator.alloc<NodeTermInt>();
                node_term_int->integer = consume();
                node_term->value = node_term_int;
                return node_term;
            }
            else if(peek().has_value() && peek().value().type == TokenType::IDENT){
                auto node_term = allocator.alloc<NodeTerm>();
                auto node_term_ident = allocator.alloc<NodeTermIdent>();
                node_term_ident->indent = consume();
                node_term->value = node_term_ident;
                return node_term;
            }
            else{
                return {};
            }
        }

        inline std::optional<NodeExpr*> parse_expr(){
            if(auto term = parse_term()){
                if(peek().has_value() && peek().value().type == TokenType::PLUS){
                    auto bin_expr_add = allocator.alloc<NodeBinExprAdd>();
                    auto lhs_expr = allocator.alloc<NodeExpr>();
                    lhs_expr->value = term.value();
                    bin_expr_add->lhs = lhs_expr;
                    consume();
                    if(auto rhs = parse_expr()){
                        bin_expr_add->rhs = rhs.value();
                        auto expr = allocator.alloc<NodeExpr>();
                        auto bin_expr = allocator.alloc<NodeBinExpr>();
                        bin_expr->value = bin_expr_add;
                        expr->value = bin_expr;
                        return expr;
                    }else{
                        std::cerr << "Expected expression!" << std::endl;
                        exit(1);
                    }
                }
                else{
                    auto expr = allocator.alloc<NodeExpr>();
                    expr->value = term.value();
                    return expr;
                }
            }
            else{
                return {};
            }
        }

        inline std::optional<NodeStmt*> parse_stmt(Error errorHandler)
        {
            auto stmt = allocator.alloc<NodeStmt>();

            if(peek().has_value() && peek().value().type == TokenType::EXIT)
            {
                consume();

                if(peek().has_value() && peek().value().type == TokenType::OPEN_PAREN){
                    consume();
                }else{
                    errorHandler.value = "(";
                    errorHandler.print_error(ErrorType::MISSED);
                    exit(1);
                }

                if(auto node_expr = parse_expr()){
                    auto stmt_exit = allocator.alloc<NodeStmtExit>();
                    stmt_exit->expr = node_expr.value();
                    stmt->stmt = stmt_exit;
                }else{
                    std::cerr << "Unexpected value!" << std::endl;
                    exit(1);
                }

                if(peek().has_value() && peek().value().type == TokenType::CLOSE_PAREN){
                    consume();
                }else{
                    errorHandler.value = "";
                    errorHandler.print_error(ErrorType::INVALID_EXPRESSION);
                    exit(1);
                }

                if(peek().has_value() && peek().value().type == TokenType::SEMI){
                    consume();
                }else{
                    errorHandler.value = ";";
                    errorHandler.print_error(ErrorType::MISSED);
                    exit(1);
                }

                std::cout << "Trovato exit statment" << std::endl; // ?
                return stmt;
            }

            else if(peek().has_value() && peek().value().type == TokenType::LET)
            {
                consume();

                if(peek().has_value() && peek().value().type == TokenType::IDENT){
                    if(peek(1).has_value() && peek(1).value().type == TokenType::EQUAL){
                        auto stmt_let = allocator.alloc<NodeStmtLet>();
                        stmt_let->ident = consume();
                        consume();
                        if(auto node_expr = parse_expr()){
                            stmt_let->expr = node_expr.value();
                            stmt->stmt = stmt_let;
                        }else{
                            errorHandler.value = "";
                            errorHandler.print_error(ErrorType::INVALID_EXPRESSION);
                            exit(1);
                        }
                    }
                }

                if(peek().has_value() && peek().value().type == TokenType::SEMI){
                    consume();
                }else{
                    errorHandler.value = ";";
                    errorHandler.print_error(ErrorType::MISSED);
                    exit(1);
                }

                return stmt;
            }else{
                return {};
            }
        }

        inline std::optional<NodeProg> parse_program(Error errorHandler)
        {
            NodeProg prog;
            while(peek().has_value())
            {
                if(auto stmt = parse_stmt(errorHandler)){
                    prog.stmts.push_back(stmt.value());
                }else{
                    errorHandler.value = "";
                    errorHandler.print_error(ErrorType::INVALID_EXPRESSION);
                    exit(1);
                }
            }
            return prog;
        }

    private:
        
        inline std::optional<Token> peek(int offset = 0) const {
            if((index + offset) >= tokens.size()){
                return {};
            }else{
                return tokens.at(index + offset);
            }
        }
        inline Token consume(){
            return tokens.at(index++);
        }

        const std::vector<Token> tokens;
        size_t index = 0;
        ArenaAllocator allocator;

};