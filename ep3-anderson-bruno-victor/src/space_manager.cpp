#include "space_manager.hpp"

SpaceManager::SpaceManager (EP &ep, std::string name) :
    name(name)
{
}

std::string SpaceManager::get_name()
{
    return name;
}
