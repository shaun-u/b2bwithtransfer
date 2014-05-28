#ifndef _B2BTRANSFACTORY_H_
#define _B2BTRANSFACTORY_H_

#include "AmApi.h"

#include <map>
#include <string>

class B2BTransDialog;

class B2BTransFactory :
  public AmSessionFactory, public AmDynInvokeFactory, public AmDynInvoke
{
  typedef std::map< std::string,B2BTransDialog* > DialogsType;
  static DialogsType dialogs;
  static AmMutex dialogsLock;


  public:
  B2BTransFactory(const std::string& applicationName);
  
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

  private:
  const std::string listDialogs() const;
  
};

#endif //_B2BTRANSFACTORY_H_
