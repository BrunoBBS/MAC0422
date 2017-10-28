#include "space_managers/quick_fit.hpp"

SpaceManagers::QuickFit::QuickFit(EP &ep) :
    SpaceManager(ep, "Quick Fit")
{}

int SpaceManagers::QuickFit::allocate(int size)
{
    return 0;
}

void SpaceManagers::QuickFit::free(int pos)
{
}
