#include "registry.hpp"

namespace cella
{
  struct CellB
  {
    void
    configure()
    {
      std::cout << __PRETTY_FUNCTION__ << std::endl;
      std::cout << "\tsucka\t" << "woot woot woot" << std::endl;
    }
    int
    process()
    {
      std::cout << "Woot:\n\t" << __PRETTY_FUNCTION__ << std::endl;
      return 1;
    }
  };
}

CELL(cellz, cellb, cella::CellB)
