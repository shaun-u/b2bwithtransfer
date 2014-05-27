#ifndef _B2BTRANSDIALOG_H_
#define _B2BTRANSDIALOG_H_

#include <string>
#include <map>

class B2BTransSession;

class B2BTransDialog
{
  const std::string dialogID;

  typedef enum {FROM,TO,TRANS }RoleIDs;

  //WHO MANAGES THE LIFECYCLE OF AMSESSIONS?
  typedef std::map< RoleIDs,B2BTransSession* > SessionsType;
  SessionsType sessions;

  public:
  
  B2BTransDialog(const std::string& id);

  B2BTransSession* begin();

  const std::string& getID() const;

  std::string toString();

};

#endif //_B2BTRANSDIALOG_H_
