$$
\begin{align}

    [Prog] &\to [Stmt]^*
    
    \\
    
    [Stmt] &\to
    \begin{cases}

        exit([Expr]);
        \\
        let ident = [Expr];
    
    \end{cases}
    
    \\
    
    [Expr] &\to
    \begin{cases}

        [BinExpr]
        \\
        [Term]

    \end{cases}
    
    \\
    
    [BinExpr] &\to
    \begin{cases}

        [Expr] * [Expr] &\text{prec} = 1
        \\
        [Expr] + [Expr] &\text{prec} = 0
    
    \end{cases}
    
    \\
    
    [Term] &\to
    \begin{cases}

        Integer
        \\
        Identifier

    \end{cases}

\end{align}
$$