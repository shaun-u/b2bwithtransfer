#include "B2BTransDialog.h"

#include "log.h"

B2BTransDialog::B2BTransDialog(const std::string& id)
  : dialogID(id)
{
  DBG("created dialog: dialogID=%s", id.c_str());
}
  
const std::string& B2BTransDialog::getID() const
{
  return dialogID;
}

std::string B2BTransDialog::toString()
{
  return getID();
}
