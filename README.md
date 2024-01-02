<div align="center">

# GFLanguage

### Grammar

$$
\begin{align*}
    [Prog] &\to [Stmt]^* \\
    [Stmt] &\to
    \begin{cases}
        \text{exit}([Expr]); \\
        \text{let ident} = [Expr];
    \end{cases} \\
    [Expr] &\to
    \begin{cases}
        \text{Integer} \\
        \text{Identifier}
    \end{cases}
\end{align*}
$$

</div>
