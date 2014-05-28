#ifndef _B2BTRANSDIALOG_H_
#define _B2BTRANSDIALOG_H_

#include "B2BTransListeners.h"

#include "AmThread.h"

#include <string>
#include <map>

class B2BTransSession;

class B2BTransDialog : public B2BTransSessionListener
{
  const std::string dialogID;

  typedef enum {FROM,TO,TRANS }RoleIDs;

  //WHO MANAGES THE LIFECYCLE OF AMSESSIONS?
  typedef std::map< RoleIDs,B2BTransSession* > SessionsType;
  SessionsType sessions;
  AmMutex sessionsLock;

  public:
  
  B2BTransDialog(const std::string& id);
  virtual ~B2BTransDialog();

  B2BTransSession* begin();

  void onStarted(B2BTransSession* sess);
  void onStopped(B2BTransSession* sess);

  const std::string& getID() const;

  std::string toString();

};

#endif //_B2BTRANSDIALOG_H_
