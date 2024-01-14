$$
\begin{align}

    [Prog] &\to [Stmt]^*

    \\

    [Scope] &\to {[Stmt]^*}
    
    \\
    
    [Stmt] &\to
    \begin{cases}

        exit( [IntExpr] );
        \\
        println( [TextExpr] )
        \\
        print( [TextExpr] );
        \\
        let \space ident = [Expr];
        \\
        ident = [Expr];
        \\
        [Scope]
        \\
        if ( [IntExpr] [CompSimb] [IntExpr] ) [Scope] [IfPred]^*
    
    \end{cases}
    
    \\

    [IfPred] &\to
    \begin{cases}
    
        elif ( [IntExpr] [CombSimb] [IntExpr] ) [Scope] [IfPred]
        \\
        else [Scope]
        \\
        \epsilon

    \end{cases}

    \\
    
    [Expr] &\to
    \begin{cases}

        [IntExpr]
        \\
        [TextExpr]

    \end{cases}

    \\
    
    [IntExpr] &\to
    \begin{cases}

        [BinExpr]
        \\
        [Term]

    \end{cases}

    \\
    
    [TextExpr] &\to
    \begin{cases}

        String
        \\
        Ident

    \end{cases}
    
    \\
    
    [BinExpr] &\to
    \begin{cases}

        [IntExpr] * [IntExpr] &\text{prec} = 1
        \\
        [IntExpr] \space / \space [Expr] &\text{prec} = 1
        \\
        [IntExpr] + [IntExpr] &\text{prec} = 0
        \\
        [IntExpr] - [IntExpr] &\text{prec} = 0
    
    \end{cases}
    
    \\
    
    [Term] &\to
    \begin{cases}

        Integer
        \\
        Identifier
        \\
        ( [IntExpr] )

    \end{cases}

\end{align}
$$