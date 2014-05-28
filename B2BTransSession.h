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

  /** initiate outgoing call */
  void call(const std::string& callid, const std::string& to,
    const std::string& from, const std::string& callgroup);

  /** AmSession overrides */
  void onSessionStart(const AmSipRequest& req);
  void onBye(const AmSipRequest& req);
  void process(AmEvent* evt);

  private:
  std::string getRUri(const std::string& uri);
};

enum B2BDialoutAction
{
  DoConnect = 100
};

struct B2BDialoutEvent : public AmEvent
{
  B2BTransSession* sess;
  B2BDialoutEvent(B2BTransSession* session) : AmEvent(DoConnect),sess(session) {}
};

#endif //_B2BTRANSSESSION_H_
