#ifndef _B2BTRANSSESSION_H_
#define _B2BTRANSSESSION_H_

#include "AmSession.h"

#include <memory>

class AmRingTone;
class AmSessionAudioConnector;
class B2BTransSessionListener;

class B2BTransSession : public AmSession
{
  std::auto_ptr< AmRingTone > ringTone;
  typedef std::vector< B2BTransSessionListener* > ListenerType;
  ListenerType listeners;
  typedef ListenerType::iterator ListenerIter; 
  AmSessionAudioConnector* bridge;

  public:
  
  B2BTransSession(AmSessionAudioConnector* audioBridge);
  virtual ~B2BTransSession();

  void addListener(B2BTransSessionListener* listener);

  void playRinging();
  void bridgeAudio(AmSessionAudioConnector* audioBridge);

  /** initiate outgoing call */
  void call(const std::string& callid, const std::string& to,
    const std::string& from, const std::string& callgroup);

  /** AmSession overrides */
  void onSessionStart(const AmSipRequest& req);
  void onSessionStart(const AmSipReply& reply);
  void onBye(const AmSipRequest& req);
  void onSipReply(const AmSipReply& reply, int old_dlg_status, const string& trans_method);
  void process(AmEvent* evt);

  private:
  std::string getRUri(const std::string& uri);
};

enum B2BDialoutAction
{
  DoConnect = 100, DoBridge
};

struct B2BDialoutEvent : public AmEvent
{
  B2BTransSession* sess;
  B2BDialoutEvent(B2BTransSession* session) : AmEvent(DoConnect),sess(session) {}
};

struct B2BBridgeAudioEvent : public AmEvent
{
  AmSessionAudioConnector* bridge;
  B2BBridgeAudioEvent(AmSessionAudioConnector* audioBridge) : AmEvent(DoBridge), bridge(audioBridge) {}
};

#endif //_B2BTRANSSESSION_H_
