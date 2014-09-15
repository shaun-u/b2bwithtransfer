#include "B2BTransDialog.h"
#include "B2BTransSession.h"

#include "AmB2ABSession.h"
#include "log.h"

#include <memory>
#include <sstream>

B2BTransDialog::B2BTransDialog(const std::string& id)
  : dialogID(id), bridge(new AmSessionAudioConnector()), transferrer(END)
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

void B2BTransDialog::transfer(const std::string& agenttag, const std::string& desturi)
{
  std::ostringstream os;
  os << "starting transfer: desturi=" << desturi;
  os << "; transferrer agenttag=" << agenttag;
  
  std::auto_ptr< B2BTransSession > transLeg(new B2BTransSession());
  transLeg->addListener(this);
  sessions[TRANS] = transLeg.get();  
  
  SessionsIter f = sessions.find(FROM), t = sessions.find(TO), transferree = sessions.end();
  if(f != sessions.end() && t != sessions.end())
  {
    if(t->second->dlg.remote_tag == agenttag)
    {
      os << " - matching: " << t->second->dlg.remote_uri;
      transferrer = t->first;
      transferree = f;
    }
    else if(f->second->dlg.remote_tag == agenttag)
    {
      os << " - matching: " << f->second->dlg.remote_uri;
      transferrer = f->first;
      transferree = t;
    }
    else
    {
      os << " - no matching session found!" << std::endl;
      ERROR("%s",os.str().c_str());
      return;
    }
  }

  DBG("%s",os.str().c_str());

  t->second->unbridgeAudio(bridge.get());
  f->second->unbridgeAudio(bridge.get());
  t->second->playRinging();
  f->second->playRinging();

  transferree->second->postEvent(
    /*give ownership*/new B2BTransferEvent(transLeg.release(),desturi));
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
    
    sess->postEvent(/*give ownership*/new B2BDialoutEvent(toLeg.release(),
      listener->getDstOverride(sess->dlg.local_uri)));
  }
  else if(sessions.size() == 2 && (sessions[TO] == sess))
  {
    os << "'to' leg connected; bridge audio between 'to' and 'from' legs" << std::endl;
    DBG("%s",os.str().c_str());

    bridge->connectSession(sess);
    sessions[FROM]->postEvent(/*give ownership*/new B2BBridgeAudioEvent(bridge.get()));
  }
  else if(sessions.size() == 3 && sessions[TRANS] == sess)
  {
    os << "transfer destination leg connected; bridge audio and bye transferrer" << std::endl;
    DBG("%s",os.str().c_str());

    sessions.erase(TRANS);
    B2BTransSession* oldTransferrer = sessions[transferrer];
    sessions.erase(transferrer);

    B2BTransSession* newFrom = sessions.begin()->second;
    sessions.clear();

    sessions[FROM] = newFrom;
    sessions[TO] = sess;

    oldTransferrer->playStop();
    newFrom->playStop();
    sess->bridgeAudio(bridge.get());
    sessions[FROM]->postEvent(/*give ownership*/new B2BBridgeAudioEvent(bridge.get()));
    oldTransferrer->postEvent(/*give ownership*/new B2BTerminateEvent()); 
  }
  
  sessionsLock.unlock();
}

void B2BTransDialog::onStopped(B2BTransSession* sess)
{
  std::ostringstream os;
  os << "onStopped sess=" << sess;
  os << "; dialog=" << this;
  
  sessionsLock.lock();
  
  if(sessions.size() == 2)
  {
    os << "; normal call termination; terminating other session" << std::endl;
    DBG("%s",os.str().c_str());

    for(SessionsIter s = sessions.begin(); s != sessions.end(); ++s)
    {
      s->second->unbridgeAudio(bridge.get());

      if(s->second == sess)
	continue;
    
      s->second->postEvent(/*give ownership*/new B2BTerminateEvent());
    }
    
    sessions.clear();
  
    sessionsLock.unlock();

    if(listener)
      listener->onTerminated(this);

    return;
  }

  if(sessions.size() == 3)
  {
    //transfer destination failed?
    SessionsIter trans = sessions.find(TRANS);
    if(trans != sessions.end() && trans->second == sess)
    {
      os << "; transfer detination call failed; reconnecting existing sessions" << std::endl;
      DBG("%s",os.str().c_str());
      sessions.erase(TRANS);
      for(SessionsIter s = sessions.begin(); s != sessions.end(); ++s)
      {
	s->second->postEvent(/*give ownership*/new B2BBridgeAudioEvent(bridge.get()));
      }
    
      sessionsLock.unlock();

      return;
    }

    SessionsIter from = sessions.find(FROM);
    SessionsIter to = sessions.find(TO);
    if((from != sessions.end() && from->second == sess) ||
      (to != sessions.end() && to->second == sess))
    {
      //if transferrer hangs up -> continue like blind transfer
      os << "; 'to' or 'from' caller has disconnected;";
      os << "; continuing to connect remaining leg with transfer destination";
      os << " (like blind transfer)" << std::endl;
      DBG("%s",os.str().c_str());

      //Stop ringing
      sess->playStop();

      //Note: TRANS leg must take TO position 
      //in map for onStarted() to behave correctly
      if(to->second == sess)
      {
	sess->playStop();
	sessions[TO] = sessions[TRANS];
	sessions.erase(TRANS);
      }
      else
      {
	sessions[FROM] = sessions[TO];
	sessions[TO] = sessions[TRANS];
	sessions.erase(TRANS);
      }
    
      sessionsLock.unlock();

      return;
    }
  }

  //else
  sessionsLock.unlock();

}

const std::string& B2BTransDialog::getID() const
{
  return dialogID;
}

bool B2BTransDialog::isTagValid(const std::string& tag)
{
  return true;//TODO
}

std::string B2BTransDialog::toString()
{
  std::ostringstream os;
  os << "{";

  sessionsLock.lock();

  SessionsIter i = sessions.find(FROM);
  if(i != sessions.end())
  {
    os << "\"from-tag\":\"" << i->second->dlg.remote_tag << "\",";
  }

  i = sessions.find(TO);
  if(i != sessions.end())
  {
    os << "\"to-tag\":\"" << i->second->dlg.remote_tag << "\",";
  }
  
  sessionsLock.unlock();
  
  os << "\"callid\":\"" << getID() << "\"";
  os << "}";

  return os.str();
}
