#include "registry.hpp"

#include <dlfcn.h>
#include <boost/format.hpp>
#include <stdexcept>
#include <map>

namespace reg
{

  cell::cell(const cell_factory* factory)
      :
        factory(factory)
  {
    factory->populate_static(*this);
  }
  void
  cell::init_holder()
  {
    if (holder.empty())
      factory->populate_member(*this);
  }

  void
  cell::declare_params()
  {
    if (declare_params_)
      declare_params_(0);
  }

  void
  cell::declare_io()
  {
    if (declare_io_)
      declare_io_(0);
  }

  void
  cell::configure()
  {
    init_holder();
    if (!configure_.empty())
      configure_();
  }

  int
  cell::process()
  {

    init_holder();
    if (!process_.empty())
      return process_();
    return 0;
  }

  cell
  cell_factory::create() const
  {
    cell c(this);
    return c;
  }

  cell_factory::~cell_factory()
  {
  }

  struct module_store
  {
    std::map<std::string, const module*> modules;
    static module_store&
    instance()
    {
      static module_store s;
      return s;
    }
  };

  const cell_factory*
  module::get_factory(const char* name) const
  {
    const cell_factory* cp = 0;
    storage::const_iterator it = factories.find(name);
    if (it != factories.end())
      cp = it->second;
    return cp;
  }
  void
  module::register_factory(const char* name, const cell_factory* factory)
  {
    factories[name] = factory;
  }

  const module*
  load(const char* module_name)
  {
    const module* m = module_store::instance().modules[module_name];
    if (m)
      return m;

    std::string name = boost::str(boost::format("lib%s.so") % module_name);
    void* handle = dlopen(name.c_str(), RTLD_LOCAL | RTLD_LAZY);
    if (!handle)
    {
      std::string error = boost::str(boost::format("dlerror:%s") % dlerror());
      throw std::runtime_error(error);
    }

    std::string symname = boost::str(boost::format("module_init_%s") % module_name);
    void* fnc_handle = dlsym(handle, symname.c_str());
    if (!fnc_handle)
    {
      std::string error = boost::str(boost::format("dlerror:%s\n") % dlerror());
      throw std::runtime_error(error);
    }

    typedef const reg::module*
    (*reg_fnc)(void);
    reg_fnc fp = reg_fnc(fnc_handle);
    module_store::instance().modules[module_name] = m = (*fp)();
    return m;
  }

}
