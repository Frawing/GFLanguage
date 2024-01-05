$$
\begin{align}

    [Prog] &\to [Stmt]^*

    \\

    [Scope] &\to {[Stmt]^*}
    
    \\
    
    [Stmt] &\to
    \begin{cases}

        exit( [Expr] );
        \\
        let \space ident = [Expr];
        \\
        [Scope]
        \\
        if ([Expr]) [Scope]
    
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
        [Expr] \space / \space [Expr] &\text{prec} = 1
        \\
        [Expr] + [Expr] &\text{prec} = 0
        \\
        [Expr] - [Expr] &\text{prec} = 0
    
    \end{cases}
    
    \\
    
    [Term] &\to
    \begin{cases}

        Integer
        \\
        Identifier
        \\
        ( [Expr] )

    \end{cases}

\end{align}
$$