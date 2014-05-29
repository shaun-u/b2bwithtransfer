#ifndef _B2BTRANSFACTORY_H_
#define _B2BTRANSFACTORY_H_

#include "AmApi.h"

#include "B2BTransListeners.h"

#include <map>
#include <string>
#include <vector>

class B2BTransDialog;

class B2BTransFactory :
  public AmSessionFactory, public AmDynInvokeFactory, public AmDynInvoke, public B2BTransDialogListener
{
  //THIS DOES NOT SCALE - use Redis or something similar, rather than a map!
  typedef std::map< std::string,B2BTransDialog* > DialogsType;
  typedef DialogsType::iterator DialogsIter;
  typedef std::vector< B2BTransDialog* > DeadDialogsType;
  typedef DeadDialogsType::iterator DeadDialogsIter;
  static DialogsType dialogs;
  static DeadDialogsType deadDialogs;
  static AmMutex dialogsLock;


  public:
  B2BTransFactory(const std::string& applicationName);
  virtual ~B2BTransFactory();
  
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

  void onTerminated(B2BTransDialog* dialog);

  private:
  const std::string listDialogs() const;
  std::string flushDeadDialogs();  
};

#endif //_B2BTRANSFACTORY_H_
