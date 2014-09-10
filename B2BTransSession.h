#ifndef _B2BTRANSSESSION_H_
#define _B2BTRANSSESSION_H_

#include "AmSession.h"

#include <memory>

class AmRingTone;
//class AmAudioFile;
class AmSessionAudioConnector;
class B2BTransSessionListener;

class B2BTransSession : public AmSession
{
  std::auto_ptr< AmRingTone > ringTone;
  //std::auto_ptr< AmAudioFile > transferInProgress;
  //std::auto_ptr< AmAudioFile > transferComplete;

  typedef std::vector< B2BTransSessionListener* > ListenerType;
  ListenerType listeners;
  typedef ListenerType::iterator ListenerIter; 

  public:
  
  B2BTransSession();
  virtual ~B2BTransSession();

  void addListener(B2BTransSessionListener* listener);

  void playRinging();
  void playStop();
  //void playTransferInProgress();
  void bridgeAudio(AmSessionAudioConnector* audioBridge);
  void unbridgeAudio(AmSessionAudioConnector* audioBridge);

  /** initiate outgoing call */
  void call(const std::string& callid, const std::string& to,
    const std::string& from, const std::string& callgroup);

  /** AmSession overrides */
  void onSessionStart(const AmSipRequest& req);
  void onSessionStart(const AmSipReply& reply);
  void onBye(const AmSipRequest& req);
  void onOutboundCallFailed(const AmSipReply& reply);
  void process(AmEvent* evt);
  void onAudioEvent(AmAudioEvent* audio_ev);

  private:
  std::string getRUri(const std::string& uri);
  void onCommonStart();
};

enum B2BDialoutAction
{
  DoConnect = 100, DoBridge = 110, DoTerminate = 120, DoFilePlayed = 130, DoTransfer = 140
};

struct B2BDialoutEvent : public AmEvent
{
  B2BTransSession* sess;
  std::string dstOverride;
  B2BDialoutEvent(B2BTransSession* session, const std::string& dst) : AmEvent(DoConnect),sess(session),dstOverride(dst) {}
};

struct B2BTransferEvent : public AmEvent
{
  B2BTransSession* sess;
  std::string uri;
  B2BTransferEvent(
    B2BTransSession* session, const std::string& desturi) 
      : AmEvent(DoTransfer),sess(session),uri(desturi) {}
};

struct B2BBridgeAudioEvent : public AmEvent
{
  AmSessionAudioConnector* bridge;
  B2BBridgeAudioEvent(AmSessionAudioConnector* audioBridge) : AmEvent(DoBridge), bridge(audioBridge) {}
};

//struct B2BFilePlayedEvent : public AmEvent
//{
//   B2BFilePlayedEvent() : AmEvent(DoFilePlayed) {}
//};

struct B2BTerminateEvent : public AmEvent
{
   B2BTerminateEvent() : AmEvent(DoTerminate) {}
};

#endif //_B2BTRANSSESSION_H_
