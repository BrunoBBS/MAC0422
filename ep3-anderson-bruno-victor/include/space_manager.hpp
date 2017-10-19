#ifndef __SPACE_MANAGER_HPP__
#define __SPACE_MANAGER_HPP__

#include "ep_base.hpp"

class EP;

class SpaceManager
{
    public:
        SpaceManager(EP &ep, int man_id);

    private:
        int man_id;
};

#endif
