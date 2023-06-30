# Copyright (C) 2023 Digi International.
#

# Variable used to generate the tar.gz file. Do not modify.
SWUPDATE_FILES_TARGZ_FILE_NAME = "swupdate-files.tar.gz"

# Initialize variable to provide a custom tar.gz file containing files/dirs to install.
SWUPDATE_FILES_TARGZ_FILE ?= ""

# Initialize variable to store the files/folders that will be part of the SWUpdate package.
SWUPDATE_FILES_LIST ?= ""

# Checks whether SWU update is based on files or not.
def update_based_on_files(d):
    return str(d.getVar('SWUPDATE_FILES_TARGZ_FILE') != "" or d.getVar('SWUPDATE_FILES_LIST') != "").lower()

# Variable that determines if SWU update is based on files or not.
SWUPDATE_IS_FILES_UPDATE = "${@update_based_on_files(d)}"
