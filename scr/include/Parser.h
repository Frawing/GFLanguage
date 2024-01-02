#pragma once

#include <variant>
#include "Token.h"

struct NodeExprInt{
    Token integer;
};

struct NodeExprIdent{
    Token indent;
};

struct NodeExpr{
    std::variant<NodeExprInt,
                 NodeExprIdent> value;
};

struct NodeStmtExit{
    NodeExpr expr;
};

struct NodeStmtLet{
    Token ident;
    NodeExpr expr;
};

struct NodeStmt{
    std::variant<NodeStmtExit,
                 NodeStmtLet> stmt;
};

struct NodeProg{
    std::vector<NodeStmt> stmts;
};

class Parser
{
    
    public:
        
        inline explicit Parser(const std::vector<Token> p_tokens) : tokens(p_tokens) {}

        inline std::optional<NodeExpr> parse_expr(){
            if(peek().has_value() && peek().value().type == TokenType::INTEGER){
                return NodeExpr{.value = NodeExprInt{.integer = consume()}};
            }
            else if(peek().has_value() && peek().value().type == TokenType::IDENT){
                return NodeExpr{.value = NodeExprIdent{.indent = consume()}};
            }
            else{
                return {};
            }
        }

        inline std::optional<NodeStmt> parse_stmt(Error errorHandler)
        {
            if(peek().has_value() && peek().value().type == TokenType::EXIT)
            {
                consume();

                NodeStmtExit stmt;

                if(peek().has_value() && peek().value().type == TokenType::OPEN_PAREN){
                    consume();
                }else{
                    errorHandler.value = "(";
                    errorHandler.print_error(ErrorType::MISSED);
                    exit(1);
                }

                if(auto node_expr = parse_expr()){
                    stmt = NodeStmtExit{.expr = node_expr.value()};
                }else{
                    std::cerr << "The parameter must be an integer!" << std::endl;
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

                return NodeStmt{.stmt = stmt};
            }
            else if(peek().has_value() && peek().value().type == TokenType::LET)
            {
                consume();

                NodeStmtLet stmt;

                if(peek().has_value() && peek().value().type == TokenType::IDENT){
                    if(peek(1).has_value() && peek(1).value().type == TokenType::EQUAL){
                        stmt = NodeStmtLet{.ident = consume()};
                        consume();
                        if(auto node_expr = parse_expr()){
                            stmt.expr = node_expr.value();
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

                return NodeStmt{.stmt = stmt};
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

};