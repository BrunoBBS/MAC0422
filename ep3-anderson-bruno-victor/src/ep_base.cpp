#include "ep_base.hpp"
#include "util.hpp"
#include "memory.hpp"

#include <stdexcept>

// Construtor
EP::EP (std::string p_mem_filename,
        std::string v_mem_filename) :
    p_mem_filename(p_mem_filename), v_mem_filename(v_mem_filename),
    free_space_manager(1), page_replace_manager(1)
{
}

// Loads input file
void EP::load_file (std::string filename)
{
    int tot_mem;
    int vir_mem;
    int alloc_s;
    int pages_s;

    std::ifstream input_file (filename, std::ifstream::in);
    if (!input_file.good())
    {
        std::cerr << "Erro ao abrir o arquivo '" << filename << "'\n";
        return;
    }

    // Reads first line in file
    std::string line;
    std::getline(input_file, line);

    bool valid_line = true;
    std::vector<std::string> parts = split_spaces(line);

    if (valid_line && parts.size() != 4)
        valid_line = false;
    
    if (valid_line)
    {
        try
        {
            tot_mem = std::stoi(parts[0]);
            vir_mem = std::stoi(parts[1]);
            alloc_s = std::stoi(parts[2]);
            pages_s = std::stoi(parts[3]);
        }
        catch (std::invalid_argument e)
        {
            valid_line = false;
        }
    }

    if (!valid_line)
    {
        std::cerr << "Primeira linha '" << line << "' é inválida\n";
        return;
    }

    // Reads remaining lines in file
    int line_n = 1;
    std::vector<int> comp_events;
    std::vector<Process> processes;

    while (std::getline(input_file, line))
    {
        line_n++;

        valid_line = true;
        parts = split_spaces(line);

        if (!parts.size())
            continue;

        // Parse "t COMPACTAR"
        if (parts.size() == 2)
        {
            int time;
            try
            {
                time = std::stoi(parts[0]);
            }
            catch (std::invalid_argument e)
            {
                valid_line = false;
            }

            if (parts[1] != "COMPACTAR")
                valid_line = false;

            if (valid_line)
                comp_events.push_back(time);
        }

        // Parse process
        else if (parts.size() > 3 && !(parts.size() % 2))
        {
            int t0, tf, b;
            std::string name;

            try
            {
                t0 = std::stoi(parts[0]);
                tf = std::stoi(parts[1]);
                b  = std::stoi(parts[2]);
            }
            catch (std::invalid_argument e)
            {
                valid_line = false;
            }

            if (valid_line)
                name = parts[3];

            std::vector<MemoryAccess> m_accesses;

            for (unsigned int i = 4;
                    valid_line && i < parts.size() - 1; i += 2)
            {
                MemoryAccess m_access;
                try
                {
                    m_access.position = std::stoi(parts[i]);
                    m_access.time = std::stoi(parts[i + 1]);
                }
                catch (std::invalid_argument e)
                {
                    valid_line = false;
                }
                m_accesses.push_back(m_access);
            }

            if (valid_line)
                processes.push_back(Process(t0, tf, b, name, m_accesses));
        }
        else
        {
            valid_line = false;
        }

        if (!valid_line)
        {
            std::cerr << "Linha " << line_n <<
                " '" << line << "' é inválida\n";
            return;
        }
    }

    input_file.close();

    memory = std::shared_ptr<Memory> (
            new Memory(tot_mem, vir_mem,
                p_mem_filename, v_mem_filename));

    if (!memory->good())
    {
        memory.reset();
        std::cerr << "Não foi possível acessar os arquivos '" <<
           p_mem_filename << "' e/ou '" << v_mem_filename << "'\n";
        return;
    }

    std::cout << "Arquivo '" << filename << "' carregado com sucesso\n";

    if (globals::e)
    {
        std::cout <<
            processes.size() << " processos carregados\n";
        std::cout <<
            comp_events.size() << " eventos de compactação carregados\n";
    }

    // Now we loaded a file
    file_loaded = true;

    // Saves input structures
    phys_mem = tot_mem;
    virt_mem = vir_mem;
    alloc_size = alloc_s;
    page_size = pages_s;
    compress_evn = comp_events;
    process_list = processes;
}

// Selects free space manager
void EP::select_free_space_manager (std::string manager_s)
{
    int manager;
    bool valid = true;
    try {
        manager = std::stoi(manager_s);
    }
    catch (std::invalid_argument e)
    {
        valid = false;
    }

    std::map<int, std::shared_ptr<SpaceManager> >::iterator location;

    if (valid &&
            (location = space_managers.find(manager)) == space_managers.end())
        valid = false;

    if (!valid)
    {
        std::cerr << "Algoritmo de gerenciamento de espaço livre '" <<
            manager_s << "' inválido\n";
        return;
    }

    std::cout << "Algoritmo de gerenciamento de espaço livre '" <<
        location->second->get_name() << "' selecionado\n";

    free_space_manager = manager;
}

// Selects page replace manager
void EP::select_page_replace_manager (std::string manager_s)
{
    int manager;
    bool valid = true;
    try {
        manager = std::stoi(manager_s);
    }
    catch (std::invalid_argument e)
    {
        valid = false;
    }

    std::map<int, std::shared_ptr<PageReplacer> >::iterator location;

    if (valid &&
            (location = page_replacers.find(manager)) == page_replacers.end())
        valid = false;

    if (!valid)
    {
        std::cerr << "Algoritmo de substituição de páginas '" <<
            manager_s << "' inválido\n";
        return;
    }

    std::cout << "Algoritmo de substituição de páginas '" <<
        location->second->get_name() << "' selecionado\n";
    
    page_replace_manager = manager;
}

// Runs simulator
void EP::run(std::string interval_s)
{
    if (!file_loaded)
    {
        std::cerr << "Nenhum arquivo carregado!\n";
        return;
    }

    int interval;
    try {
        interval = std::stoi(interval_s);
    }
    catch (std::invalid_argument e)
    {
        std::cerr << "Intervalo inválido '" << interval_s << "'\n";
        return;
    }

    std::shared_ptr<PageReplacer> page_replacer;
    std::shared_ptr<SpaceManager> space_manager;

    page_replacer = page_replacers.at(page_replace_manager);
    space_manager = space_managers.at(free_space_manager);

    space_manager->set_page_replacer(page_replacer);

    uint t = 0;
}

// Insert space manager option
bool EP::add_space_manager(int option_number, SpaceManager *manager)
{
    if (!manager)
        return false;

    std::pair<std::map<int, std::shared_ptr<SpaceManager> >::iterator, bool>
        ret = space_managers.insert(
                std::pair<int, std::shared_ptr<SpaceManager> > (
                    option_number,
                    std::shared_ptr<SpaceManager> (manager)));

    return ret.second;
}

// Insert page replacer option
bool EP::add_page_replacer(int option_number, PageReplacer *replacer)
{
    if (!replacer)
        return false;

    std::pair<std::map<int, std::shared_ptr<PageReplacer> >::iterator, bool>
        ret = page_replacers.insert(
                std::pair<int, std::shared_ptr<PageReplacer> > ( 
                    option_number,
                    std::shared_ptr<PageReplacer> (replacer)));

    return ret.second;
}