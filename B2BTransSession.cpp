#include "B2BTransSession.h"
#include "B2BTransListeners.h"

#include "AmRingTone.h"
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
