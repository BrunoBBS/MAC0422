#include "space_managers/best_fit.hpp"

SpaceManagers::BestFit::BestFit(EP &ep) :
    SpaceManager(ep, "Best Fit")
{}

int SpaceManagers::BestFit::allocate(int size)
{
    return 0;
}

void SpaceManagers::BestFit::free(int pos)
{
}
