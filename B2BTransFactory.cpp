#include "B2BTransFactory.h"
#include "B2BTransDialog.h"
#include "B2BTransSession.h"

#include "AmUriParser.h"
#include "AmConfig.h"
#include "log.h"

#include <sstream>
#include <memory>
#include <iterator>

#define MOD_NAME "b2bwithtrans"
#define DST_OVERRIDE_LIST "dst_override_list"

EXPORT_SESSION_FACTORY(B2BTransFactory,MOD_NAME);
EXPORT_PLUGIN_CLASS_FACTORY(B2BTransFactory,MOD_NAME);

B2BTransFactory::DialogsType B2BTransFactory::dialogs;
B2BTransFactory::DeadDialogsType B2BTransFactory::deadDialogs;
AmMutex B2BTransFactory::dialogsLock;

B2BTransFactory::B2BTransFactory(const std::string& applicationName)
  : AmSessionFactory(applicationName), AmDynInvokeFactory(applicationName)
{
  std::ostringstream os;
  os << "constructing factory instance: applicationName=" << applicationName;
  os << "; this=" << this << std::endl;
  DBG("%s",os.str().c_str());
}

B2BTransFactory::~B2BTransFactory()
{
  std::ostringstream os;
  os << "de-constructing factory instance";
  os << "; this=" << this << std::endl;
  DBG("%s",os.str().c_str());
}

int B2BTransFactory::onLoad()
{
  std::ostringstream os;
  os << "loaded this=" << this << "; ";

  AmConfigReader reader;
  if(reader.loadFile(AmConfig::ModConfigPath + std::string(MOD_NAME ".conf")))
  {
    os << "could not load config file; aborting" << std::endl;
    ERROR("%s",os.str().c_str());
    return -1;
  }

  if(reader.hasParameter(DST_OVERRIDE_LIST))
  {
    os << "using destination overrides :" << std::endl;
    parseOverrides(reader.getParameter(DST_OVERRIDE_LIST));

    for(OverridesIter i = dstOverrides.begin(); i != dstOverrides.end(); ++i)
    {
      os << '\t' << i->first << " -> " << i->second << std::endl;
    }
  }

  DBG("%s",os.str().c_str());

  return 0;
}

void B2BTransFactory::parseOverrides(const std::string& list)
{
  size_t itemStart = 0, itemEnd = 0;
  while(itemStart != std::string::npos && itemEnd != std::string::npos)
  {
    itemStart = list.find_first_not_of("&",itemEnd);
    if(itemStart != std::string::npos)
    {
      itemEnd = list.find_first_of("&",itemStart);
      if(itemEnd != std::string::npos)
      {
	addOverride(list.substr(itemStart,itemEnd - itemStart));
      }
      else
      {
	addOverride(list.substr(itemStart));
      }
    }
  }
}

void B2BTransFactory::addOverride(const std::string& item)
{
  size_t separator = item.find("=");
  //DBG("%s : %s",item.substr(0,separator).c_str(),item.substr(separator + 1).c_str());
  dstOverrides[item.substr(0,separator)] = item.substr(separator + 1);
}

AmSession* B2BTransFactory::onInvite(const AmSipRequest& req)
{
  std::ostringstream os;
  os << "new dialog=" << req.callid << "; ";
  os << "from=" << req.from << "; ";
  os << "to=" << req.to << "; ";
  
  std::auto_ptr< B2BTransDialog > dialog(new B2BTransDialog(req.callid));
  dialog->setListener(this);

  os << "dialog=" << dialog.get() << "; ";
  os << "this=" << this << std::endl;

  INFO("%s",os.str().c_str());

  dialogsLock.lock();

  dialogs[dialog->getID()] = dialog.get();

  dialogsLock.unlock();

  return dialog.release()->begin();
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
  std::copy(argv.begin(),argv.end(),std::ostream_iterator< std::string >(os,";"));

  DBG("%s",os.str().c_str());

  if(method == "transfer")
  {
    std::vector< std::string > params(args.asStringVector());
    if(params.size() != 3)
      ret.push("incorrect params: callid local-tag dest-uri expected");
    else
      ret.push(transfer(params[0],params[1],params[2]).c_str());
  }
  else if(method == "list")
  {
    ret.push(listDialogs().c_str());
  }
  else if(method == "flushdead")
  {
    ret.push(flushDeadDialogs().c_str());
  }
  else if(method == "test")
  {
    ret.push(os.str().c_str());
  }
  else
  {
    ret.push("di(\"b2bwithtrans\" \"transfer\" \"callid\" \"local-tag\" \"dest-uri\")" \
      "; di(\"b2bwithtrans\" \"list\"); di(\"b2bwithtrans\" \"flushdead\")");
  }
}

void B2BTransFactory::onTerminated(B2BTransDialog* dialog)
{
  std::ostringstream os;
  os << "this=" << this << "; dialog=" << dialog;

  dialogsLock.lock();
  
  if(dialogs.erase(dialog->getID()) > 0)
    os << "; found and erased dialog";
  
  deadDialogs.push_back(dialog);

  dialogsLock.unlock();

  os << "; dialog set aside for deletion" << std::endl;

  DBG("%s",os.str().c_str());
}

std::string B2BTransFactory::getDstOverride(const std::string& to)
{
  OverridesIter i = dstOverrides.find(to);
  return i == dstOverrides.end() ? "" : i->second;
}

std::string B2BTransFactory::transfer(
    const std::string& callid,const std::string& localtag,const std::string& desturi)
{
  std::ostringstream os,ret;
  os << "callid=" << callid << "; localtag=" << localtag;
  os << "; desturi=" << desturi << "; this=" << this << std::endl;
  DBG("%s",os.str().c_str());

  AmUriParser p;
  p.uri = desturi;
  p.parse_uri();
  p.dump();

  bool valid = true;

  if(desturi.find("sip:") == std::string::npos)
  {
    ret << "invalid desturi=" << desturi << ": must be 'sip:'";
    valid = false;
  }
  if(p.uri_user.empty() || p.uri_host.empty())
  {
    ret << "invalid desturi=" << desturi << ": uri must contain user and host parts";
    valid = false;
  }
  if(!p.display_name.empty())
  {
    ret << "invalid desturi=" << desturi << ": exclude display-name=" << p.display_name;
    valid = false;
  }
  if(!p.uri_headers.empty())
  {
    ret << "invalid desturi=" << desturi << ": exclude uri-headers=" << p.uri_headers;
    valid = false;
  }
  if(!p.uri_param.empty())
  {
    ret << "invalid desturi=" << desturi << ": exclude  uri-param=" << p.uri_param;
    valid = false;
  }
  
  if(valid)
  {
    dialogsLock.lock();

    DialogsIter t = dialogs.find(callid);
    if(t != dialogs.end())
    {
      if(t->second->isTagValid(localtag))
      {
	t->second->transfer(localtag, desturi);
	ret << "ok";
      }
      else
      {
	ret << "invalid localtag=" << localtag;
      }
    }
    else
    {
      ret << "invalid callid=" << callid;
    }

    dialogsLock.unlock();
  }

  return ret.str();
}

const std::string B2BTransFactory::listDialogs() const
{
  std::ostringstream os;
  //THIS DOES NOT SCALE - use Redis or something similar

  os << "{\"dialogs\":[";

  dialogsLock.lock();

  for(DialogsType::const_iterator d = dialogs.begin(); d != dialogs.end(); ++d)
  {
    os << d->second->toString();
    if(++d != dialogs.end())
    {
      os << ",";
    }
     --d;
  }

  dialogsLock.unlock();

  os << "]}";

  DBG("%s",os.str().c_str());

  return os.str();
}

std::string B2BTransFactory::flushDeadDialogs()
{
  std::ostringstream os;
  //THIS IS RUBBISH - just for PoC

  dialogsLock.lock();

  os << "flushed " << deadDialogs.size() << " dialogs" << std::endl;

  for(DeadDialogsIter d = deadDialogs.begin(); d != deadDialogs.end(); ++d)
  {
    delete *d;
  }
  
  deadDialogs.clear();

  dialogsLock.unlock();

  DBG("%s",os.str().c_str());

  return os.str();
}
