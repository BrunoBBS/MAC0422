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
    std::vector<Process> processes;
    std::map<uint, std::vector<Event> > events;

    uint uid = 0;

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

            // If line is valid
            if (valid_line)
            {
                // Generate compress event
                Event evn;
                evn.type = COMPRESS;
                evn.uid = -1;
                evn.pos = -1;

                // Add to events
                std::map<uint, std::vector<Event> >::iterator evn_time =
                    events.insert({time, std::vector<Event> ()}).first;

                evn_time->second.push_back(evn);
            }
        }

        // Parse process
        else if (parts.size() > 3 && !(parts.size() % 2))
        {
            uint t0, tf, b;
            t0 = tf = b = 0;
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

            {
                // Generate process start event
                Event evn;
                evn.type = START;
                evn.uid = uid;
                evn.pos = -1;

                // Add to events
                std::map<uint, std::vector<Event> >::iterator evn_time =
                    events.insert({t0, std::vector<Event> ()}).first;

                evn_time->second.push_back(evn);
            }

            for (unsigned int i = 4;
                    valid_line && i < parts.size() - 1; i += 2)
            {
                uint position, time;
                position = time = 0;
                try
                {
                    position = std::stoi(parts[i]);
                    time = std::stoi(parts[i + 1]);
                }
                catch (std::invalid_argument e)
                {
                    valid_line = false;
                }

                // Fail if memory access is not from t0 to tf
                if (time < t0 || time > tf)
                {
                    std::cerr << "Acesso de memória inválido (fora de [" <<
                        t0 << ", " << tf << "])\n";
                    valid_line = false;
                    continue;
                }

                // Fail if access is outside process range
                if (position >= b)
                {
                    std::cerr << "Processo não pode acessar posição " <<
                        position << "\n";
                    valid_line = false;
                    continue;
                }


                // Generate memory access event
                Event evn;
                evn.type = ACCESS;
                evn.uid = uid;
                evn.pos = position;

                // Add to events
                std::map<uint, std::vector<Event> >::iterator evn_time =
                    events.insert({time, std::vector<Event> ()}).first;

                evn_time->second.push_back(evn);
            }
            
            {
                // Generate process end event
                Event evn;
                evn.type = END;
                evn.uid = uid;
                evn.pos = -1;

                // Add to events
                std::map<uint, std::vector<Event> >::iterator evn_time =
                    events.insert({tf, std::vector<Event> ()}).first;

                evn_time->second.push_back(evn);
            }

            if (valid_line)
                processes.push_back(Process(t0, tf, b, uid++, name));
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
            events.size() << " eventos carregados\n";
    }

    // Now we loaded a file
    file_loaded = true;

    // Sort events by type
    for (std::pair<uint, std::vector<Event> > evn_vect : events)
        std::sort(evn_vect.second.begin(), evn_vect.second.end(),
                [](Event const &a, Event const &b)
                {
                    return a.type > b.type; 
                });

    // Saves input structures
    phys_mem = tot_mem;
    virt_mem = vir_mem;
    alloc_size = alloc_s;
    page_size = pages_s;
    evn = events;
    process_list = processes;

    memory = std::shared_ptr<Memory>(new Memory(tot_mem, vir_mem));
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

    running = true;

    // Sets up page replacer and space manager
    std::shared_ptr<PageReplacer> page_replacer;
    std::shared_ptr<SpaceManager> space_manager;

    page_replacer = page_replacers.at(page_replace_manager);
    space_manager = space_managers.at(free_space_manager);

    space_manager->set_page_replacer(page_replacer);

    space_manager->init();
    page_replacer->init();

    // Current time
    uint t = 0;

    // Iterator of relevant timestamps
    std::map<uint, std::vector<Event> >::iterator next_rel_time;
    next_rel_time = evn.begin();

    // While there are still relevant events
    while (next_rel_time != evn.end())
    {
        // Warns page replacer that a clock instant has elapsed
        page_replacer->clock();

        curr_instant = t;

        // If there are events to process
        if (next_rel_time->first == t)
        {
            std::vector<Event> &events = next_rel_time->second;
           
            // Current event on this instant being accessed
            curr_event = 0;

            // Run each access
            for (Event event : events)
            {
                switch (event.type)
                {
                    case START:
                        space_manager->start_process(
                                process_list[event.uid]);
                        break;
                    case ACCESS:
                        space_manager->write(event.pos,
                                process_list[event.uid],
                                event.uid);
                        break;
                    case END:
                        space_manager->end_process(
                                process_list[event.uid]);
                        break;
                    case COMPRESS:
                        space_manager->compress();
                        break;
                    default:
                        {
                            std::cerr << "Tipo de evento " << event.type
                                << " não é conhecido!\n";
                            running = false;
                            return;
                        }
                        break;
                }

                curr_event++;
            }

            // Go to next relevant time
            next_rel_time++;
        }

        // Print on intervals
        if (interval > 0 && !(t % interval))
        {
            std::string interval_title = "Starting print of instant " +
                std::to_string(t);
            
            std::cout << std::endl;

            for (int i = 0; i < interval_title.size() + 4; i++)
                std::cout << "*";

            std::cout << std::endl;
            
            std::cout << "* " << interval_title << " *" << std::endl;

            for (int i = 0; i < interval_title.size() + 4; i++)
                std::cout << "*";
            
            std::cout << std::endl << std::endl;

            memory->print();
        }
        // Moves to next instant
        t++;
    }

    space_manager->end();
    page_replacer->end();

    running = false;
}

// Insert space manager option
bool EP::add_space_manager(int option_number, SpaceManager *manager)
{
    if (!manager)
        return false;

    return space_managers.insert(
            {option_number, std::shared_ptr<SpaceManager> (manager)}).second;
}

// Insert page replacer option
bool EP::add_page_replacer(int option_number, PageReplacer *replacer)
{
    if (!replacer)
        return false;

    return page_replacers.insert(
            {option_number, std::shared_ptr<PageReplacer> (replacer)}).second;
}
