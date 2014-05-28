#ifndef _B2BTRANSSESSION_H_
#define _B2BTRANSSESSION_H_

#include "AmSession.h"

#include <memory>

class AmRingTone;
class B2BTransSessionListener;

class B2BTransSession : public AmSession
{
  std::auto_ptr< AmRingTone > ringTone;
  typedef std::vector< B2BTransSessionListener* > ListenerType;
  ListenerType listeners;
  typedef ListenerType::iterator ListenerIter; 

  public:
  
  B2BTransSession();
  virtual ~B2BTransSession();

  void addListener(B2BTransSessionListener* listener);

  void playRinging(bool manual=true);
  void playStop(bool manual=true);

  /** AmSession overrides */
  void onSessionStart(const AmSipRequest& req);
  void onBye(const AmSipRequest& req);
};

#endif //_B2BTRANSSESSION_H_
