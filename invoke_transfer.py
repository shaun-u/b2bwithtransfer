import xmlrpclib

p = xmlrpclib.ServerProxy("http://localhost:8090")

# list all active b2b calss
p.di("b2bwithtrans","list")

# transfer call using callid, local-tag, dest-uri from values presented in list
# callid identifies the call
# local-tag is the SIP From tag of the party performing the transfer
# dest-uri is the SIP URI of the transfer destination
p.di("b2bwithtrans", "transfer", "callid", "local-tag", "dest-uri")

# purge used call dialogs
# For the PoC, used call dialogs are not automatically cleaned up
p.di("b2bwithtrans", "flushdead")

# help showing available methods
p.di("b2bwithtrans", "help")
