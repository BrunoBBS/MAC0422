#include "space_managers/worst_fit.hpp"

SpaceManagers::WorstFit::WorstFit(EP &ep) :
    SpaceManager(ep, "Worst Fit")
{}

int SpaceManagers::WorstFit::allocate(int size)
{
    return 0;
}

void SpaceManagers::WorstFit::free(int pos)
{
}
