#ifndef _B2BTRANSSESSION_H_
#define _B2BTRANSSESSION_H_

#include "AmSession.h"

#include <memory>

class AmRingTone;

class B2BTransSession : public AmSession
{
  std::auto_ptr< AmRingTone > ringTone;

  public:
  
  B2BTransSession();

  /** AmSession overrides */
  void onSessionStart(const AmSipRequest& req);
  void onBye(const AmSipRequest& req);
};

#endif //_B2BTRANSSESSION_H_
