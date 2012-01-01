#pragma once
#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <map>
namespace reg
{
  struct sigs
  {
    typedef void
    declare_params(int);
    typedef void
    declare_io(int);
    typedef void
    configure(void);
    typedef int
    process(void);

    typedef boost::function<declare_params> fn_declare_params;
    typedef boost::function<declare_io> fn_declare_io;
    typedef boost::function<configure> fn_configure;
    typedef boost::function<process> fn_process;

#define REG_DECLARE_GET_F(FncName)                                             \
    struct get_f_##FncName##_                                                  \
    {                                                                          \
      template<class U,typename Sig>                                           \
      static boost::function<Sig> has_f(__decltype(&U::FncName))                      \
          {return boost::function<Sig>(&U::FncName);}                                 \
      template<class U,typename Sig>                                           \
      static boost::function<Sig> has_f(...){return boost::function<Sig>();}                 \
    }

#define REG_GET_F(Type,FncName,Signature)                                      \
    reg::sigs::get_f_##FncName##_::has_f<Type,Signature>(0)

    REG_DECLARE_GET_F(declare_params); //
    REG_DECLARE_GET_F(declare_io); //
    REG_DECLARE_GET_F(configure); //
    REG_DECLARE_GET_F(process);
    //
  };

  struct cell_factory;
  struct cell
  {
    cell(const cell_factory* factory);

    void
    init_holder();

    int
    process();

    void
    configure();

    void
    declare_params();

    void
    declare_io();
    const cell_factory* factory;
    boost::any holder;
    sigs::fn_declare_params declare_params_;
    sigs::fn_declare_io declare_io_;
    sigs::fn_configure configure_;
    sigs::fn_process process_;

  };
  struct cell_factory
  {
    virtual
    ~cell_factory();

    cell
    create() const;

    virtual
    void
    populate_member(cell& c) const = 0;

    virtual
    void
    populate_static(cell& c) const = 0;

  };

  template<class CellT>
  struct cell_factory_: cell_factory
  {
    typedef boost::shared_ptr<CellT> PtrT;

    //static functions.
    sigs::fn_declare_params declare_params;
    sigs::fn_declare_io declare_io;

    //member functions, have different signatures...
    //these must be handled with boost::bind
    typedef void
    sig_configure(CellT*);
    typedef int
    sig_process(CellT*);

    boost::function<sig_configure> configure;
    boost::function<sig_process> process;

    cell_factory_()
        :
          declare_params(REG_GET_F(CellT,declare_params,sigs::declare_params)),
          declare_io(REG_GET_F(CellT,declare_io,sigs::declare_io)),
          configure(REG_GET_F(CellT,configure,sig_configure)),
          process(REG_GET_F(CellT,process,sig_process))
    {
    }

    void
    populate_member(cell& c) const
    {
      PtrT p(new CellT);
      c.holder = p;
      if (configure)
        c.configure_ = boost::bind(configure, p.get());
      if (process)
        c.process_ = boost::bind(process, p.get());
    }

    void
    populate_static(cell& c) const
    {
      c.declare_params_ = declare_params;
      c.declare_io_ = declare_io;
    }

    static const cell_factory*
    instance()
    {
      static cell_factory_ cf;
      return &cf;
    }
  };

  struct module
  {
    typedef std::map<std::string, const cell_factory*> storage;
    storage factories;
    const cell_factory*
    get_factory(const char* name) const;
    void
    register_factory(const char* name, const cell_factory* factory);
  };

  const module*
  load(const char* name);

}

#define MODULE_GET(NAME)                                                       \
    get_module_##NAME()

#define MODULE_DECLARE(NAME)                                                   \
    reg::module* get_module_##NAME()

#define MODULE(NAME)                                                           \
extern "C" {                                                                   \
  const void* module_init_##NAME(void);                                        \
}                                                                              \
MODULE_DECLARE(NAME) {                                                         \
  static reg::module m;                                                        \
  return &m;                                                                   \
}                                                                              \
const void* module_init_##NAME(void){                                          \
  return MODULE_GET(NAME);                                                     \
}

#define CELL(MNAME,NAME,CELLT)                                                 \
extern MODULE_DECLARE(MNAME);                                                  \
struct reg_##NAME##_##MNAME##_ {                                               \
  reg_##NAME##_##MNAME##_(){                                                   \
    reg::module* m = MODULE_GET(MNAME);                                        \
    m->register_factory(#NAME,reg::cell_factory_<CELLT>::instance());          \
  }                                                                            \
  static const reg_##NAME##_##MNAME##_ reger_;                                 \
};                                                                             \
const reg_##NAME##_##MNAME##_ reg_##NAME##_##MNAME##_::reger_;

