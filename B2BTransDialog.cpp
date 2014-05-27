#include "B2BTransDialog.h"
#include "B2BTransSession.h"

#include "log.h"

#include <memory>

B2BTransDialog::B2BTransDialog(const std::string& id)
  : dialogID(id)
{
  DBG("created dialog: dialogID=%s", id.c_str());

  std::auto_ptr< B2BTransSession > session(new B2BTransSession());
  sessions[FROM] = session.release();  
}
  
B2BTransSession* B2BTransDialog::begin()
{
  return sessions[FROM];
}

const std::string& B2BTransDialog::getID() const
{
  return dialogID;
}

std::string B2BTransDialog::toString()
{
  return getID();
}
