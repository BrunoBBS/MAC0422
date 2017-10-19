#include "space_manager.hpp"

SpaceManager::SpaceManager (EP &ep, int man_id) :
    man_id(man_id)
{
    ep.add_space_manager(man_id, this);
}
