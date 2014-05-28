#include "B2BTransSession.h"
#include "B2BTransListeners.h"

#include "AmRingTone.h"
#include "AmUriParser.h"
#include "AmSessionContainer.h"
#include "log.h"

#include <sstream>

B2BTransSession::B2BTransSession()
  : ringTone(new AmRingTone(0,2000,4000,440,480))
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

void B2BTransSession::playRinging(bool manual)
{
  std::ostringstream os;
  os << "playing ringing on session=" << this;
  os << "; manual (i.e. must manage media processor)=" << manual << std::endl;
  DBG("%s",os.str().c_str());

  setInOut(NULL,ringTone.get());
  if(manual)
  {

  }
}

void B2BTransSession::playStop(bool manual)
{
  std::ostringstream os;
  os << "play stopping on session=" << this;
  os << "; manual (i.e. must manage media processor)=" << manual << std::endl;
  DBG("%s",os.str().c_str());

  if(manual)
  {

  }
}

void B2BTransSession::call(const std::string& callid, const std::string& to,
    const std::string& from, const std::string& callgroup)
{

  std::ostringstream os;
  os << "calling callid=" << callid << "; to=" << to << "; from=" << from;
  os << "; callgroup=" << callgroup << std::endl;
  DBG("%s",os.str().c_str());

  DBG("completing dialog parameterization");
  dlg.local_tag = AmSession::getNewId();
  dlg.callid = callid;
  dlg.local_party = from;
  dlg.remote_party = to;
  dlg.remote_uri = getRUri(to);

  DBG("generating sdp offer");
  std::string sdp_offer;
  sdp.genRequest(advertisedIP(),RTPStream()->getLocalPort(),sdp_offer);
  DBG("%s",sdp_offer.c_str());

  DBG("sending invite");
  dlg.sendRequest("INVITE","application/sdp", sdp_offer);
  start();
  
  if(AmSessionContainer::instance()->addSession(dlg.local_tag,this) != AmSessionContainer::Inserted)
  {
    ERROR("DAMN IT! TODO THROW AN EXCEPTION AND CLEAN UP");
    //throw an exception
  }
  DBG("complete");
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

void B2BTransSession::onBye(const AmSipRequest& req)
{
  std::ostringstream os;
  os << "bye session=" << this << std::endl;
  DBG("%s",os.str().c_str());

  //AmSession is stopped in superclass
  AmSession::onBye(req);
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
    DBG("processing B2BDialoutEvent");
    
    if(devt->event_id == DoConnect)
    {
      DBG("B2BDialoutEvent is for DoConnect");

      devt->sess->call(getCallID(),dlg.local_party,dlg.remote_party,getCallgroup());
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
