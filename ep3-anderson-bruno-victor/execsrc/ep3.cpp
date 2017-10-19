#include "ep_base.hpp"
#include "typedef.hpp"
#include "util.hpp"

#include <stdio.h>

#include <iostream>
#include <string>

#include <vector>

// Create options
void initialize (EP &ep)
{
}


// Main function
int main ()
{
    // Stores line
    std::string line;

    // Base EP class
    EP ep; 

    // Initialize
    initialize (ep);

    // While not exited
    while (true)
    {
        // Read line
        std::cout << "[ep3]: ";
        std::getline(std::cin, line);

        // Split command
        std::vector<std::string> c_parts = split_spaces(line);

        if (!c_parts.size())
            continue;

        // Switch command
        if (c_parts[0] == "carrega" || c_parts[0] == "load")
        {
            if (c_parts.size() != 2)
                std::cout << "Carrega um arquivo de entrada\n" <<
                    "Usage: carrega <arquivo>\n";
            else
                ep.load_file(c_parts[1]);
        }
        else if (c_parts[0] == "espaco" || c_parts[0] == "free_space")
        {
            if (c_parts.size() != 2)
                std::cout <<
                    "Seleciona o algoritmo de gerenciamento de espaço livre\n"
                    << "Usage: espaco <num>\n";
            else
                ep.select_free_space_manager(c_parts[1]);
        }
        else if (c_parts[0] == "substitui" || c_parts[0] == "replace")
        {
            if (c_parts.size() != 2)
                std::cout <<
                    "Seleciona o algoritmo de substituição de páginas\n"
                    << "Usage: substitui <num>\n";
            else
                ep.select_free_space_manager(c_parts[1]);
        }
        else if (c_parts[0] == "executa" || c_parts[0] == "run")
        {
            if (c_parts.size() != 2)
                std::cout <<
                    "Executa o simulador\n"
                    << "Usage: executa <intervalo>\n";
            else
                ep.select_free_space_manager(c_parts[1]);
        }
        else if (c_parts[0] == "ajuda" || c_parts[0] == "help")
            std::cout << "Comandos disponíveis:\n" <<
                "carrega   (load)\n" <<
                "espaco    (free_space)\n" <<
                "substitui (replace)\n" <<
                "executa   (run)\n" <<
                "ajuda     (help)\n" <<
                "sai       (exit)\n" <<
                "extra     (extra)\n";
        else if (c_parts[0] == "extra")
        {
            globals::e = !globals::e;
            std::cout << "Extra prints are now " <<
                (globals::e ? "on" : "off") <<
                "\n";
        }
        else if (c_parts[0] == "sai" || c_parts[0] == "exit")
        {
            std::cout << "Saindo...\n";
            break;
        }
        else
            std::cout << "Comando '" << c_parts[0] << "' não reconhecido\n";
    }

    return 0;
}
