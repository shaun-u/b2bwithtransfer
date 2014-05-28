#include "B2BTransDialog.h"
#include "B2BTransSession.h"

#include "log.h"

#include <memory>
#include <sstream>

B2BTransDialog::B2BTransDialog(const std::string& id)
  : dialogID(id)
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
  
B2BTransSession* B2BTransDialog::begin()
{
  return sessions[FROM];
}

void B2BTransDialog::onStarted(B2BTransSession* sess)
{
  std::ostringstream os;
  os << "onStarted sess=" << sess;
  os << "; dialog=" << this;
  os << std::endl;
  DBG("%s",os.str().c_str());

  sessionsLock.lock();

  if(sessions.size() == 1 && sessions[FROM] == sess)
  {
    DBG("from leg connected; play ringtone");   
    sess->playRinging(false);
    
    DBG("calling to leg");


    std::auto_ptr< B2BTransSession > toLeg(new B2BTransSession());
    toLeg->addListener(this);
    sessions[TO] = toLeg.get();  
    
    sess->postEvent(/*give ownership*/new B2BDialoutEvent(toLeg.release()));
  }
  
  sessionsLock.unlock();
}

void B2BTransDialog::onStopped(B2BTransSession* sess)
{
  std::ostringstream os;
  os << "onStopped sess=" << sess;
  os << "; dialog=" << this;
  
  sessionsLock.lock();

  sessionsLock.unlock();

  os << std::endl;
  DBG("%s",os.str().c_str());
}

const std::string& B2BTransDialog::getID() const
{
  return dialogID;
}

std::string B2BTransDialog::toString()
{
  return getID();
}
