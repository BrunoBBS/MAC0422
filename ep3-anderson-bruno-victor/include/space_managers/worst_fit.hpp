#ifndef __SP_MAN_WORST_FIT_HPP__

#include "ep_base.hpp"
#include "space_manager.hpp"

namespace SpaceManagers
{
    class WorstFit : public SpaceManager 
    {
        public:
            WorstFit(EP &ep);

        private:
            int allocate(int size);
            void free(int pos);
    };
}

#endif
