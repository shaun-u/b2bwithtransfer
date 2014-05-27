#include "B2BTransSession.h"

#include "AmRingTone.h"
#include "log.h"

B2BTransSession::B2BTransSession()
  : ringTone(new AmRingTone(0,2000,4000,440,480))
{

}

void B2BTransSession::onSessionStart(const AmSipRequest& req)
{
  DBG("session started");

  //TODO remove the following and call listener & put in some destructors to check if and when called
  setInOut(NULL,ringTone.get());
}

void B2BTransSession::onBye(const AmSipRequest& req)
{
  DBG("session stopped");

  //AmSession is stopped in superclass
  AmSession::onBye(req);
}
