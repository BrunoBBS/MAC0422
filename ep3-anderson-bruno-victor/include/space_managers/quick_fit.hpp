#ifndef __SP_MAN_QUICK_FIT_HPP__

#include "ep_base.hpp"
#include "space_manager.hpp"

namespace SpaceManagers
{
class QuickFit : public SpaceManager
{
  public:
    QuickFit(EP &ep);
    ~QuickFit();

    void init();

  private:
    int allocate(int size);
    void free(int pos);
};
}

#endif
