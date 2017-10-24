#ifndef __SP_MAN_BEST_FIT_HPP__

#include "ep_base.hpp"
#include "space_manager.hpp"

namespace SpaceManagers
{
    class BestFit : public SpaceManager 
    {
        public:
            BestFit(EP &ep);

        private:
            int allocate(int size);
            void free(int pos);
    };
}

#endif
