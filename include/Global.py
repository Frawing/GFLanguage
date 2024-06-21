from __future__ import annotations

from colorama import Fore, Style, init as init_colorama
from enum import Enum
import os, sys, argparse
import prettytable

def print_error(line : str, col : str, message : str, context : str, suggestion : str) -> None:
    print(
        f"\n"
        f"{Fore.RED    }Error at Line {line}{f', {col}' if col != '' else ''}:\n"
        f"{Fore.YELLOW }- Context: {context}\n"
        f"{Fore.RED    }- {message}\n"
        f"{Fore.CYAN   }Suggestion: {suggestion}\n"
    )
    sys.exit(1)