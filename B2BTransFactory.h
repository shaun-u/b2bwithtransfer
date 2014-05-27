#ifndef _B2BTRANSFACTORY_H_
#define _B2BTRANSFACTORY_H_

#include "AmApi.h"

class B2BTransFactory :
  public AmSessionFactory, public AmDynInvokeFactory, public AmDynInvoke
{
  /**
   * Dynamic invoke instance
   */
  static B2BTransFactory* _instance;
  
  public:
  CallTransFactory(const std::string& applicationName);
  
  int onLoad();

  /**
   * incoming call-leg entry point
   * starts new b2b session
   */
  AmSession* onInvite(const AmSipRequest& req);
  
  /**
   * Dynamic Invoke Methods
   */
  AmDynInvoke* getInstance();
  void invoke(const string& method, const AmArg& args, AmArg& ret);

  
};

#endif //_B2BTRANSFACTORY_H_
