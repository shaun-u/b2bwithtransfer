#include "B2BTransFactory.h"

#include "log.h"

#define MOD_NAME "b2bwithtrans"

EXPORT_SESSION_FACTORY(B2BTransFactory,MOD_NAME);
EXPORT_PLUGIN_CLASS_FACTORY(B2BTransFactory,MOD_NAME);

B2BTransFactory* B2BTransFactory::_instance=0;

B2BTransFactory::B2BTransFactory(const std::string& applicationName)
  : AmSessionFactory(applicationName), AmDynInvokeFactory(applicationName)
{
  DBG("constructing factory instance: applicationName=%s",applicationName.c_str());
  if(_instance != 0)
    ERROR("factory instance already exists; overwriting");
  _instance = this;
}

int B2BTransFactory::onLoad()
{
  DBG("loaded");
  return 0;
}

AmSession* B2BTransFactory::onInvite(const AmSipRequest& req)
{
  INFO("new dialog=%s; from=%s; to=%s",req.callid.c_str(),req.from.c_str(),req.to.c_str());
  return NULL;
}

AmDynInvoke* B2BTransFactory::getInstance()
{
  return _instance;
}

void B2BTransFactory::invoke(const string& method, const AmArg& args, AmArg& ret)
{
  DBG("method=%s",method.c_str());
}
