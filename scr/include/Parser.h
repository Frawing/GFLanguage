#pragma once

#include <variant>
#include "Arena.h"
#include "Token.h"

struct NodeExpr;
struct NodeStmt;

struct NodeTermInt{
    Token integer;
};

struct NodeTermIdent{
    Token indent;
};

struct NodeTermParen{
    NodeExpr* expr;
};

struct NodeTerm{
    std::variant<NodeTermInt*,
                 NodeTermIdent*,
                 NodeTermParen*> value;
};

struct NodeBinExprMul{
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprDiv{
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprAdd{
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExprSub{
    NodeExpr* lhs;
    NodeExpr* rhs;
};

struct NodeBinExpr{
    std::variant<NodeBinExprMul*, NodeBinExprDiv*,
                 NodeBinExprAdd*, NodeBinExprSub*> value;
};

struct NodeExpr{
    std::variant<NodeTerm*,
                 NodeBinExpr*> value;
};

struct NodeScope{
    std::vector<NodeStmt*> stmts;
};

struct NodeStmtExit{
    NodeExpr* expr;
};

struct NodeStmtLet{
    Token ident;
    NodeExpr* expr;
};

struct NodeStmtIf
{
    NodeExpr* cond;
    NodeScope* scope;
};


struct NodeStmt{
    std::variant<NodeStmtLet*,
                 NodeStmtExit*,
                 NodeStmtIf*, NodeScope*> stmt;
};

struct NodeProg{
    std::vector<NodeStmt*> stmts;
};

class Parser
{

    public:

        explicit inline Parser(const std::vector<Token> p_tokens) 
            : tokens(p_tokens), allocator(1024 * 1024 * 4) {}

        inline std::optional<NodeTerm*>  parse_term();
        inline std::optional<NodeExpr*>  parse_expr(int min_prec = 0);
        inline std::optional<NodeScope*> parse_scope();
        inline std::optional<NodeStmt*>  parse_stmt();
        inline std::optional<NodeProg>   parse_program()
        {
            NodeProg prog;
            while(peek().has_value())
            {
                if(auto stmt = parse_stmt()){
                    prog.stmts.push_back(stmt.value());
                }else{
                    std::cerr << "Invalid Statment" << std::endl;
                    exit(1);
                }
            }
            return prog;
        }

    private:
        
        const inline std::optional<Token> peek(int offset = 0)
        {
            if((index + offset) >= tokens.size()){
                return {};
            }else{
                return tokens.at(index + offset);
            }
        }
        const inline Token consume(){
            return tokens.at(index++);
        }

        const inline std::optional<Token> try_consume(const TokenType type)
        {
            if (peek().has_value() && peek().value().type == type){
                return consume();
            }
            return {};
        }

        const std::vector<Token> tokens;
        size_t index = 0;
        ArenaAllocator allocator;

};