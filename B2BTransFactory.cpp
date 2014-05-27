#include "B2BTransFactory.h"
#include "B2BTransDialog.h"

#include "log.h"
#include <sstream>
#include <memory>
#include <iterator>

#define MOD_NAME "b2bwithtrans"

EXPORT_SESSION_FACTORY(B2BTransFactory,MOD_NAME);
EXPORT_PLUGIN_CLASS_FACTORY(B2BTransFactory,MOD_NAME);

B2BTransFactory::DialogsType B2BTransFactory::dialogs;

B2BTransFactory::B2BTransFactory(const std::string& applicationName)
  : AmSessionFactory(applicationName), AmDynInvokeFactory(applicationName)
{
  std::ostringstream os;
  os << "constructing factory instance: applicationName=" << applicationName;
  os << "; this=" << this << std::endl;
  DBG("%s",os.str().c_str());
}

int B2BTransFactory::onLoad()
{
  std::ostringstream os;
  os << "loaded this=" << this << std::endl;
  DBG("%s",os.str().c_str());
  return 0;
}

AmSession* B2BTransFactory::onInvite(const AmSipRequest& req)
{
  std::ostringstream os;
  os << "new dialog=" << req.callid << "; ";
  os << "from=" << req.from << "; ";
  os << "to=" << req.to << "; ";
  os << "this=" << this << std::endl;

  INFO("%s",os.str().c_str());
  
  std::auto_ptr< B2BTransDialog > dialog(new B2BTransDialog(req.callid));

  dialogs[dialog->getID()] = dialog.release();
  
  return NULL;
}

AmDynInvoke* B2BTransFactory::getInstance()
{
  std::ostringstream os;
  os << "this=" << this << std::endl;
  DBG("%s",os.str().c_str());

  return this;
}

void B2BTransFactory::invoke(const string& method, const AmArg& args, AmArg& ret)
{
  std::vector< std::string > argv(args.asStringVector());
  std::ostringstream os;

  os << "method=" << method << std::endl;
  os << "args size=" << argv.size() << std::endl;
  std::copy(argv.begin(),argv.end(),std::ostream_iterator< std::string >(os,"\n"));

  DBG("%s",os.str().c_str());

  if(method == "test")
  {
    ret.push(os.str().c_str());
  }
  else if(method == "list")
  {
    ret.push(listDialogs().c_str());
  }
}

const std::string B2BTransFactory::listDialogs() const
{
  std::ostringstream os;
  WARN("THIS DOES NOT SCALE; NOR IS IT THREAD SAFE");

  if(dialogs.size() == 0)
  {
    os << "empty" << std::endl;
  }
  else
  {
    for(DialogsType::const_iterator d = dialogs.begin(); d != dialogs.end(); ++d)
    {
      os << d->first << " : " << d->second->toString() << std::endl;
    }
  }

  DBG("%s",os.str().c_str());

  return os.str();
}
