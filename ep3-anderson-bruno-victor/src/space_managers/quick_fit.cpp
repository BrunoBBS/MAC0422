#include "space_managers/quick_fit.hpp"

SpaceManagers::QuickFit::QuickFit(EP &ep) : SpaceManager(ep, "Quick Fit")
{
}

void SpaceManagers::QuickFit::init()
{
}

int SpaceManagers::QuickFit::allocate(int size)
{
    // Can't allocate 0 or less bytes
    if (size <= 0)
        return -1;

    // Minimum allocation units required for specified size
    int min_req = (size / ep.get_alloc_size()) +
                  (size % ep.get_alloc_size() ? 1 : 0);
    return 0;
}

void SpaceManagers::QuickFit::free(int pos)
{
}

void SpaceManagers::QuickFit::redist_mem()
{
    /*
     * ->Para cada bloco de memoria:
     *     -> Para cada tamanho popular:
     *         -> Divide o bloco atual em espaços de tamanho popular atual
     *         -> Adiciona cada endereço inicial em sua respectiva lista
     */

    mem_block *curr_block = s_anchor.next;
    // For each block of memory
    for (; *curr_block != e_anchor; curr_block = curr_block->next)
    {
        // For each frequent size
        for (auto pop_size : pop_sizes)
        {
            // Just rename some things
            // The current size we are dividing the current memory block in
            int popular_size = pop_size.first;

            // The list of popular-sized blocks of memory
            std::list<mem_block> *subdivision_list = &pop_size.second;

            /*
             * For each popular-sized block available inside current memory
             * block
             */
            for (int i = 0; i < curr_block->size / popular_size; i++)
            {
                mem_block block;
                block.init = curr_block->init + i * popular_size;
                block.size = popular_size;
                subdivision_list->push_back(block);
            }
        }
    }
}

void SpaceManagers::QuickFit::pre_calculation()
{
    /*
     * ->lê os processos               
     * ->se o tamanho de alocação é diferente:
     *     -> adiciona na lista
     * ->senão:
     *     -> soma o existente
     * ->separa os <n_sizes> mais repetidos
     * ->separa os blocos de memoria em blocos menores de cada tamanho
     * ->adiciona cada novo bloquinho na lista de seu respectivo tamanho
     * 
     */
    std::list<alloc_size> sizes;
}
