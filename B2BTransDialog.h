#ifndef _B2BTRANSDIALOG_H_
#define _B2BTRANSDIALOG_H_

#include "B2BTransListeners.h"

#include "AmThread.h"

#include <string>
#include <map>
#include <memory>

class AmSessionAudioConnector;
class B2BTransSession;
class B2BTransDialogListener;

class B2BTransDialog : public B2BTransSessionListener
{
  const std::string dialogID;
  B2BTransDialogListener* listener;

  typedef enum {FROM,TO,TRANS }RoleIDs;

  //WHO MANAGES THE LIFECYCLE OF AMSESSIONS?
  typedef std::map< RoleIDs,B2BTransSession* > SessionsType;
  typedef SessionsType::iterator SessionsIter;
  SessionsType sessions;
  AmMutex sessionsLock;

  std::auto_ptr< AmSessionAudioConnector> bridge;
  
  public:
  
  B2BTransDialog(const std::string& id);
  virtual ~B2BTransDialog();

  void setListener(B2BTransDialogListener* dialogListener);
  B2BTransSession* begin();

  void onStarted(B2BTransSession* sess);
  void onStopped(B2BTransSession* sess);

  const std::string& getID() const;

  std::string toString();

};

#endif //_B2BTRANSDIALOG_H_
