#ifndef _B2BTRANSLISTENERS_H_
#define _B2BTRANSLISTENERS_H_

class B2BTransSession;

class B2BTransSessionListener
{
  public:
  virtual void onStarted(B2BTransSession* sess) = 0;
  virtual void onStopped(B2BTransSession* sess) = 0;
};

class B2BTransDialogListener
{
  public:
};

#endif //_B2BTRANSLISTENERS_H_
