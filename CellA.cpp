#include "registry.hpp"
#include "util.hpp"

namespace cella
{
  struct CellA
  {
    void
    configure()
    {
      std::cout << __PRETTY_FUNCTION__ << std::endl;
      std::cout << "\tkow\t" << "woot woot woot" << std::endl;
      std::cout << ecto::name_of<CellA>() << std::endl;
    }
    int
    process()
    {
      std::cout << "Woot:\n\t" << __PRETTY_FUNCTION__ << std::endl;
      return 1;
    }
  };
}

CELL(cellz, cella, cella::CellA)
MODULE(cellz)
