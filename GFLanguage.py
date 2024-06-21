from include.Generator import *
import time

def GFLanguage() -> None:
    # Init
    s : float = time.perf_counter()
    init_colorama(autoreset = True)

    # Get Args
    arg_parser = argparse.ArgumentParser(description = "GFLanguage Compiler")
    arg_parser.add_argument('-i',   "--input",              help = "Input file")
    arg_parser.add_argument('-o',   "--output",             help = "Output file")
    arg_parser.add_argument('-dt',  "--debug-tokens",        help = "Debug tokens",       action = "store_true")
    arg_parser.add_argument('-dn',  "--debug-nodes",         help = "Debug nodes",        action = "store_true")
    arg_parser.add_argument('-dct', "--debug-compile-time", help = "Debug compile time", action = "store_true")
    args = arg_parser.parse_args()

    # Assign Args
    input_file    : str  = args.input
    output_file   : str  = args.output
    debug_tokens  : bool = args.debug_tokens
    debug_nodes   : bool = args.debug_nodes
    debug_comp_tm : bool = args.debug_compile_time

    # Check Required Args
    if(input_file == None):
        print_error("N/A", "Input file not specified.", "Init Compilation", "Specify the input file using the '-i' argument.")
    
    # Check Errors / Get File Content
    try:
        file_content : str = open(input_file, "r").read()
    except FileNotFoundError:
        print_error("N/A", "Input file not found.", "Init Compilation", f"Make sure the file '{input_file}' exists and the path is correct.")

    # Lexer
    tokenizer : Tokenizer   = Tokenizer(file_content)
    tokens    : list[Token] = tokenizer.tokenize()
    
    if(debug_tokens):
        table = prettytable.PrettyTable(["Type", "Value", "Row", "Col"])
        for token in tokens:
            table.add_row([token.type.name, token.value, token.row, token.col])
        print(f"{Fore.CYAN}\n{table}\n")

    # Parser
    parser : Parser      = Parser(tokens)
    tree   : NodeProgram = parser.parse_program()

    if(debug_nodes):
        print(f"{Fore.CYAN}\n{tree}\n")

    # Generator
    try:
        generator : Generator = Generator(tree)
        open("program.asm", "w").write(generator.gen_program())
        # os.system("fasm program.asm")
        os.system("nasm program.asm -f elf64 -o program.o")
        os.system("ld -o PROGRAM program.o -dynamic-linker /lib64/ld-linux-x86-64.so.2 -L./raylib/ -lc -lraylib -lm")
        # os.remove("program.asm")
        os.remove("program.o")
    except Exception as e:
        print(f"{Fore.RED}\n{e}\n")
        exit(1)

    # Debug Compilation Time
    if(debug_comp_tm):
        e : float = time.perf_counter()
        print(f"{Fore.GREEN}\nDebug Compile Time : {(e - s):.10f}\n")
    exit(0)

if(__name__ == "__main__"):
    GFLanguage()
