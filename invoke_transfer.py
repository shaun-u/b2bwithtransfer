import xmlrpclib

p = xmlrpclib.ServerProxy("http://localhost:8090")
p.di("click2trans","testingtransfer")

