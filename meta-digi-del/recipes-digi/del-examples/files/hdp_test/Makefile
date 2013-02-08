 # Copyright (c) 2008-2009 Atheros Corporation. All rights reserved.
 #
 # This program is free software; you can redistribute it and/or modify
 # it under the terms of the GNU General Public License version 2 as
 # published by the Free Software Foundation;
 #
 # Software distributed under the License is distributed on an "AS
 # IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 # implied. See the License for the specific language governing
 # rights and limitations under the License.


ROOTFS_DIR = $(strip $(wildcard $(DEL_PROJ_DIR)/build/rootfs))
BINARY     = hdp-test.py

-include $(DEL_PROJ_DIR)/build/.config-$(DEL_PLATFORM)

ifdef CONFIG_APPS_HDP_TEST
all: $(BINARY)

install: $(BINARY)
ifneq ($(ROOTFS_DIR),)
	install -D -m 0755 $< $(ROOTFS_DIR)/usr/bin/$<
else
	@printf "\n"
	@printf "[ WARNING ] $<: installation directory not found, application not installed\n"
	@printf "\n"
	@printf "            This could be due to support for rootfs was not included in the project.\n"
	@printf "\n"
	@printf "            Please, create a project with rootfs support or edit the application makefile\n"
	@printf "            and set the ROOTFS_DIR variable to the installation directory.\n"
	@printf "\n"
endif
else
install:
	@: # Do nothing if the application is not selected
endif # CONFIG_APPS_HDP_TEST

clean:
