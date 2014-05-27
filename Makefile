plug_in_name = b2bwithtrans

module_ldflags =
module_cflags =

COREPATH ?=../sems/sems-1.4.3/core
include $(COREPATH)/plug-in/Makefile.app_module
