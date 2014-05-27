#ifndef _B2BTRANSDIALOG_H_
#define _B2BTRANSDIALOG_H_

#include <string>

class B2BTransDialog
{
  const std::string dialogID;

  public:
  
  B2BTransDialog(const std::string& id);

  const std::string& getID() const;

  std::string toString();

};

#endif //_B2BTRANSDIALOG_H_
