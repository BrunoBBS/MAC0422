#ifndef __SPACE_MANAGER_HPP__
#define __SPACE_MANAGER_HPP__

#include "ep_base.hpp"

class EP;

class SpaceManager
{
    public:
        SpaceManager(EP &ep, std::string name);

        std::string get_name();

    private:
        std::string name;
};

#endif
