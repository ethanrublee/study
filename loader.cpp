#include "registry.hpp"

int main(int argv, char ** argc)
{
    const char* name = argc[1];
    const char* cell_name = argc[2];
    const reg::module* f = reg::load(name);
    reg::cell cell =  f->get_factory(cell_name)->create();
    cell.configure();
    cell.process();
}
