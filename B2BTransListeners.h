#ifndef _B2BTRANSLISTENERS_H_
#define _B2BTRANSLISTENERS_H_

#include <string>

class B2BTransSession;
class B2BTransDialog;

class B2BTransSessionListener
{
  public:
  virtual void onStarted(B2BTransSession* sess) = 0;
  virtual void onStopped(B2BTransSession* sess) = 0;
};

class B2BTransDialogListener
{
  public:
  virtual void onTerminated(B2BTransDialog* dialog) = 0;
  virtual std::string getDstOverride(const std::string& to) = 0;
};

#endif //_B2BTRANSLISTENERS_H_
