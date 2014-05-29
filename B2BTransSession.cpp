#include "B2BTransSession.h"
#include "B2BTransListeners.h"

#include "AmRingTone.h"
#include "AmUriParser.h"
#include "AmSessionContainer.h"
#include "AmMediaProcessor.h"
#include "AmB2ABSession.h"
#include "log.h"

#include <sstream>

B2BTransSession::B2BTransSession(AmSessionAudioConnector* audioBridge)
  : bridge(audioBridge), ringTone(new AmRingTone(0,2000,4000,440,480))
{
  std::ostringstream os;
  os << "creating session=" << this << std::endl;
  DBG("%s",os.str().c_str());
}

B2BTransSession::~B2BTransSession()
{
  std::ostringstream os;
  os << "destroying session=" << this << std::endl;
  DBG("%s",os.str().c_str());
}

void B2BTransSession::addListener(B2BTransSessionListener* listener)
{
  std::ostringstream os;
  os << "adding listener=" << listener << " to session=" << this  << std::endl;
  DBG("%s",os.str().c_str());

  listeners.push_back(listener);
}

void B2BTransSession::playRinging()
{
  std::ostringstream os;
  os << "playing ringing on session=" << this << std::endl;
  DBG("%s",os.str().c_str());

  setInOut(NULL,ringTone.get());
  if(getDetached())
  {
    AmMediaProcessor::instance()->addSession(this,getCallgroup());
  }
}

void B2BTransSession::bridgeAudio(AmSessionAudioConnector* audioBridge)
{
  std::ostringstream os;

  if(audioBridge)
  {
    os << "bridging audio bridge=" << audioBridge << "; session=" << this << std::endl;
    DBG("%s",os.str().c_str());

    audioBridge->connectSession(this);
  }
  else
  {
    os << "unbridging audio bridge=" << audioBridge << "; session=" << this << std::endl;
    DBG("%s",os.str().c_str());
    
    setInOut(NULL,NULL);
  }

  if(getDetached())
  {
    AmMediaProcessor::instance()->addSession(this,getCallgroup());
  }
}

void B2BTransSession::call(const std::string& callid, const std::string& to,
    const std::string& from, const std::string& callgroup)
{

  std::ostringstream os;
  os << "calling callid=" << callid << "; to=" << to << "; from=" << from;
  os << "; callgroup=" << callgroup << "; this=" << this << std::endl;
  DBG("%s",os.str().c_str());

  setCallgroup(callgroup);
  setNegotiateOnReply(true);//automatically accept audio
  dlg.local_tag = AmSession::getNewId();
  dlg.callid = callid;
  dlg.local_party = from;
  dlg.remote_party = to;
  dlg.remote_uri = getRUri(to);

  std::string sdp_offer;
  sdp.genRequest(advertisedIP(),RTPStream()->getLocalPort(),sdp_offer);

  dlg.sendRequest("INVITE","application/sdp", sdp_offer);
  start();
  
  if(AmSessionContainer::instance()->addSession(dlg.local_tag,this) != AmSessionContainer::Inserted)
  {
    ERROR("DAMN IT! TODO THROW AN EXCEPTION AND CLEAN UP");
    //throw an exception
  }
}

void B2BTransSession::onSessionStart(const AmSipRequest& req)
{
  std::ostringstream os;
  os << "started session=" << this << std::endl;
  DBG("%s",os.str().c_str());

  for(ListenerIter l = listeners.begin(); l != listeners.end(); ++l)
  {
    (*l)->onStarted(this);
  }
}

void B2BTransSession::onSessionStart(const AmSipReply& reply)
{
  std::ostringstream os;
  os << "started session=" << this << std::endl;
  DBG("%s",os.str().c_str());

  for(ListenerIter l = listeners.begin(); l != listeners.end(); ++l)
  {
    (*l)->onStarted(this);
  }
}

void B2BTransSession::onBye(const AmSipRequest& req)
{
  std::ostringstream os;
  os << "bye session=" << this << std::endl;
  DBG("%s",os.str().c_str());

  //AmSession is stopped in superclass
  AmSession::onBye(req);
}

void B2BTransSession::onSipReply(const AmSipReply& reply, int old_dlg_status, const string& trans_method)
{
  if(dlg.getStatus() == AmSipDialog::Connected)
  {
    //setNegotiateOnReply(true);//automatically accept audio
    //setInOut(NULL,ringTone.get());
  }
  
  AmSession::onSipReply(reply,old_dlg_status,trans_method);
}

/**
 * do not delete AmEvents, even if you created them with a call to new()!
 * it will cause a core dump if you do!
 */
void B2BTransSession::process(AmEvent* evt)
{
  B2BDialoutEvent* devt = dynamic_cast< B2BDialoutEvent* >(evt);
  if(devt)
  {
    std::ostringstream os;
    os << "processing B2BDialoutEvent; this=" << this;
    
    if(devt->event_id == DoConnect)
    {
      os << "; DoConnect" << std::endl;
      DBG("%s",os.str().c_str());

      devt->sess->call(getCallID(),dlg.local_party,dlg.remote_party,getCallgroup());
    }    
    return;
  }

  B2BBridgeAudioEvent* bevt = dynamic_cast< B2BBridgeAudioEvent* >(evt);
  if(bevt)
  {
    std::ostringstream os;
    os << "processing B2BBridgeAudioEvent; this=" << this;
    
    if(bevt->event_id == DoBridge)
    {
      os << "; DoBridge" << std::endl;
      DBG("%s",os.str().c_str());
      
      bridgeAudio(bevt->bridge);
    }    
    return;
  }


  AmSession::process(evt);
}

std::string B2BTransSession::getRUri(const std::string& uri)
{
  AmUriParser parser;
  parser.uri = uri;
  parser.parse_uri();
  parser.dump();

  std::ostringstream os;
  if(uri.find("sip:") != std::string::npos)
    os << "sip:";
  else if(uri.find("sips:") != std::string::npos)
    os << "sips:";
  else if(uri.find("tel:") != std::string::npos)
    os << "tel:";

  if(!parser.uri_user.empty())
  {
    os << parser.uri_user;
    if(!parser.uri_host.empty())
      os << "@";
  }

  if(!parser.uri_host.empty())
  {
    os << parser.uri_host;
    if(!parser.uri_port.empty())
      os << ":" << parser.uri_port;
  }

  //DBG("AS YET UNUSED Reconstructed RUri=%s",os.str().c_str());
  //return parser.uri;
  return os.str();
}
