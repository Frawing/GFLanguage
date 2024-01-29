# Documentation

## Commenti

```Ruby
#! Questo è un commento

&* Questo è un
   commento multilinea *&
```

## Variabili

```Javascript
let x = 0;
let string = "Stringa";

x = 1;
string = "Stringa modificata";

x = "Adesso sono una stringa";
string = x;
```

## Exit Function

```Javascript
#! Se ometti la funzione exit sarà automaticamente inserito un exit code 0

let exit_code_number = 0;
exit(exit_code_number);
exit(1);
```

## Print / PrintLn

```Javascript
let world = "world";

print("Hello ");
println(world);
```

## Input

```Javascript
print("Età: ");
input();

print("Nome: ");
input(nome);

print("Allora ti chiami ");
println(nome);

print("Mi ricordi quanti anni hai? ")
input(eta);

print("Quindi hai ");
println(eta)
```

## While

```Javascript

let x = 15;

while(x > 10)
{
   print("X è maggiore di 10");
   x -= 1;
}

```