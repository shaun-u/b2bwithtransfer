#include "B2BTransDialog.h"
#include "B2BTransSession.h"

#include "AmB2ABSession.h"
#include "log.h"

#include <memory>
#include <sstream>

B2BTransDialog::B2BTransDialog(const std::string& id)
  : dialogID(id), bridge(new AmSessionAudioConnector())
{
  std::ostringstream os;
  os << "created dialog=" << this << "; dialogID=" << id << std::endl;
  DBG("%s",os.str().c_str());

  std::auto_ptr< B2BTransSession > session(new B2BTransSession());
  session->addListener(this);
  sessions[FROM] = session.release();  
}

B2BTransDialog::~B2BTransDialog()
{
  std::ostringstream os;
  os << "destroyed dialog=" << this << "; dialogID=" << dialogID << std::endl;
  DBG("%s",os.str().c_str());
}
  
void B2BTransDialog::setListener(B2BTransDialogListener* dialogListener)
{
  std::ostringstream os;
  os << "dialog=" << this << "; listener=" << dialogListener << std::endl;

  DBG("%s",os.str().c_str());

  listener = dialogListener;
}

B2BTransSession* B2BTransDialog::begin()
{
  return sessions[FROM];
}

void B2BTransDialog::onStarted(B2BTransSession* sess)
{
  std::ostringstream os;
  os << "onStarted sess=" << sess;
  os << "; dialog=" << this;

  sessionsLock.lock();

  if(sessions.size() == 1 && sessions[FROM] == sess)
  {
    os << "'from' leg connected; play ringtone and connect 'to' leg" << std::endl;
    DBG("%s",os.str().c_str());
    
    sess->playRinging();
    
    std::auto_ptr< B2BTransSession > toLeg(new B2BTransSession());
    toLeg->addListener(this);
    sessions[TO] = toLeg.get();  
    
    sess->postEvent(/*give ownership*/new B2BDialoutEvent(toLeg.release()));
  }
  else if(sessions.size() == 2 && (sessions[TO] == sess))
  {
    os << "'to' leg connected; bridge audio between 'to' and 'from' legs" << std::endl;
    DBG("%s",os.str().c_str());

    bridge->connectSession(sess);
    sessions[FROM]->postEvent(/*give ownership*/new B2BBridgeAudioEvent(bridge.get()));
  }
  
  sessionsLock.unlock();
}

void B2BTransDialog::onStopped(B2BTransSession* sess)
{
  std::ostringstream os;
  os << "onStopped sess=" << sess;
  os << "; dialog=" << this;
  os << "; terminating all sessions" << std::endl;
  DBG("%s",os.str().c_str());
  
  sessionsLock.lock();
  
  for(SessionsIter s = sessions.begin(); s != sessions.end(); ++s)
  {
    if(s->second == sess)
      continue;
    
    s->second->postEvent(/*give ownership*/new B2BTerminateEvent());
  }

  sessions.clear();

  sessionsLock.unlock();

  if(listener)
    listener->onTerminated(this);
}

const std::string& B2BTransDialog::getID() const
{
  return dialogID;
}

std::string B2BTransDialog::toString()
{
  return getID();
}
